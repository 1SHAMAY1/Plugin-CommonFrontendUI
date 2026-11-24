#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "Subsystems/CommonFrontendUI_Subsystem.h"
#include "AsyncAction_PushFrontendWidgetToStack.generated.h"

class UCommonFrontendActivatableWidget;

/** Delegate broadcast when the widget is successfully created from the soft class */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFrontendWidgetCreated, UCommonFrontendActivatableWidget*, CreatedWidget);

/** Delegate broadcast when the widget is successfully pushed to the specified stack */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFrontendWidgetPushed, UCommonFrontendActivatableWidget*, PushedWidget);

/** Delegate broadcast when widget creation or pushing fails */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFrontendWidgetPushFailed);

/**
 * Async Blueprint node to push a specific frontend widget class to a UI stack.
 * Unlike PushWidgetByTag, this version allows pushing arbitrary widget classes at runtime to a named stack.
 */
UCLASS()
class COMMONFRONTENDUI_API UAsyncAction_PushFrontendWidgetToStack : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	/**
	 * Pushes the specified widget class to a UI stack defined by a gameplay tag.
	 *
	 * @param WorldContextObject	Context object (usually player controller or world).
	 * @param StackTag				The gameplay tag representing the UI stack to push to.
	 * @param WidgetClass			The soft class of the widget to create and push.
	 * @param bAutoFocus			Should the widget be focused automatically.
	 */
	UFUNCTION(BlueprintCosmetic, BlueprintCallable, Category = "CommonFrontendUI", meta = (
		BlueprintInternalUseOnly = "true",
		WorldContext = "WorldContextObject",
		DisplayName = "Push Frontend Widget To Stack"
	))
	static UAsyncAction_PushFrontendWidgetToStack* PushWidgetToStack(
		UObject* WorldContextObject,
		UPARAM(meta = (Categories = "CFUI.WidgetStack")) FGameplayTag StackTag,
		TSoftClassPtr<UCommonFrontendActivatableWidget> WidgetClass,
		bool bAutoFocus = false
	);

	/** Called when the widget is created (after soft loading) */
	UPROPERTY(BlueprintAssignable, Category = "CommonFrontendUI|Async")
	FOnFrontendWidgetCreated OnCreated;

	/** Called when the widget is successfully pushed to the stack */
	UPROPERTY(BlueprintAssignable, Category = "CommonFrontendUI|Async")
	FOnFrontendWidgetPushed OnPushed;

	/** Called when the widget could not be loaded or pushed */
	UPROPERTY(BlueprintAssignable, Category = "CommonFrontendUI|Async")
	FOnFrontendWidgetPushFailed OnFailed;

	/** BlueprintAsyncActionBase override to begin async work */
	virtual void Activate() override;

private:

	/** Cached world context for asset loading and subsystem access */
	UPROPERTY()
	UObject* WorldContextObject = nullptr;

	/** Widget class to push (soft reference) */
	UPROPERTY()
	TSoftClassPtr<UCommonFrontendActivatableWidget> WidgetClass;

	/** Tag representing the UI stack to push into */
	UPROPERTY()
	FGameplayTag StackTag;

	/** Whether the widget should auto-focus after being pushed */
	UPROPERTY()
	bool bAutoFocus = false;

	/** Callback used internally by the UI subsystem after push attempt */
	void InternalCallback(EAsyncPushActionState State, UCommonFrontendActivatableWidget* Widget);
};
