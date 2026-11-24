#include "Widgets/CommonFrontendOptionsWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/LocalPlayer.h"

#include "Widgets/Components/CommonFrontendButtonBase.h"
#include "Widgets/CommonFrontendDataRegistry.h"
#include "Widgets/CommonFrontendDetails.h"
#include "Utility/CommonFrontendLibrary.h"
#include "Utility/CommonFrontendGameUserSettings.h"
#include "Widgets/Components/CommonFrontendListView.h"
#include "Widgets/Components/CommonFrontendTabListWidgetBase.h"
#include "Subsystems/CommonFrontendUI_Subsystem.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "Widgets/Dataobject/ListDataObjectCollection.h"
#include "Widgets/ListEntries/ListEntryWidgetBase.h"
#include "Widgets/DataObject/ListDataObjectValue.h"
#include "Data/CommonFrontendSettingsRegistry.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void UCommonFrontendOptionsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Register custom action for resetting settings to default values
	if (!ResetAction.IsNull())
	{
		ResetActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				ResetAction,
				true,
				FSimpleDelegate::CreateUObject(this, &ThisClass::OnResetBoundActionTriggered)
			)
		);
	}

	// Register custom action for saving settings
	if (!SaveAction.IsNull())
	{
		SaveActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				SaveAction,
				true,
				FSimpleDelegate::CreateUObject(this, &ThisClass::OnSaveBoundActionTriggered)
			)
		);
	}
	
	// Register standard "Back" action to deactivate the widget
	RegisterUIActionBinding(
		FBindUIActionArgs(
			ICommonInputModule::GetSettings().GetDefaultBackAction(),
			true,
			FSimpleDelegate::CreateUObject(this,&ThisClass::OnBackBoundActionTriggered)
		)
	);

	TabListWidget_OptionsTabs->OnTabSelected.AddUniqueDynamic(this,&ThisClass::OnOptionsTabSelected);
	
	CommonListView_OptionsList->OnItemIsHoveredChanged().AddUObject(this,&ThisClass::OnListViewItemHovered);
	CommonListView_OptionsList->OnItemSelectionChanged().AddUObject(this,&ThisClass::OnListViewItemSelected);

	// Default to unbound on init until we have data
	if (ResettableDataArray.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandle);
	}
	if (UnsavedDataItems.IsEmpty())
	{
		RemoveActionBinding(SaveActionHandle);
	}

	if (SettingsRegistry.IsPending())
	{
		SettingsRegistryHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			SettingsRegistry.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &ThisClass::OnSettingsRegistryLoaded)
		);
	}
	else
	{
		OnSettingsRegistryLoaded();
	}

	const bool bIsSaveBound = GetActionBindings().Contains(SaveActionHandle);
	const bool bIsResetBound = GetActionBindings().Contains(ResetActionHandle);
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	UE_LOG(LogTemp, Warning, TEXT("[CommonFrontend] NativeOnInitialized End | Save Bound: %d | Reset Bound: %d"), bIsSaveBound, bIsResetBound);
}

void UCommonFrontendOptionsWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	UCommonFrontendDataRegistry* Registry = GetOrCreateDataRegistry();
	if (!Registry)
	{
		// Registry might not be loaded yet, OnSettingsRegistryLoaded will handle population when ready.
		return;
	}

	for (UListDataObjectCollection* TabCollection : Registry->GetRegisteredOptionsTabCollections())
	{
		if (!TabCollection)
		{
			continue;
		}

		const FName TabID = TabCollection->GetDataID();

		if (TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabID) != nullptr)
		{
			continue;
		}

		TabListWidget_OptionsTabs->RequestRegisterTab(TabID,TabCollection->GetDataDisplayName());
	}

	// Clear any stale unsaved state from previous session
	for (TWeakObjectPtr<UListDataObjectBase>& WeakData : UnsavedDataItems)
	{
		if (UListDataObjectValue* DataValue = Cast<UListDataObjectValue>(WeakData.Get()))
		{
			DataValue->ClearBufferedChanges();
		}
	}
	UnsavedDataItems.Empty();
	RemoveActionBinding(SaveActionHandle);
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	UE_LOG(LogTemp, Warning, TEXT("[CommonFrontend] NativeOnActivated: Widget Activated."));
	
}

void UCommonFrontendOptionsWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	UCommonFrontendLibrary::GetCommonFrontendGameUserSettings()->ApplySettings(true);
}

UWidget* UCommonFrontendOptionsWidget::NativeGetDesiredFocusTarget() const
{
	if (UObject* SelectedObject = CommonListView_OptionsList->GetSelectedItem())
	{
		if (UUserWidget* SelectedEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem(SelectedObject))
		{
			return SelectedEntryWidget;
		}
	}

	return Super::NativeGetDesiredFocusTarget();
}

UCommonFrontendDataRegistry* UCommonFrontendOptionsWidget::GetOrCreateDataRegistry()
{
	if (!CreatedOwningDataRegistry && !SettingsRegistry.IsPending())
	{
		if (const UCommonFrontendSettingsRegistry* LoadedRegistry = SettingsRegistry.Get())
		{
			CreatedOwningDataRegistry = NewObject<UCommonFrontendDataRegistry>(this);
			CreatedOwningDataRegistry->InitFromSettingsRegistry(LoadedRegistry, GetOwningLocalPlayer());
		}
	}

	if (!CreatedOwningDataRegistry)
	{
		return nullptr;
	}

	return CreatedOwningDataRegistry;
}

void UCommonFrontendOptionsWidget::OnSettingsRegistryLoaded()
{
	// Ensure registry is created if it wasn't already
	GetOrCreateDataRegistry();

	// If we are currently active, we need to populate the tabs now
	if (IsActivated() && CreatedOwningDataRegistry)
	{
		// Re-run the activation logic to populate tabs
		NativeOnActivated();
	}
}

void UCommonFrontendOptionsWidget::OnResetBoundActionTriggered()
{
	// Prompt the user for confirmation before resetting all settings in the current tab
	if (ResettableDataArray.IsEmpty())
	{
		return;
	}

	UCommonButtonBase* SelectedTabButton = TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabListWidget_OptionsTabs->GetActiveTab());

	UCommonFrontendButtonBase* ButtonBase = Cast<UCommonFrontendButtonBase>(SelectedTabButton);
	if (!ButtonBase)
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Error, TEXT("SelectedTabButton is not UCommonFrontendButtonBase"));
		return;
	}
	const FString SelectedTabButtonName = ButtonBase->GetButtonDisplayText().ToString();

	TArray<FString> Buttons;
	Buttons.Add(TEXT("Yes"));
	Buttons.Add(TEXT("No"));

	UCommonFrontendUI_Subsystem::Get(this)->PushChoiceScreenToModalStackAsync(
		FText::FromString(TEXT("Reset")),
		FText::FromString(TEXT("Are you sure you want to reset all the settings under the ") + SelectedTabButtonName + TEXT(" tab?")),
		Buttons,
		[this](const FString& SelectedID)
		{
			if (!SelectedID.Equals(TEXT("Yes"), ESearchCase::IgnoreCase))
			{
				return;
			}

			bIsResettingData = true;
			bool bHasDataFailedToReset = false;

			// Revert all modified data in this tab collection to their default states
			for (UListDataObjectBase* DataToReset : ResettableDataArray)
			{
				if (DataToReset && !DataToReset->TryResetBackToDefaultValue())
				{
					bHasDataFailedToReset = true;
				}
			}

			if (!bHasDataFailedToReset)
			{
				if (UCommonFrontendLibrary::ShouldEnableDebug())
				UE_LOG(LogTemp, Warning, TEXT("[CommonFrontend] Reset complete. Clearing ResettableDataArray."));
				ResettableDataArray.Empty();
				RemoveActionBinding(ResetActionHandle);
			}

			bIsResettingData = false;
			OnSaveBoundActionTriggered();

		}
	);
}

