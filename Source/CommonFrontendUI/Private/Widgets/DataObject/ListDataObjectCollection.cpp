#include "Widgets/DataObject/ListDataObjectCollection.h"

void UListDataObjectCollection::AddChildListData(UListDataObjectBase* InChildListData)
{
	InChildListData->InitDataObject();

	InChildListData->SetParentData(this);

	ChildListDataArray.Add(InChildListData);
}

TArray<UListDataObjectBase*> UListDataObjectCollection::GetAllChildListData() const
{
	return ChildListDataArray;
}

bool UListDataObjectCollection::HasAnyChildListData() const
{
	return !ChildListDataArray.IsEmpty();
}
