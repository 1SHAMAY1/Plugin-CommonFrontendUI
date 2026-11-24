#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "CommonFrontendGameUserSettings.generated.h"

/**
 * Custom Game User Settings class for frontend-specific settings.
 * 
 * This class extends UGameUserSettings to store and manage audio-related user preferences
 * such as master volume, music, sound effects, UI sound levels, and display gamma. 
 * These settings are saved in the user's config and can be applied at runtime.
 */
UCLASS(Blueprintable, BlueprintType)
class COMMONFRONTENDUI_API UCommonFrontendGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:

	/** Gets the current display gamma setting. */
	UFUNCTION(BlueprintCallable, Category = "Video")
	float GetCurrentDisplayGamma() const;

	/** Sets the display gamma level. */
	UFUNCTION(BlueprintCallable, Category = "Video")
	void SetCurrentDisplayGamma(float InNewGamma);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	float GetMasterVolume() const { return MasterVolume; }
	
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMasterVolume(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	float GetMusicVolume() const { return MusicVolume; }
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMusicVolume(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	float GetSFXVolume() const { return SFXVolume; }
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetSFXVolume(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	float GetUIVolume() const { return UIVolume; }
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetUIVolume(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	float GetVoiceChatVolume() const { return VoiceChatVolume; }
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetVoiceChatVolume(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	bool GetVoiceChatEnabled() const { return bVoiceChatEnabled; }
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetVoiceChatEnabled(bool bEnable) { bVoiceChatEnabled = bEnable; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	bool GetPushToTalkEnabled() const { return bPushToTalkEnabled; }
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetPushToTalkEnabled(bool bEnable) { bPushToTalkEnabled = bEnable; }

protected:
	UPROPERTY(Config)
	float MasterVolume = 1.0f;

	UPROPERTY(Config)
	float MusicVolume = 0.8f;

	UPROPERTY(Config)
	float SFXVolume = 0.8f;

	UPROPERTY(Config)
	float UIVolume = 1.0f;

	UPROPERTY(Config)
	float VoiceChatVolume = 1.0f;

	UPROPERTY(Config)
	bool bVoiceChatEnabled = true;

	UPROPERTY(Config)
	bool bPushToTalkEnabled = false;

	/** Keeps preloaded audio assets alive to prevent garbage collection */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> PreloadedAssets;

	bool bAudioAssetsPreloaded = false;

public:
	
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	virtual UWorld* GetWorld() const override;
	/**
	 * Preloads all audio assets defined in DeveloperSettings to avoid hitches during runtime.
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PreloadAudioAssets();

	/**
	 * Applies audio settings based on the bindings defined in CommonFrontendDeveloperSettings.
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void ApplyAudioSettings();

protected:
	/**
	 * Called after ApplySettings and before RequestUIUpdate and SaveSettings to allow Blueprint extensions.
	 * Override this in BP to add custom logic when settings are applied.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CommonFrontendGameUserSettings")
	void ApplySettingsExt();
};
