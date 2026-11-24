#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataListEntryMappingConfig.generated.h"

class UListEntryWidgetBase;
class UListDataObjectBase;

/**
 * UDataListEntryMappingConfig
 *
 * A data asset that defines mappings between data object classes and their corresponding list entry widget classes.
 * Used in modular UI systems to drive list-based UIs (e.g., settings menus, inventories, quest logs) via class-based mapping.
 * 
 * This system avoids hardcoded widget bindings by allowing UI population to be fully data-driven.
 * Typically used by UI systems that dynamically spawn list entry widgets based on the data object class provided at runtime.
 */
UCLASS(BlueprintType)
class COMMONFRONTENDUI_API UDataListEntryMappingConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	/**
	 * Returns the associated list entry widget class for the provided data object instance.
	 * Will return nullptr if no match is found.
	 *
	 * @param InDataObject		The data object instance used to find a corresponding widget class.
	 * @return					The matching list entry widget class or nullptr if not found.
	 */
	TSubclassOf<UListEntryWidgetBase> FindEntryWidgetClassByDataObject(UListDataObjectBase* InDataObject) const;

private:

	/** Maps data object classes to their corresponding list entry widget classes. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "List Entry Mapping", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<UListDataObjectBase>, TSubclassOf<UListEntryWidgetBase>> DataObjectListEntryMap;
};
