#include "Widgets/ListEntries/ListEntryWidgetBase.h"

#include "CommonInputSubsystem.h"
#include "Components/ListView.h"
#include "Widgets/DataObject/ListDataObjectBase.h"

void UListEntryWidgetBase::NativeOnListEntryWidgetHovered(bool bWasHovered)
{
	const bool bIsEntryWidgetStillSelected = IsListItemSelected();
	if (!bWasHovered && bIsEntryWidgetStillSelected)
	{
		BP_OnListEntryWidgetHovered(bWasHovered, bIsEntryWidgetStillSelected);
		return;
	}
	if (bWasHovered)
		SetStyle(ButtonStyleEntryHovered);
	else if (!bIsEntryWidgetStillSelected)
		SetStyle(Style);
	NativeOnCurrentTextStyleChanged();
	BP_OnListEntryWidgetHovered(bWasHovered, bIsEntryWidgetStillSelected);
}

UWidget* UListEntryWidgetBase::NativeGetWidgetToFocusForGamepad() const
{
	return BP_GetWidgetToFocusForGamepad();
}

void UListEntryWidgetBase::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (UListDataObjectBase* DataObject = Cast<UListDataObjectBase>(ListItemObject))
	{
		OnOwningListDataObjectSet(DataObject);
	}
}

void UListEntryWidgetBase::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	UObject* CurrentItem = GetListItem();
	if (!CurrentItem)
	{
		return;
	}

	NativeOnListEntryWidgetHovered(bIsSelected);
	TSubclassOf<UCommonButtonStyle> ButtonStyle = bIsSelected ? ButtonStyleEntryHovered : Style;
	SetStyle(ButtonStyle);
	NativeOnCurrentTextStyleChanged();
	BP_OnToggleEntryWidgetHighlightState(bIsSelected);
}


void UListEntryWidgetBase::NativeOnEntryReleased()
{
	IUserObjectListEntry::NativeOnEntryReleased();

	if (CachedOwningDataObject)
	{
		CachedOwningDataObject->OnListDataModified.RemoveAll(this);
		CachedOwningDataObject->OnDependencyDataModified.RemoveAll(this);
		CachedOwningDataObject = nullptr;
	}

	NativeOnListEntryWidgetHovered(false);
}


FReply UListEntryWidgetBase::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();

	// Redirect focus to the appropriate sub-widget if using gamepad navigation
	if (CommonInputSubsystem && CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		if (UWidget* WidgetToFocus = NativeGetWidgetToFocusForGamepad())
		{
			if (TSharedPtr<SWidget> SlateWidgetToFocus = WidgetToFocus->GetCachedWidget())
			{
				return FReply::Handled().SetUserFocus(SlateWidgetToFocus.ToSharedRef());
			}
		}
	}

	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UListEntryWidgetBase::OnOwningListDataObjectSet(UListDataObjectBase* InOwningListDataObject)
{
	if (!InOwningListDataObject)
	{
		return;
	}
 
	// Bind the widget to its corresponding data object and initialize display state
	SetButtonText(InOwningListDataObject->GetDataDisplayName());

	if (!InOwningListDataObject->OnListDataModified.IsBoundToObject(this))
	{
		InOwningListDataObject->OnListDataModified.AddUObject(this, &ThisClass::OnOwningListDataObjectModified);
	}

	if (!InOwningListDataObject->OnDependencyDataModified.IsBoundToObject(this))
	{
		InOwningListDataObject->OnDependencyDataModified.AddUObject(this, &ThisClass::OnOwningDependencyDataObjectModified);
	}

	OnToggleEditableState(InOwningListDataObject->IsDataCurrentlyEditable());

	CachedOwningDataObject = InOwningListDataObject;
}

void UListEntryWidgetBase::OnOwningListDataObjectModified(UListDataObjectBase* OwningModifiedData, EOptionsListDataModifyReason ModifyReason)
{
	// Base implementation does nothing
}

void UListEntryWidgetBase::OnOwningDependencyDataObjectModified(UListDataObjectBase* OwningModifiedDependencyData, EOptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningDataObject)
	{
		OnToggleEditableState(CachedOwningDataObject->IsDataCurrentlyEditable());
	}
}

void UListEntryWidgetBase::SelectThisEntryWidget()
{
	if (UListView* OwningListView = Cast<UListView>(GetOwningListView()))
	{
		OwningListView->SetSelectedItem(GetListItem());
	}
}

void UListEntryWidgetBase::NativeOnHovered()
{
	if (UObject* CurrentItem = GetListItem())
	{
		if (UListView* OwningListView = Cast<UListView>(GetOwningListView()))
		{
			if (OwningListView->GetListItems().Contains(CurrentItem))
			{
				OwningListView->SetSelectedItem(CurrentItem);
			}
		}
	}
	Super::NativeOnHovered();
}