void UCommonFrontendOptionsWidget::OnBackBoundActionTriggered()
{
	if (UnsavedDataItems.Num() > 0)
	{
		// Show choice screen for unsaved changes on exit
		TArray<FString> Buttons;
		Buttons.Add(TEXT("Save & Exit"));
		Buttons.Add(TEXT("Discard & Exit"));
		Buttons.Add(TEXT("Cancel"));

		UCommonFrontendUI_Subsystem::Get(this)->PushChoiceScreenToModalStackAsync(
			FText::FromString(TEXT("Unsaved Changes")),
			FText::FromString(TEXT("You have unsaved changes. What would you like to do?")),
			Buttons,
			[this](const FString& SelectedID)
			{
				if (SelectedID.Equals(TEXT("Save & Exit"), ESearchCase::IgnoreCase))
				{
					OnSaveBoundActionTriggered();
					DeactivateWidget();
				}
				else if (SelectedID.Equals(TEXT("Discard & Exit"), ESearchCase::IgnoreCase))
				{
					DiscardUnsavedChanges();
					DeactivateWidget();
				}
			}
		);
	}
	else
	{
		DeactivateWidget();
	}
}

void UCommonFrontendOptionsWidget::OnOptionsTabSelected(FName TabId)
{
	if (UnsavedDataItems.Num() > 0 && !LastActiveTabID.IsNone() && TabId != LastActiveTabID)
	{
		// Guard: Prevent switching if we have unsaved changes
		
		// Immediately revert visual selection so user stays on current tab while deciding
		if (TabListWidget_OptionsTabs->GetActiveTab() != LastActiveTabID)
		{
			TabListWidget_OptionsTabs->SelectTabByID(LastActiveTabID);
			// This will trigger OnOptionsTabSelected again with LastActiveTabID.
			// The recursive call will hit "TabId == LastActiveTabID" and proceed to refresh list (safe).
			// We return here to stop processing the *new* tab logic in this stack frame.
			
			// Show the modal
			TArray<FString> Buttons;
			Buttons.Add(TEXT("Save & Continue"));
			Buttons.Add(TEXT("Discard & Continue"));
			Buttons.Add(TEXT("Cancel"));

			UCommonFrontendUI_Subsystem::Get(this)->PushChoiceScreenToModalStackAsync(
				FText::FromString(TEXT("Unsaved Changes")),
				FText::FromString(TEXT("You have unsaved changes in the current tab. What would you like to do?")),
				Buttons,
				[this, TabId](const FString& SelectedID)
				{
					if (SelectedID.Equals(TEXT("Save & Continue"), ESearchCase::IgnoreCase))
					{
						OnSaveBoundActionTriggered();
						// Now force switch to the originally requested tab
						TabListWidget_OptionsTabs->SelectTabByID(TabId); 
					}
					else if (SelectedID.Equals(TEXT("Discard & Continue"), ESearchCase::IgnoreCase))
					{
						DiscardUnsavedChanges();
						TabListWidget_OptionsTabs->SelectTabByID(TabId);
					}
					// On Cancel, do nothing (we already reverted to LastActiveTabID)
				}
			);

			return; 
		}
	}

	LastActiveTabID = TabId;

	// Refresh the list view items based on the newly selected tab category
	DetailsView_ListEntryInfo->ClearDetailsViewInfo();

	UCommonFrontendDataRegistry* Registry = GetOrCreateDataRegistry();
	if (!Registry)
	{
		return;
	}

	TArray<UListDataObjectBase*> FoundListSourceItems = Registry->GetListSourceItemsBySelectedTabID(TabId);

	CommonListView_OptionsList->SetListItems(FoundListSourceItems);
	CommonListView_OptionsList->RequestRefresh();

	if (CommonListView_OptionsList->GetNumItems() != 0)
	{
		CommonListView_OptionsList->NavigateToIndex(0);
		CommonListView_OptionsList->SetSelectedIndex(0);
	}

	ResettableDataArray.Empty();
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	UE_LOG(LogTemp, Warning, TEXT("[CommonFrontend] OnOptionsTabSelected: TabId=%s, FoundItems=%d"), *TabId.ToString(), FoundListSourceItems.Num());

	for (UListDataObjectBase* FoundListSourceItem : FoundListSourceItems)
	{
		if (!FoundListSourceItem)
		{
			continue;
		}

		// Log details for every item
		FString CurrentVal = TEXT("N/A");
		FString DefaultVal = TEXT("N/A");
		bool bIsDirty = false;
		bool bCanReset = FoundListSourceItem->CanResetBackToDefaultValue();

		if (UListDataObjectValue* ValueData = Cast<UListDataObjectValue>(FoundListSourceItem))
		{
			CurrentVal = ValueData->GetCurrentValueAsString();
			if (ValueData->HasDefaultValue())
			{
				DefaultVal = ValueData->GetDefaultValueAsString();
			}
			bIsDirty = ValueData->IsDirty();
		}

		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Warning, TEXT("  - Item: %s | Cur: %s | Def: %s | Dirty: %d | Reset: %d"), 
			*FoundListSourceItem->GetDataID().ToString(), *CurrentVal, *DefaultVal, bIsDirty, bCanReset);

		if (!FoundListSourceItem->OnListDataModified.IsBoundToObject(this))
		{
			FoundListSourceItem->OnListDataModified.AddUObject(this,&ThisClass::OnListViewListDataModified);
		}

		if (bCanReset)
		{
			ResettableDataArray.AddUnique(FoundListSourceItem);
		}
	}

	if (ResettableDataArray.IsEmpty())
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Warning, TEXT("[CommonFrontend] No resettable data found. Removing ResetAction."));
		RemoveActionBinding(ResetActionHandle);
	}
	else
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Warning, TEXT("[CommonFrontend] Resettable data found (%d items). Adding/Keeping ResetAction."), ResettableDataArray.Num());
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);
		}
	}

	const bool bIsSaveBound = GetActionBindings().Contains(SaveActionHandle);
	const bool bIsResetBound = GetActionBindings().Contains(ResetActionHandle);
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	UE_LOG(LogTemp, Warning, TEXT("[CommonFrontend] OnOptionsTabSelected End | Save Bound: %d | Reset Bound: %d"), bIsSaveBound, bIsResetBound);
}

