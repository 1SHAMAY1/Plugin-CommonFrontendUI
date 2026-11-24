#include "Utility/Async/AsyncAction_PushChoiceScreen.h"

#include "Subsystems/CommonFrontendUI_Subsystem.h"

UAsyncAction_PushChoiceScreen* UAsyncAction_PushChoiceScreen::PushChoiceScreen(
	const UObject* WorldContextObject,
	FText ScreenTitle,
	FText ScreenMessage,
	const TArray<FString>& ButtonLabels)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UAsyncAction_PushChoiceScreen* Action = NewObject<UAsyncAction_PushChoiceScreen>();
	Action->CachedOwningWorld = WorldContextObject->GetWorld();
	Action->CachedTitle = ScreenTitle;
	Action->CachedMessage = ScreenMessage;
	Action->CachedButtonLabels = ButtonLabels;
	return Action;
}

void UAsyncAction_PushChoiceScreen::Activate()
{
	auto* Subsystem = UCommonFrontendUI_Subsystem::Get(CachedOwningWorld.Get());
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("PushChoiceScreen: Subsystem not available"));
		SetReadyToDestroy();
		return;
	}

	Subsystem->PushChoiceScreenToModalStackAsync(
		CachedTitle,
		CachedMessage,
		CachedButtonLabels,
		[this](const FString& SelectedLabel)
		{
			OnButtonClicked.Broadcast(SelectedLabel);
			SetReadyToDestroy();
		}
	);
}