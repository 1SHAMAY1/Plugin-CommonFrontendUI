#pragma once

#include "CoreMinimal.h"
#include "Data/CommonFrontendUI_StructsAndEnums.h"
#include "Engine/DeveloperSettings.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "CommonFrontendDeveloperSettings.generated.h"

class UCommonFrontendUI_DataConfig;

/**
 * Developer Settings for the Common Frontend UI plugin.
 * 
 * These settings allow customization of default UI configuration, audio setup, localization tables,
 * image descriptions, and credits data—all configurable via the Project Settings panel under Game.
 */
UCLASS(Config = Game, DefaultConfig, EditInlineNew)
class COMMONFRONTENDUI_API UCommonFrontendDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UCommonFrontendDeveloperSettings();

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadOnly, Category = "Frontend UI")
	FSoftObjectPath DefaultFrontendUI_DataConfig;

	/** Global SoundMix used to control overall audio levels and priorities. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Audio", meta = (AllowedClasses = "/Script/Engine.SoundMix"))
	TSoftObjectPtr<USoundMix> MasterSoundMix;

	/** Optional: Additional custom resolutions to include in the settings menu (e.g., 2560x1080). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Video")
	TArray<FIntPoint> CustomResolutions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Audio", meta = (TitleProperty = "VolumeSettingName"))
	TArray<FCommonFrontendAudioBinding> AudioBindings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Debug")
	bool bEnableDebugLogs;
};