void UCommonFrontendOptionsWidget::OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
	if (!InHoveredItem)
	{
		return;
	}

	UListEntryWidgetBase* HoveredEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem<UListEntryWidgetBase>(InHoveredItem);

	if (!HoveredEntryWidget)
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Error, TEXT("HoveredEntryWidget invalid in OnListViewItemHovered"));
		return;
	}

	HoveredEntryWidget->NativeOnListEntryWidgetHovered(bWasHovered);

	if (bWasHovered)
	{
		UListDataObjectBase* ListDataObject = Cast<UListDataObjectBase>(InHoveredItem);
		if (!ListDataObject)
		{
			if (UCommonFrontendLibrary::ShouldEnableDebug())
			UE_LOG(LogTemp, Error, TEXT("InHoveredItem is not UListDataObjectBase"));
			return;
		}

		DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
			ListDataObject,
			TryGetEntryWidgetClassName(InHoveredItem)
		);
	}
	else
	{
		if (UListDataObjectBase* SelectedItem = CommonListView_OptionsList->GetSelectedItem<UListDataObjectBase>())
		{
			DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
				SelectedItem,
				TryGetEntryWidgetClassName(SelectedItem)
			);
		}
	}
}

void UCommonFrontendOptionsWidget::HandleNestedItemSelection(UObject* InSelectedItem)
{
	OnListViewItemSelected(InSelectedItem);
}

void UCommonFrontendOptionsWidget::OnListViewItemSelected(UObject* InSelectedItem)
{
	if (!InSelectedItem)
	{
		return;
	}

	UListDataObjectBase* ListDataObject = Cast<UListDataObjectBase>(InSelectedItem);
	if (!ListDataObject)
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Error, TEXT("InSelectedItem is not UListDataObjectBase"));
		return;
	}

	DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
			ListDataObject,
			TryGetEntryWidgetClassName(InSelectedItem)
		);
}

