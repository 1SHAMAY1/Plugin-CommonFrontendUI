#pragma once

#include "CoreMinimal.h"
#include "Data/CommonFrontendUI_StructsAndEnums.h"
class UTexture2D;

#include "ListDataObjectBase.generated.h"

/**
 * Macro to define standard getter/setter accessors for list data properties.
 */
#define LIST_DATA_ACCESSOR(DataType, PropertyName) \
FORCEINLINE DataType Get##PropertyName() const { return PropertyName; } \
void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName; }

/**
 * Base class for all list data objects used in the frontend UI system.
 * 
 * This class represents a single configurable or displayable item within a list-based UI system.
 * It supports parent-child relationships, dynamic edit conditions, and update notifications.
 */
UCLASS(Abstract)
class COMMONFRONTENDUI_API UListDataObjectBase : public UObject
{
	GENERATED_BODY()

public:

	/** Delegate broadcast when this data object is modified. */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListDataModifiedDelegate, UListDataObjectBase*, EOptionsListDataModifyReason)

	/** Called when this object's data is directly or indirectly modified. */
	FOnListDataModifiedDelegate OnListDataModified;

	/** Called when dependent (linked) data objects are modified. */
	FOnListDataModifiedDelegate OnDependencyDataModified;

	/** Standard accessors for common data properties */
	LIST_DATA_ACCESSOR(FName, DataID)
	LIST_DATA_ACCESSOR(FText, DataDisplayName)
	LIST_DATA_ACCESSOR(FText, DescriptionRichText)
	LIST_DATA_ACCESSOR(FText, DisabledRichText)
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage)
	LIST_DATA_ACCESSOR(UListDataObjectBase*, ParentData)

	/** Initializes this data object after creation (manual or dynamic). */
	void InitDataObject();

	/** Returns all child list data objects (if any). Override to provide custom behavior. */
	virtual TArray<UListDataObjectBase*> GetAllChildListData() const { return TArray<UListDataObjectBase*>(); }

	/** Returns true if this object has any children. */
	virtual bool HasAnyChildListData() const { return false; }

	/** Whether to apply the changes in real-time when modified. */
	void SetShouldApplySettingsImmediately(bool bShouldApplyRightAway) { bShouldApplyChangeImmediatly = bShouldApplyRightAway; }

	/** Checks if the changes should be applied immediately. */
	bool ShouldApplySettingsImmediately() const { return bShouldApplyChangeImmediatly; }

	/** Returns true if the value is currently the default. */
	virtual bool HasDefaultValue() const { return false; }

	/** Whether the object can reset back to a known default value. */
	virtual bool CanResetBackToDefaultValue() const { return false; }

	/** Attempts to reset to the default value. Returns success. */
	virtual bool TryResetBackToDefaultValue() { return false; }

	/** Adds a new condition to determine whether the data is currently editable. */
	void AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition);

	/** Links a dependency on another data object. When it changes, this may also change. */
	void AddEditDependencyData(UListDataObjectBase* InDependencyData);

	/** Evaluates all edit conditions and dependencies to determine if this data is editable. */
	bool IsDataCurrentlyEditable();

	/** Notifies the system that this data object was modified. */
	virtual void NotifyListDataModified(UListDataObjectBase* ModifiedData, EOptionsListDataModifyReason ModifyReason = EOptionsListDataModifyReason::DirectlyModified);

	/** Returns wheteher is dirty in buffer. */
	virtual bool IsDirty() const { return false;};

	/** Updates Current value to the Internal value removing buffer, thus performing a cleanup. */
	virtual void CleanUp();

protected:

	/** Called when the object is initialized and ready. Override for custom setup logic. */
	virtual void OnDataObjectInitialized();
	
	/** Determines if this object can be set from a forced string value. */
	virtual bool CanSetToForcedStringValue(const FString& InForcedValue) const { return false; }

	/** Called when this object is being forced to apply a string value. */
	virtual void OnSetToForcedStringValue(const FString& InForcedValue) {}

	/** Called when a dependency (linked object) was modified. */
	virtual void OnEditDependencyDataModified(UListDataObjectBase* ModifiedDependencyData, EOptionsListDataModifyReason ModifyReason);
	
private:

	/** Unique identifier for this data entry. */
	FName DataID;

	/** Display name used in the UI. */
	FText DataDisplayName;

	/** Rich text description shown when enabled. */
	FText DescriptionRichText;

	/** Rich text shown when this data is disabled/uneditable. */
	FText DisabledRichText;

	/** Optional image shown in association with this data. */
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage;

	/** Parent object in a hierarchical list structure (transient only). */
	UPROPERTY(Transient)
	UListDataObjectBase* ParentData;

	/** If true, apply changes immediately when this object is modified. */
	bool bShouldApplyChangeImmediatly = false;

	/** All active edit conditions used to determine visibility/editability. */
	UPROPERTY(Transient)
	TArray<FOptionsDataEditConditionDescriptor> EditConditionDescArray;
};
