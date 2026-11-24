#pragma once

#include "CoreMinimal.h"
#include "Widgets/DataObject/ListDataObjectCollection.h"
#include "ListDataObjectNestedTab.generated.h"

/**
 * UListDataObjectNestedTab
 *
 * Represents a nested tab within the settings UI. Contains child settings
 * and provides metadata for tab display.
 */
UCLASS()
class COMMONFRONTENDUI_API UListDataObjectNestedTab : public UListDataObjectCollection
{
	GENERATED_BODY()

public:
	/** Sets the unique identifier for this nested tab. */
	void SetNestedTabId(FName InTabId) { NestedTabId = InTabId; }

	/** Returns the unique identifier for this nested tab. */
	FName GetNestedTabId() const { return NestedTabId; }

	/** Sets the collapsed state of the tab (true for collapsed, false for expanded). */
	void SetIsCollapsed(bool bInCollapsed) { bIsCollapsed = bInCollapsed; }

	/** Returns whether this tab is currently in a collapsed state. */
	bool IsCollapsed() const { return bIsCollapsed; }

private:
	/** Unique identifier for this nested tab. */
	FName NestedTabId;

	/** Whether this tab is collapsed (logic inversion of Expanded). Default to false (Expanded). */
	bool bIsCollapsed = false;
};
