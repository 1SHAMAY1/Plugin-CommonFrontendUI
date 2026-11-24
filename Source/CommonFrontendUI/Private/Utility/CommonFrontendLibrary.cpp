#include "Utility/CommonFrontendLibrary.h"

#include "Utility/DeveloperSettings/CommonFrontendDeveloperSettings.h"
#include "Utility/CommonFrontendGameUserSettings.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"

UCommonFrontendGameUserSettings* UCommonFrontendLibrary::GetCommonFrontendGameUserSettings()
{
	if (GEngine)
	{
		return Cast<UCommonFrontendGameUserSettings>(GEngine->GetGameUserSettings());
	}
	return nullptr;
}

bool UCommonFrontendLibrary::HasValidBenchmarkSettings()
{
	if (UGameUserSettings* GUS = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		const float CPU = GUS->GetLastCPUBenchmarkResult();
		const float GPU = GUS->GetLastGPUBenchmarkResult();

		return (CPU != -1.0f && GPU != -1.0f);
	}
	return false;
}

const bool UCommonFrontendLibrary::ShouldEnableDebug()
{
	return GetDefault<UCommonFrontendDeveloperSettings>()->bEnableDebugLogs;
}
