#pragma once

#include "CoreMinimal.h"
class ULocalPlayer;

#include "CommonFrontendDataRegistry.generated.h"

class UListDataObjectBase;
class UListDataObjectCollection;
class UCommonFrontendSettingsRegistry;

/**
 * UCommonFrontendDataRegistry
 *
 * Acts as the centralized data manager for front-end settings data used across the CommonFrontend UI.
 * This registry initializes and holds references to options tabs such as Audio, Video, and Controls.
 * It provides utilities for accessing UI data collections (e.g., for ListView/DataTable/TreeView-driven UIs).
 * It also provides child-item resolution recursively for hierarchical list data objects.
 */
UCLASS()
class COMMONFRONTENDUI_API UCommonFrontendDataRegistry : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	 * Initializes the registry using a data asset.
	 * Dynamically creates tabs and settings links based on the asset definition.
	 */
	virtual void InitFromSettingsRegistry(const UCommonFrontendSettingsRegistry* Registry, ULocalPlayer* InOwningLocalPlayer);

	/** Returns the list of registered tab collections (e.g., Audio, Video, Control tabs). */
	const TArray<UListDataObjectCollection*>& GetRegisteredOptionsTabCollections() const { return RegisteredOptionsTabCollections; }

	/**
	 * Fetches all list data items that are under a specific top-level tab (by ID).
	 * This is typically used to filter and show tab-specific data in frontend UI panels.
	 *
	 * @param InSelectedTabID - The identifier of the currently selected tab.
	 * @return Array of all list data objects associated with the given tab.
	 */
	TArray<UListDataObjectBase*> GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID) const;

protected:

	/**
	 * Populates child list data recursively for a given parent node.
	 * This supports tree-style UI representations where nested categories are common.
	 *
	 * @param InParentData - The parent list data object.
	 * @param OutFoundChildListData - Output array for all found children recursively.
	 */
	void FindChildListDataRecursively(UListDataObjectBase* InParentData, TArray<UListDataObjectBase*>& OutFoundChildListData) const;



private:

	/** Array of tab-level collections that represent categories in the frontend settings UI. */
	UPROPERTY(Transient)
	TArray<UListDataObjectCollection*> RegisteredOptionsTabCollections;

	/** Map of DataID -> DataObject for dependency linking. */
	TMap<FName, UListDataObjectBase*> CreatedDataObjectsMap;

	/** Map of DataObject -> array of dependent IDs to link after processing. */
	TMap<UListDataObjectBase*, TArray<FName>> PendingDependencyLinks;

	/** Helper to process a setting definition and add it to a parent collection. */
	void ProcessSettingDef(const class UFrontendSettingDef* SettingDef, UListDataObjectCollection* ParentCollection, class UEnhancedInputUserSettings* EIUserSettings);

	/** Links all pending dependencies after all data objects are created. */
	void LinkPendingDependencies();
};
