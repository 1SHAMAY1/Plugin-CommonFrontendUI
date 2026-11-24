#include "Utility/CommonFrontendGameUserSettings.h"

#include "Utility/DeveloperSettings/CommonFrontendDeveloperSettings.h"
#include "AudioDevice.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/StreamableManager.h"

void UCommonFrontendGameUserSettings::PreloadAudioAssets()
{
	if (bAudioAssetsPreloaded)
	{
		return;
	}
	bAudioAssetsPreloaded = true;

	const UCommonFrontendDeveloperSettings* DevSettings = GetDefault<UCommonFrontendDeveloperSettings>();
	if (!DevSettings)
	{
		return;
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	TArray<FSoftObjectPath> AssetsToLoad;

	// Collect assets to load
	if (!DevSettings->MasterSoundMix.IsNull())
	{
		AssetsToLoad.Add(DevSettings->MasterSoundMix.ToSoftObjectPath());
	}

	for (const FCommonFrontendAudioBinding& Binding : DevSettings->AudioBindings)
	{
		if (!Binding.SoundClass.IsNull())
		{
			AssetsToLoad.Add(Binding.SoundClass);
		}
		if (!Binding.SoundMixOverride.IsNull())
		{
			AssetsToLoad.Add(Binding.SoundMixOverride);
		}
	}

	if (AssetsToLoad.IsEmpty())
	{
		return;
	}

	// Request async load
	Streamable.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateWeakLambda(this, [this, AssetsToLoad]()
	{
		// Keep loaded assets alive
		for (const FSoftObjectPath& AssetPath : AssetsToLoad)
		{
			if (UObject* LoadedAsset = AssetPath.ResolveObject())
			{
				PreloadedAssets.AddUnique(LoadedAsset);
			}
		}
		
		// Apply settings once assets are ready
		ApplyAudioSettings();
	}));
}



float UCommonFrontendGameUserSettings::GetCurrentDisplayGamma() const
{
	if (GEngine)
	{
		return GEngine->GetDisplayGamma();
	}

	return 0.0f;
}

void UCommonFrontendGameUserSettings::SetCurrentDisplayGamma(float InNewGamma)
{
	if (GEngine)
	{
		GEngine->DisplayGamma = InNewGamma;
	}
}

void UCommonFrontendGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	PreloadAudioAssets();

	ApplyResolutionSettings(bCheckForCommandLineOverrides);
	ApplyNonResolutionSettings();
	
	ApplyAudioSettings();
	ApplySettingsExt();
	RequestUIUpdate();
	SaveSettings();
}

void UCommonFrontendGameUserSettings::ApplySettingsExt_Implementation()
{
}

void UCommonFrontendGameUserSettings::SetMasterVolume(float NewValue)
{
	MasterVolume = NewValue;
	ApplyAudioSettings();
}

void UCommonFrontendGameUserSettings::SetMusicVolume(float NewValue)
{
	MusicVolume = NewValue;
	ApplyAudioSettings();
}

void UCommonFrontendGameUserSettings::SetSFXVolume(float NewValue)
{
	SFXVolume = NewValue;
	ApplyAudioSettings();
}

void UCommonFrontendGameUserSettings::SetUIVolume(float NewValue)
{
	UIVolume = NewValue;
	ApplyAudioSettings();
}

void UCommonFrontendGameUserSettings::SetVoiceChatVolume(float NewValue)
{
	VoiceChatVolume = NewValue;
	ApplyAudioSettings();
}

void UCommonFrontendGameUserSettings::ApplyAudioSettings()
{
	const UCommonFrontendDeveloperSettings* DevSettings = GetDefault<UCommonFrontendDeveloperSettings>();
	if (!DevSettings)
	{
		return;
	}

	for (const FCommonFrontendAudioBinding& Binding : DevSettings->AudioBindings)
	{
		// Retrieve the current volume value
		float CurrentVolume = 1.0f;
		
		if (!Binding.GetterFunctionName.IsNone())
		{
			if (UFunction* Func = FindFunction(Binding.GetterFunctionName))
			{
				struct FFloatReturn { float Val; };
				FFloatReturn RetVal{ 1.0f };
				ProcessEvent(Func, &RetVal);
				CurrentVolume = RetVal.Val;
			}
		}
		else if (!Binding.VolumeSettingName.IsNone())
		{
			if (FDoubleProperty* Prop = FindFProperty<FDoubleProperty>(GetClass(), Binding.VolumeSettingName))
			{
				CurrentVolume = (float)Prop->GetPropertyValue_InContainer(this);
			}
			else if (FFloatProperty* FloatProp = FindFProperty<FFloatProperty>(GetClass(), Binding.VolumeSettingName))
			{
				CurrentVolume = FloatProp->GetPropertyValue_InContainer(this);
			}
		}

		if (USoundClass* LoadedSoundClass = Cast<USoundClass>(Binding.SoundClass.ResolveObject()))
		{
			USoundMix* TargetMix = nullptr;
			if (!Binding.SoundMixOverride.IsNull())
			{
				TargetMix = Cast<USoundMix>(Binding.SoundMixOverride.ResolveObject());
			}
			else if (!DevSettings->MasterSoundMix.IsNull())
			{
				TargetMix = Cast<USoundMix>(DevSettings->MasterSoundMix.Get());
			}

			// If assets aren't loaded yet (async in progress), we skip application to avoid blocking.
			// They will be applied when PreloadAudioAssets finishes.
			if (TargetMix)
			{
				UGameplayStatics::PushSoundMixModifier(this, TargetMix);
				
				UGameplayStatics::SetSoundMixClassOverride(
					this, 
					TargetMix, 
					LoadedSoundClass, 
					CurrentVolume, 
					1.0f, 
					0.0f
				);
			}
		}
	}
}

UWorld* UCommonFrontendGameUserSettings::GetWorld() const
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}

	if (GEngine)
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::Game || Context.WorldType == EWorldType::PIE)
			{
				if (Context.World())
				{
					return Context.World();
				}
			}
		}
	}

	return nullptr;
}
