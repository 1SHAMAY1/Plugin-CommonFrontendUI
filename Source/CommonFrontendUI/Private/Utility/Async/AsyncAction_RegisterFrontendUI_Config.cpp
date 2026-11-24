#include "Utility/Async/AsyncAction_RegisterFrontendUI_Config.h"
#include "Subsystems/CommonFrontendUI_Subsystem.h"

UAsyncAction_RegisterFrontendUI_Config* UAsyncAction_RegisterFrontendUI_Config::RegisterFrontendUI_Config(
	UObject* WorldContextObject,
	UCommonFrontendUI_DataConfig* Config)
{
	if (!WorldContextObject || !Config)
	{
		return nullptr;
	}

	UAsyncAction_RegisterFrontendUI_Config* Action = NewObject<UAsyncAction_RegisterFrontendUI_Config>();
	Action->WorldContextObject = WorldContextObject;
	Action->Config = Config;
	return Action;
}

void UAsyncAction_RegisterFrontendUI_Config::Activate()
{
	if (!WorldContextObject || !Config)
	{
		OnFailure.Broadcast(nullptr);
		SetReadyToDestroy();
		return;
	}

	UCommonFrontendUI_Subsystem* Subsystem = UCommonFrontendUI_Subsystem::Get(WorldContextObject);
	if (!Subsystem)
	{
		OnFailure.Broadcast(nullptr);
		SetReadyToDestroy();
		return;
	}

	Subsystem->RegisterWidgetDataConfigAsync(
		Config,
		FOnWidgetConfigLoaded::CreateUObject(this, &UAsyncAction_RegisterFrontendUI_Config::OnComplete_Internal));
}

void UAsyncAction_RegisterFrontendUI_Config::OnComplete_Internal(UCommonPrimaryLayoutWidget* LoadedWidget)
{
	if (LoadedWidget)
	{
		OnSuccess.Broadcast(LoadedWidget);
	}
	else
	{
		OnFailure.Broadcast(nullptr);
	}

	SetReadyToDestroy();
}