FString UCommonFrontendOptionsWidget::TryGetEntryWidgetClassName(UObject* InOwningListItem) const
{
	if (UUserWidget* FoundEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem(InOwningListItem))
	{
		return FoundEntryWidget->GetClass()->GetName();
	}

	return TEXT("Entry Widget Not Valid");
}

void UCommonFrontendOptionsWidget::OnListViewListDataModified(UListDataObjectBase* ModifiedData,
                                                              EOptionsListDataModifyReason ModifyReason)
{
	if (!ModifiedData)
	{
		return;
	}
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	UE_LOG(LogTemp, Warning, TEXT("[CommonFrontend] OnListViewListDataModified: Item=%s, Reason=%d, bIsResettingData=%d"), 
		*ModifiedData->GetDataID().ToString(), (int32)ModifyReason, bIsResettingData);

	// Track Unsaved Changes - ALLOW this to run even during reset to ensure Save button clears
	if (ModifiedData->ShouldApplySettingsImmediately())
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Warning, TEXT("  - Immediate Apply. Removing from UnsavedDataItems if present."));
		if (UnsavedDataItems.Contains(ModifiedData))
		{
			UnsavedDataItems.Remove(ModifiedData);
		}
	}
	else
	{
		UListDataObjectValue* ValueData = Cast<UListDataObjectValue>(ModifiedData);

		// Check if the value is actually different from the committed value
		const bool bIsDirty = ValueData && (ValueData->IsDirty());
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Warning, TEXT("  - ValueData=%p, bIsDirty=%d"), ValueData, bIsDirty);

		if (bIsDirty)
		{
			if (!UnsavedDataItems.Contains(ModifiedData))
			{
				if (UCommonFrontendLibrary::ShouldEnableDebug())
				UE_LOG(LogTemp, Warning, TEXT("  - Adding to UnsavedDataItems. Total: %d"), UnsavedDataItems.Num() + 1);
				UnsavedDataItems.Add(ModifiedData);

				// Ensure Save Binding is active
				if (!GetActionBindings().Contains(SaveActionHandle))
				{
					AddActionBinding(SaveActionHandle);
				}
			}
		}
		else
		{
			// If not dirty (reverted to original), remove from unsaved items
			if (UnsavedDataItems.Contains(ModifiedData))
			{
				if (UCommonFrontendLibrary::ShouldEnableDebug())
				UE_LOG(LogTemp, Warning, TEXT("  - Removing from UnsavedDataItems. Total: %d"), UnsavedDataItems.Num() - 1);
				UnsavedDataItems.Remove(ModifiedData);
			}

			// If no more unsaved items, hide the save button
			if (UnsavedDataItems.IsEmpty())
			{
				if (UCommonFrontendLibrary::ShouldEnableDebug())
				UE_LOG(LogTemp, Warning, TEXT("  - No unsaved items. Removing SaveAction."));
				RemoveActionBinding(SaveActionHandle);
			}
		}
	}

	const bool bCanReset = ModifiedData->CanResetBackToDefaultValue();
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	UE_LOG(LogTemp, Warning, TEXT("  - CanResetBackToDefaultValue=%d"), bCanReset);

	if (!bIsResettingData)
	{
		if (bCanReset)
		{
			ResettableDataArray.AddUnique(ModifiedData);

			if (!GetActionBindings().Contains(ResetActionHandle))
			{
				AddActionBinding(ResetActionHandle);
			}
		}
		else
		{
			if (ResettableDataArray.Contains(ModifiedData))
			{
				ResettableDataArray.Remove(ModifiedData);
			}
		}
	}

	if (ModifiedData->ShouldApplySettingsImmediately())
	{
		if (UCommonFrontendGameUserSettings* GUS = UCommonFrontendLibrary::GetCommonFrontendGameUserSettings())
		{
			GUS->ApplySettings(true);
		}
	}

	if (ResettableDataArray.IsEmpty())
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Warning, TEXT("  - ResettableDataArray empty. Removing ResetAction."));
		RemoveActionBinding(ResetActionHandle);
	}
	else
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Warning, TEXT("  - ResettableDataArray has %d items:"), ResettableDataArray.Num());
		for (UListDataObjectBase* Item : ResettableDataArray)
		{
			if (Item && UCommonFrontendLibrary::ShouldEnableDebug())
				UE_LOG(LogTemp, Warning, TEXT("    * %s"), *Item->GetDataID().ToString());
		}
	}

	if (UnsavedDataItems.IsEmpty())
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Warning, TEXT("  - UnsavedDataItems empty. Removing SaveAction."));
		RemoveActionBinding(SaveActionHandle);
	}
	else
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Warning, TEXT("  - UnsavedDataItems has %d items:"), UnsavedDataItems.Num());
		for (TWeakObjectPtr<UListDataObjectBase> WeakItem : UnsavedDataItems)
		{
			if (UListDataObjectBase* Item = WeakItem.Get())
				if (UCommonFrontendLibrary::ShouldEnableDebug())
					UE_LOG(LogTemp, Warning, TEXT("    * %s"), *Item->GetDataID().ToString());
		}
	}

	const bool bIsSaveBound = GetActionBindings().Contains(SaveActionHandle);
	const bool bIsResetBound = GetActionBindings().Contains(ResetActionHandle);
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	UE_LOG(LogTemp, Warning, TEXT("[CommonFrontend] OnListViewListDataModified End | Save Bound: %d | Reset Bound: %d"), bIsSaveBound, bIsResetBound);
}

