#include "Utility/Async/AsyncAction_PushFrontendWidget.h"

#include "Subsystems/CommonFrontendUI_Subsystem.h"

UAsyncAction_PushFrontendWidget* UAsyncAction_PushFrontendWidget::PushWidget(
	UObject* InWorldContextObject,
	UPARAM(meta = (Categories = "CFUI.AssociatedTag")) FGameplayTag InAssociatedTag,
	bool bInAutoFocus)
{
	if (!InWorldContextObject || !InAssociatedTag.IsValid())
	{
		return nullptr;
	}

	UAsyncAction_PushFrontendWidget* Node = NewObject<UAsyncAction_PushFrontendWidget>();
	Node->WorldContextObject = InWorldContextObject;
	Node->AssociatedTag = InAssociatedTag;
	Node->bAutoFocus = bInAutoFocus;
	return Node;
}

void UAsyncAction_PushFrontendWidget::Activate()
{
	if (!WorldContextObject || !AssociatedTag.IsValid())
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

	Subsystem->PushSoftWidgetAsync(
		AssociatedTag,
		[this](EAsyncPushActionState State, UCommonFrontendActivatableWidget* Widget)
		{
			InternalCallback(State, Widget);
		});
}

void UAsyncAction_PushFrontendWidget::InternalCallback(EAsyncPushActionState State, UCommonFrontendActivatableWidget* Widget)
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
