#include "Data/DataListEntryMappingConfig.h"

#include "Widgets/DataObject/ListDataObjectBase.h"

TSubclassOf<UListEntryWidgetBase> UDataListEntryMappingConfig::FindEntryWidgetClassByDataObject(
	UListDataObjectBase* InDataObject) const
{
	if (!InDataObject)
		return nullptr;
	
	const TSubclassOf<UListDataObjectBase> DataClass = InDataObject->GetClass();
	if (const TSubclassOf<UListEntryWidgetBase>* FoundEntry = DataObjectListEntryMap.Find(DataClass))
		return *FoundEntry;
	
	for (const auto& Pair : DataObjectListEntryMap)
		if (InDataObject->IsA(Pair.Key))
			return Pair.Value;

	return nullptr;
}
