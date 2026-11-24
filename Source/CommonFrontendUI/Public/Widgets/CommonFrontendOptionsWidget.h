#pragma once

#include "CoreMinimal.h"
#include "CommonFrontendActivatableWidget.h"
#include "Engine/DataTable.h"
#include "Data/CommonFrontendUI_StructsAndEnums.h"
#include "CommonFrontendOptionsWidget.generated.h"

class UListDataObjectBase;
class UCommonFrontendDataRegistry;
class UCommonFrontendDetails;
class UCommonFrontendListView;
class UCommonFrontendTabListWidgetBase;
class UCommonFrontendSettingsRegistry;
struct FStreamableHandle;

/**
 * UCommonFrontendOptionsWidget
 *
 * This widget represents the core options/settings screen used within the Common Frontend UI system.
 * It handles displaying and interacting with a configurable list of option tabs, setting categories,
 * and individual setting items. It also supports selection, hover, and detail display for the selected
 * setting item and integrates with the input system for back/reset functionality.
 *
 * Key Features:
 * - Tab-based category navigation using UCommonFrontendTabListWidgetBase.
 * - List view display and selection via UCommonFrontendListView.
 * - Details panel display for selected option using UCommonFrontendDetails.
 * - Integration with UCommonFrontendDataRegistry for dynamic data loading and modification tracking.
 * - Input binding for reset and back navigation.
 *
 * Expected to be extended or used as-is in projects that adopt the Common Frontend UI plugin structure.
 */
UCLASS()
class COMMONFRONTENDUI_API UCommonFrontendOptionsWidget : public UCommonFrontendActivatableWidget
{
	GENERATED_BODY()

protected:
	/** Called once after widget construction and binding. Used for setup logic. */
	virtual void NativeOnInitialized() override;

	/** Called when this widget becomes the active/visible UI. */
	virtual void NativeOnActivated() override;

	/** Called when this widget is deactivated or hidden. */
	virtual void NativeOnDeactivated() override;

	/** Optionally override to define which widget should receive focus by default. */
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

private:
	/** Returns the existing data registry or creates a new one if not yet available. */
	UCommonFrontendDataRegistry* GetOrCreateDataRegistry();

	/** Handles user-triggered reset action. Resets all tracked settings to default. */
	void OnResetBoundActionTriggered();

	/** Handles user-triggered back action. Used to navigate away from options screen. */
	void OnBackBoundActionTriggered();

	/** Callback when a new tab is selected in the tab list widget. */
	UFUNCTION()
	void OnOptionsTabSelected(FName TabId);

	/** Callback when a list item is hovered. */
	void OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered);
	
	/** Allows nested lists to report their selection to the main widget. */
	void HandleNestedItemSelection(UObject* InSelectedItem);
	
	/** Callback when a list item is selected. */
	void OnListViewItemSelected(UObject* InSelectedItem);

	/** Attempts to retrieve the widget class name used for a given list item. For debugging/logging. */
	FString TryGetEntryWidgetClassName(UObject* InOwningListItem) const;

	/** Callback when the underlying data list is modified (added/removed/reset/etc). */
	void OnListViewListDataModified(UListDataObjectBase* ModifiedData, EOptionsListDataModifyReason ModifyReason);

	/** Tab list widget used to display different option categories. */
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Frontend Options|Widgets")
	UCommonFrontendTabListWidgetBase* TabListWidget_OptionsTabs;

	/** Optional registry asset to dynamically populate settings tabs. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Options", meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UCommonFrontendSettingsRegistry> SettingsRegistry;

	/** List view widget displaying actual settings entries. */
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Frontend Options|Widgets")
	UCommonFrontendListView* CommonListView_OptionsList;

	/** Details view for showing information or editable fields about the selected entry. */
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Frontend Options|Widgets")
	UCommonFrontendDetails* DetailsView_ListEntryInfo;

	/** Internal data registry created and owned by this widget. */
	UPROPERTY(Transient, EditAnywhere, Category = "Frontend Options|Internal")
	UCommonFrontendDataRegistry* CreatedOwningDataRegistry;

	/** Input action used for resetting settings. Bound to keyboard/controller input. */
	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options|Input", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;

	/** Handle for the registered reset input action binding. */
	FUIActionBindingHandle ResetActionHandle;

	/** Collection of data objects that support resetting to default. */
	UPROPERTY(Transient)
	TArray<UListDataObjectBase*> ResettableDataArray;

	/** Flag indicating whether a reset operation is currently in progress to prevent recursion. */
	bool bIsResettingData = false;

	/** Input action used for manually saving settings. */
	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options|Input", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle SaveAction;

	/** Handle for the registered save input action binding. */
	FUIActionBindingHandle SaveActionHandle;

	/** Handles user-triggered save action. */
	void OnSaveBoundActionTriggered();

	/** Set of data items that have been modified but not yet saved. */
	UPROPERTY(Transient)
	TSet<TWeakObjectPtr<UListDataObjectBase>> UnsavedDataItems;

	/** Tracks the ID of the last active tab to allow reverting navigation if unsaved changes exist. */
	FName LastActiveTabID;

	/** Helper to revert unsaved changes (reload from config). */
	void DiscardUnsavedChanges();

	/** Callback when the settings registry asset has finished loading asynchronously. */
	void OnSettingsRegistryLoaded();

	/** Handle for the async load request to keep it alive. */
	TSharedPtr<FStreamableHandle> SettingsRegistryHandle;
};
