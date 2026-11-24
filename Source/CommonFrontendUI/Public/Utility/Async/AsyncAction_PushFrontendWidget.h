#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "Subsystems/CommonFrontendUI_Subsystem.h"
#include "AsyncAction_PushFrontendWidget.generated.h"

class UCommonFrontendActivatableWidget;

/** Delegate broadcast when the widget is created but not yet pushed */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFrontendWidgetCreatedByTag, UCommonFrontendActivatableWidget*, CreatedWidget);

/** Delegate broadcast after the widget has been successfully pushed to the stack */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFrontendWidgetPushedByTag, UCommonFrontendActivatableWidget*, PushedWidget);

/** Delegate broadcast if the widget push operation fails (e.g. tag not mapped) */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFrontendWidgetPushFailedByTag);

/**
 * Async Blueprint node to push a frontend widget to the screen stack using an associated gameplay tag.
 * Supports dynamic loading and async confirmation once created and pushed.
 */
UCLASS()
class COMMONFRONTENDUI_API UAsyncAction_PushFrontendWidget : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	/**
	 * Pushes a frontend widget to the active screen stack based on the given associated tag.
	 * @param WorldContextObject	Context object (usually player controller or world).
	 * @param AssociatedTag			The gameplay tag used to identify the widget entry.
	 * @param bAutoFocus			Whether to auto-focus the widget after push.
	 */
	UFUNCTION(BlueprintCosmetic, BlueprintCallable, Category = "CommonFrontendUI", meta = (
		BlueprintInternalUseOnly = "true",
		WorldContext = "WorldContextObject",
		DisplayName = "Push Frontend Widget by Tag"
	))
	static UAsyncAction_PushFrontendWidget* PushWidget(
		UObject* WorldContextObject,
		UPARAM(meta = (Categories = "CFUI.AssociatedTag")) FGameplayTag AssociatedTag,
		bool bAutoFocus = false
	);

	/** Called when the widget is successfully created */
	UPROPERTY(BlueprintAssignable, Category = "CommonFrontendUI|Async")
	FOnFrontendWidgetCreatedByTag OnCreated;

	/** Called when the widget is successfully pushed to the stack */
	UPROPERTY(BlueprintAssignable, Category = "CommonFrontendUI|Async")
	FOnFrontendWidgetPushedByTag OnPushed;

	/** Called if widget creation or push fails */
	UPROPERTY(BlueprintAssignable, Category = "CommonFrontendUI|Async")
	FOnFrontendWidgetPushFailedByTag OnFailed;

	/** BlueprintAsyncActionBase override to activate the async node */
	virtual void Activate() override;

private:

	/** Cached world context */
	UPROPERTY()
	UObject* WorldContextObject = nullptr;

	/** Gameplay tag to resolve widget class via data config */
	UPROPERTY()
	FGameplayTag AssociatedTag;

	/** Should the widget receive focus after pushing? */
	UPROPERTY()
	bool bAutoFocus = false;

	/** Callback for push completion from UI subsystem */
	void InternalCallback(EAsyncPushActionState State, UCommonFrontendActivatableWidget* Widget);
};
