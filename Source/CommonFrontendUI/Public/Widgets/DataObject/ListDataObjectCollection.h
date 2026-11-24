#pragma once

#include "CoreMinimal.h"
#include "Widgets/DataObject/ListDataObjectBase.h"
#include "ListDataObjectCollection.generated.h"

/**
 * UListDataObjectCollection
 *
 * Represents a container object that can hold and manage a collection of child list data objects.
 * Used within the frontend UI system to represent hierarchical option structures (e.g., categories).
 */
UCLASS()
class COMMONFRONTENDUI_API UListDataObjectCollection : public UListDataObjectBase
{
	GENERATED_BODY()

public:
	/** Adds a new child data object to this collection. */
	void AddChildListData(UListDataObjectBase* InChildListData);

	/** Returns all child list data objects contained in this collection. */
	virtual TArray<UListDataObjectBase*> GetAllChildListData() const override;

	/** Returns true if this collection contains any child data objects. */
	virtual bool HasAnyChildListData() const override;

private:
	/** Array of child list data objects stored in this collection. */
	UPROPERTY(Transient)
	TArray<UListDataObjectBase*> ChildListDataArray;
};
