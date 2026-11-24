#include "Utility/DeveloperSettings/CommonFrontendDeveloperSettings.h"

UCommonFrontendDeveloperSettings::UCommonFrontendDeveloperSettings()
{
	CategoryName = TEXT("Game");
	SectionName = TEXT("Common Frontend UI");

	// Default Master Mix
	MasterSoundMix = FSoftObjectPath(TEXT("/CommonFrontendUI/Audio/SCM_Default.SCM_Default"));

	// Default Audio Bindings
	AudioBindings.Add({ FName("MasterVolume"), FSoftObjectPath(TEXT("/CommonFrontendUI/Audio/SC_Master_Plugin.SC_Master_Plugin")), FSoftObjectPath(), FName("GetMasterVolume"), FName("SetMasterVolume") });
	AudioBindings.Add({ FName("MusicVolume"), FSoftObjectPath(TEXT("/CommonFrontendUI/Audio/SC_Music_Plugin.SC_Music_Plugin")), FSoftObjectPath(), FName("GetMusicVolume"), FName("SetMusicVolume") });
	AudioBindings.Add({ FName("SFXVolume"), FSoftObjectPath(TEXT("/CommonFrontendUI/Audio/SC_SFX_Plugin.SC_SFX_Plugin")), FSoftObjectPath(), FName("GetSFXVolume"), FName("SetSFXVolume") });
	AudioBindings.Add({ FName("UIVolume"), FSoftObjectPath(TEXT("/CommonFrontendUI/Audio/SC_UI_Plugin.SC_UI_Plugin")), FSoftObjectPath(), FName("GetUIVolume"), FName("SetUIVolume") });
	AudioBindings.Add({ FName("VoiceChatVolume"), FSoftObjectPath(), FSoftObjectPath(), FName("GetVoiceChatVolume"), FName("SetVoiceChatVolume") });
}
