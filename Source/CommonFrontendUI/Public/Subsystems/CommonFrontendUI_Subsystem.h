#pragma once

#include "CoreMinimal.h"
#include "Data/CommonFrontendUI_StructsAndEnums.h"
#include "GameplayTagContainer.h"
#include "Data/CommonFrontendUI_DataConfig.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Widgets/CommonFrontendActivatableWidget.h"
#include "CommonFrontendUI_Subsystem.generated.h"

class UCommonFrontendButtonBase;
class UCommonFrontendActivatableWidget;
class UCommonFrontendUI_DataConfig;
class UCommonPrimaryLayoutWidget;

/** Delegate triggered after a primary layout widget is loaded from config */
DECLARE_DELEGATE_OneParam(FOnWidgetConfigLoaded, UCommonPrimaryLayoutWidget*);

/** Dynamic multicast delegate for broadcasting button description text updates */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnButtonDescriptionTextUpdatedDelegate,
	UCommonFrontendButtonBase*, BroadcastingButton,
	FText, DescriptionText);

/**
 * GameInstanceSubsystem responsible for managing the UI flow and widget stack 
 * in a modular frontend system. Handles loading data configs, pushing widgets,
 * and providing access to runtime UI config and state.
 */
UCLASS()
class COMMONFRONTENDUI_API UCommonFrontendUI_Subsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Static utility function to get this subsystem from any context object */
	static UCommonFrontendUI_Subsystem* Get(const UObject* WorldContext);

	/** Subsystem initialization – triggers async config load if needed */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Determines if this subsystem should be created based on context */
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** Async callback called once the UI data config soft reference is loaded */
	void OnDataConfigLoaded(TSoftObjectPtr<UCommonFrontendUI_DataConfig> LoadedAsset);

	/**
	 * Registers widget data config and loads the primary layout widget asynchronously.
	 * @param InConfig - The config data asset to register
	 * @param OnLoadedCallback - Callback to receive the layout widget once created
	 */
	void RegisterWidgetDataConfigAsync(UCommonFrontendUI_DataConfig* InConfig, FOnWidgetConfigLoaded OnLoadedCallback);

	/**
	 * Pushes a widget (specified as a soft class reference) to the stack associated with a tag.
	 * Loads the widget asynchronously before pushing.
	 * @param StackTag - Tag representing the target stack
	 * @param WidgetClass - Soft class reference to the widget
	 * @param AsyncPushActionCallback - Callback invoked once widget is pushed or failed
	 */
	void PushSoftWidgetToStackAsync(
		const FGameplayTag& StackTag,
		TSoftClassPtr<UCommonFrontendActivatableWidget> WidgetClass,
		TFunction<void(EAsyncPushActionState, UCommonFrontendActivatableWidget*)> AsyncPushActionCallback);

	/**
	 * Pushes a single-use widget asynchronously (not bound to a specific stack).
	 * @param AssociatedTag - Tag to track or filter the widget
	 * @param AsyncPushActionCallback - Callback with state and instance
	 */
	void PushSoftWidgetAsync(
		const FGameplayTag& AssociatedTag,
		TFunction<void(EAsyncPushActionState, UCommonFrontendActivatableWidget*)> AsyncPushActionCallback);

	/**
	 * Displays a user-defined choice screen on the modal stack asynchronously.
	 * @param InScreenTitle - Title text
	 * @param InScreenMsg - Message body text
	 * @param InButtons - Array of button labels (strings). If empty, a single "Back" button will be shown.
	 * @param ButtonClickedCallback - Callback receives the selected label as string
	 */
	void PushChoiceScreenToModalStackAsync(
		const FText& InScreenTitle,
		const FText& InScreenMsg,
		const TArray<FString>& InButtons,
		TFunction<void(const FString&)> ButtonClickedCallback);

	/** Returns the current primary layout widget being displayed (can be null) */
	UFUNCTION(BlueprintPure, Category = "FrontendUI")
	UCommonPrimaryLayoutWidget* GetCurrentLayoutWidget() const { return CurrentLayOutWidget; }

	/** Returns the current UI data config asset (can be null if not loaded) */
	UFUNCTION(BlueprintPure, Category = "FrontendUI")
	UCommonFrontendUI_DataConfig* GetWidgetDataConfig() const { return CurrentDataConfig; }

	/**
	 * Gets the widget stack data associated with a specific widget class.
	 * @param WidgetToCheck - Widget class to lookup
	 * @return Stack data for that widget
	 */
	UFUNCTION(BlueprintPure, Category = "FrontendUI")
	FWidgetStackData GetWidgetStackDataForWidget(TSubclassOf<UCommonFrontendActivatableWidget> WidgetToCheck) const;

	/**
	 * Gets the input config associated with a specific widget class.
	 * @param WidgetToCheck - Widget class to check
	 * @return Input configuration info
	 */
	UFUNCTION(BlueprintPure, Category = "FrontendUI")
	FUIInputConfig GetInputConfigForWidget(TSubclassOf<UCommonFrontendActivatableWidget> WidgetToCheck) const;

	/** Broadcasts when a button's description text needs to be updated in the UI */
	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdated;

private:
	/** The currently active primary layout widget */
	UPROPERTY(Transient)
	TObjectPtr<UCommonPrimaryLayoutWidget> CurrentLayOutWidget;

	/** The currently active UI data configuration asset (loaded async) */
	UPROPERTY(Transient)
	TObjectPtr<UCommonFrontendUI_DataConfig> CurrentDataConfig;

	/** Cached widget stack data per widget class for runtime access */
	UPROPERTY(Transient)
	TMap<TSubclassOf<UCommonFrontendActivatableWidget>, FWidgetStackData> LoadedWidgetMap;
};
