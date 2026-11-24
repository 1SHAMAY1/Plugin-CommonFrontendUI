#pragma once

#include "CoreMinimal.h"
#include "Widgets/DataObject/ListDataObjectBase.h"
#include "ListDataObjectValue.generated.h"

class FOptionsDataInteractionHelper;

/**
 * Abstract base class for all value-based list data objects.
 * 
 * This class supports binding to dynamic getter and setter functions,
 * and handles default values as strings. It serves as the foundation
 * for all configurable list data entries such as strings, bools, enums, etc.
 */
UCLASS(Abstract)
class COMMONFRONTENDUI_API UListDataObjectValue : public UListDataObjectBase
{
	GENERATED_BODY()

public:

	/** Assigns a dynamic getter delegate for retrieving current data value. */
	void SetDataDynamicGetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicGetter);

	/** Assigns a dynamic setter delegate for updating the current data value. */
	void SetDataDynamicSetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicSetter);

	/** Sets whether changes should be buffered locally instead of applied immediately. */
	void SetBufferChanges(bool bInBufferChanges) { bBufferChanges = bInBufferChanges; }

	/** Commits any buffered changes to the underlying setter. */
	void CommitBufferedChanges();

	/** Clears any buffered changes, reverting to the underlying getter value. */
	void ClearBufferedChanges();
	
	/** Returns the current buffered value if any. */
	bool HasBufferedChanges() const { return PendingBufferedValue.IsSet(); }

	/** Sets the default string value for this data object. */
	void SetDefaultValueFromString(const FString& InDefaultValue) { DefaultStringValue = InDefaultValue; }

	/** Returns true if a default value has been assigned. */
	virtual bool HasDefaultValue() const override { return DefaultStringValue.IsSet(); }

	/** Retrieves the default string value (only call if `HasDefaultValue()` is true). */
	FString GetDefaultValueAsString() const { return DefaultStringValue.GetValue(); }

	virtual bool IsDirty() const override;

	FString GetCommittedValue() const;

	/** Helper to get the current value (buffered or underlying) as string. */
	FString GetCurrentValueAsString() const { return GetValueInternal(); }

	/** Optional dynamic getter bound to live game data. */
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicGetter;

	/** Optional dynamic setter bound to live game data. */
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicSetter;

	bool GetBufferChanges() const { return bBufferChanges; }

private:

	/** The default value stored as a string for consistency across derived types. */
	TOptional<FString> DefaultStringValue;

	/** If true, changes are stored in PendingBufferedValue instead of calling DataDynamicSetter. */
	bool bBufferChanges = false;

	/** Locally buffered value that hasn't been committed yet. */
	TOptional<FString> PendingBufferedValue;

protected:
	/** Helper to set value, respecting buffering. */
	void SetValueInternal(const FString& InValue);

	/** Helper to get value, respecting buffering. */
	FString GetValueInternal() const;
};
