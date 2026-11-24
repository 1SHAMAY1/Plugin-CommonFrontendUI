#include "Utility/Async/AsyncAction_PushFrontendWidgetToStack.h"
#include "Subsystems/CommonFrontendUI_Subsystem.h"

UAsyncAction_PushFrontendWidgetToStack* UAsyncAction_PushFrontendWidgetToStack::PushWidgetToStack(
	UObject* WorldContextObject,
	UPARAM(meta = (Categories = "CFUI.WidgetStack")) FGameplayTag StackTag,
	TSoftClassPtr<UCommonFrontendActivatableWidget> WidgetClass,
	bool bAutoFocus)
{
	if (!WorldContextObject || !StackTag.IsValid() || WidgetClass.IsNull())
	{
		return nullptr;
	}

	UAsyncAction_PushFrontendWidgetToStack* Action = NewObject<UAsyncAction_PushFrontendWidgetToStack>();
	Action->WorldContextObject = WorldContextObject;
	Action->StackTag = StackTag;
	Action->WidgetClass = WidgetClass;
	Action->bAutoFocus = bAutoFocus;
	return Action;
}

void UAsyncAction_PushFrontendWidgetToStack::Activate()
{
	if (!WorldContextObject || !StackTag.IsValid() || WidgetClass.IsNull())
	{
		OnFailed.Broadcast();
		SetReadyToDestroy();
		return;
	}

	UCommonFrontendUI_Subsystem* Subsystem = UCommonFrontendUI_Subsystem::Get(WorldContextObject);
	if (!Subsystem)
	{
		OnFailed.Broadcast();
		SetReadyToDestroy();
		return;
	}

	Subsystem->PushSoftWidgetToStackAsync(
		StackTag,
		WidgetClass,
		[this](EAsyncPushActionState State, UCommonFrontendActivatableWidget* Widget)
		{
			InternalCallback(State, Widget);
		});
}

void UAsyncAction_PushFrontendWidgetToStack::InternalCallback(EAsyncPushActionState State, UCommonFrontendActivatableWidget* Widget)
{
	if (!Widget)
	{
		OnFailed.Broadcast();
		SetReadyToDestroy();
		return;
	}

	switch (State)
	{
	case EAsyncPushActionState::OnCreated:
		OnCreated.Broadcast(Widget);
		break;

	case EAsyncPushActionState::OnPushed:
		if (bAutoFocus)
		{
			if (UWidget* FocusWidget = Widget->GetDesiredFocusTarget())
			{
				FocusWidget->SetFocus();
			}
		}
		OnPushed.Broadcast(Widget);
		SetReadyToDestroy();
		break;

	default:
		OnFailed.Broadcast();
		SetReadyToDestroy();
		break;
	}
}
