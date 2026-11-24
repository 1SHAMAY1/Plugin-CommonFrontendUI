#pragma once

#include "CoreMinimal.h"
#include "Widgets/ListEntries/ListEntryWidgetBase.h"
#include "ListEntryWidgetNestedTab.generated.h"

class UListDataObjectNestedTabCollection;
class UCommonFrontendTabListWidgetBase;
class UCommonFrontendListView;

/**
 * UListEntryWidgetNestedTab
 *
 * A UI list entry widget representing a nested tab/category header in the settings menu.
 * This widget displays a tab header that can be expanded/collapsed to show child settings.
 *
 * It binds to a UListDataObjectNestedTab instance to reflect the tab name and its children.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class COMMONFRONTENDUI_API UListEntryWidgetNestedTab : public UListEntryWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnTabSelected(FName TabID);

	virtual void OnNestedItemSelectionChanged(UObject* Item);

protected:
	/** Called when the owning data object is set for this entry. */
	virtual void OnOwningListDataObjectSet(UListDataObjectBase* InOwningListDataObject) override;

	virtual void NativeConstruct() override;

	virtual UWidget* NativeGetWidgetToFocusForGamepad() const override;
	
private:
	/** Registers the nested tabs into the CommonUI Tab List. */
	void RegisterNestedTabs();

	/** The Horizontal Tab Switcher. */
	UPROPERTY(BlueprintReadOnly, Category = "Nested Tab", meta = (AllowPrivateAccess = "true", BindWidget))
	UCommonFrontendTabListWidgetBase* NestedTabList;

	/** The List View to display the content of the selected tab. */
	UPROPERTY(BlueprintReadOnly, Category = "Nested Tab", meta = (AllowPrivateAccess = "true", BindWidget))
	UCommonFrontendListView* NestedEntryListView;

	/** Cached reference to the data object. */
	UPROPERTY(Transient)
	UListDataObjectNestedTabCollection* CachedCollectionObject;
};
