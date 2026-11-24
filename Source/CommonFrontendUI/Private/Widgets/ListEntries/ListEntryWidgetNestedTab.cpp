#include "Widgets/ListEntries/ListEntryWidgetNestedTab.h"

#include "CommonFrontendOptionsWidget.h"
#include "Widgets/DataObject/ListDataObjectNestedTabCollection.h"
#include "Widgets/DataObject/ListDataObjectNestedTab.h"
#include "Widgets/Components/CommonFrontendTabListWidgetBase.h"
#include "Widgets/Components/CommonFrontendListView.h"

void UListEntryWidgetNestedTab::NativeConstruct()
{
	Super::NativeConstruct();

	if (NestedTabList)
	{
		NestedTabList->OnTabSelected.AddUniqueDynamic(this, &ThisClass::OnTabSelected);
	}

	if (NestedEntryListView)
	{
		NestedEntryListView->OnItemSelectionChanged().AddUObject(this, &ThisClass::OnNestedItemSelectionChanged);
	}
}

UWidget* UListEntryWidgetNestedTab::NativeGetWidgetToFocusForGamepad() const
{
	if (UWidget* BP_ValidWidget = BP_GetWidgetToFocusForGamepad())
		return BP_ValidWidget;

	if (CachedCollectionObject && NestedTabList)
	{
		for (const UListDataObjectNestedTab* Tab : CachedCollectionObject->GetNestedTabs())
		{
			if (Tab)
			{
				if (UWidget* TabButton = NestedTabList->GetTabButtonBaseByID(Tab->GetNestedTabId()))
				{
					return TabButton;
				}
			}
		}
	}

	return const_cast<UWidget*>(Cast<UWidget>(this));
}

void UListEntryWidgetNestedTab::OnNestedItemSelectionChanged(UObject* Item)
{
}

void UListEntryWidgetNestedTab::OnOwningListDataObjectSet(UListDataObjectBase* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	CachedCollectionObject = Cast<UListDataObjectNestedTabCollection>(InOwningListDataObject);

	if (CachedCollectionObject)
	{
		RegisterNestedTabs();

		const TArray<UListDataObjectNestedTab*>& Tabs = CachedCollectionObject->GetNestedTabs();
		if (Tabs.Num() > 0 && Tabs[0])
		{
			const FName DefaultTabID = Tabs[0]->GetNestedTabId();
			
			// Ensure the default tab's child list items are populated directly
			OnTabSelected(DefaultTabID);

			if (NestedTabList)
			{
				NestedTabList->SelectTabByID(DefaultTabID);
			}
		}
	}
}

void UListEntryWidgetNestedTab::RegisterNestedTabs()
{
	if (!NestedTabList || !CachedCollectionObject) return;

	for (UListDataObjectNestedTab* Tab : CachedCollectionObject->GetNestedTabs())
	{
		if (!Tab) continue;
		
		if (NestedTabList->GetTabButtonBaseByID(Tab->GetNestedTabId())) continue;

		NestedTabList->RequestRegisterTab(Tab->GetNestedTabId(), Tab->GetDataDisplayName());
	}
}

void UListEntryWidgetNestedTab::OnTabSelected(FName TabID)
{
	if (!CachedCollectionObject || !NestedEntryListView) return;

	for (UListDataObjectNestedTab* Tab : CachedCollectionObject->GetNestedTabs())
	{
		if (Tab && Tab->GetNestedTabId() == TabID)
		{
			TArray<UListDataObjectBase*> TabChildItems = Tab->GetAllChildListData();
			NestedEntryListView->SetListItems(TabChildItems);
			NestedEntryListView->RequestRefresh();

			if (TabChildItems.Num() > 0)
			{
				NestedEntryListView->SetSelectedIndex(0);
			}
			return;
		}
	}
}