void UCommonFrontendOptionsWidget::OnSaveBoundActionTriggered()
{
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	{
		UE_LOG(LogTemp, Warning, TEXT("[CommonFrontend] OnSaveBoundActionTriggered: Triggered (Unsaved Items: %d)"), UnsavedDataItems.Num());
	}

	// Commit all buffered changes first
	TArray<TWeakObjectPtr<UListDataObjectBase>> ItemsToRevert = UnsavedDataItems.Array();

	for (TWeakObjectPtr<UListDataObjectBase>& WeakData : ItemsToRevert)
	{
		if (UListDataObjectValue* DataValue = Cast<UListDataObjectValue>(WeakData.Get()))
		{
			if (UCommonFrontendLibrary::ShouldEnableDebug())
			{
				UE_LOG(LogTemp, Warning, TEXT("  - Committing Item: %s"), *DataValue->GetDataID().ToString());
			}
			DataValue->CommitBufferedChanges();
		}
	}

	if (UCommonFrontendGameUserSettings* GUS = UCommonFrontendLibrary::GetCommonFrontendGameUserSettings())
	{
		UE_LOG(LogTemp, Warning, TEXT("  - Applying Settings to GUS."));
		GUS->ApplySettings(true);
	}

	UnsavedDataItems.Empty();
	RemoveActionBinding(SaveActionHandle);

	// Force UI refresh to reflect changes (e.g. Overall Quality preset update)
	CommonListView_OptionsList->RequestRefresh();
}

void UCommonFrontendOptionsWidget::DiscardUnsavedChanges()
{
	// Revert all unsaved items to their committed values
	TArray<TWeakObjectPtr<UListDataObjectBase>> ItemsToRevert = UnsavedDataItems.Array();

	for (TWeakObjectPtr<UListDataObjectBase>& WeakData : ItemsToRevert)
	{
		if (UListDataObjectBase* DataObj = WeakData.Get())
		{
			if (UCommonFrontendLibrary::ShouldEnableDebug())
			{
				UE_LOG(LogTemp, Warning, TEXT("[CommonFrontend] Discarding changes for: %s"), *DataObj->GetDataID().ToString());
			}
			DataObj->CleanUp();
		}
	}

	UnsavedDataItems.Empty();
	RemoveActionBinding(SaveActionHandle);
}
