#pragma once

#include "CoreMinimal.h"
#include "Widgets/DataObject/ListDataObjectBase.h"
#include "Widgets/DataObject/ListDataObjectNestedTab.h"
#include "ListDataObjectNestedTabCollection.generated.h"

/**
 * UListDataObjectNestedTabCollection
 * 
 * Represents a collection of Nested Tabs.
 * The UI Widget for this object displays a horizontal tab switcher.
 */
UCLASS()
class COMMONFRONTENDUI_API UListDataObjectNestedTabCollection : public UListDataObjectBase
{
	GENERATED_BODY()

public:
	/** Returns the read-only list of nested tabs contained within this collection. */
	const TArray<UListDataObjectNestedTab*>& GetNestedTabs() const { return NestedTabs; }

	/** Adds a new nested tab object to this collection. */
	void AddNestedTab(UListDataObjectNestedTab* InTab) { NestedTabs.Add(InTab); }

    virtual TArray<UListDataObjectBase*> GetAllChildListData() const override
    {
        TArray<UListDataObjectBase*> Children;
        for (UListDataObjectNestedTab* Tab : NestedTabs)
        {
            Children.Add(Tab);
        }
        return Children;
    }

    virtual bool HasAnyChildListData() const override
    {
        return NestedTabs.Num() > 0;
    }

private:
	/** The list of nested tabs contained in this collection. */
	UPROPERTY(Transient)
	TArray<UListDataObjectNestedTab*> NestedTabs;
};
