#pragma once

#include "CoreMinimal.h"
#include "Widgets/DataObject/ListDataObjectValue.h"
#include "ListDataObjectString.generated.h"

/**
 * UListDataObjectString
 *
 * Represents a string-based data object used in option list UI.
 * Supports navigation between predefined string options and tracks the current selection.
 * Stores both internal string values and user-facing display text.
 */
UCLASS()
class COMMONFRONTENDUI_API UListDataObjectString : public UListDataObjectValue
{
	GENERATED_BODY()

public:
	/** Adds a new selectable option consisting of a string value and its display text */
	void AddDynamicOption(const FString& InStringValue, const FText& InDisplayText);

	/** Sets the list of available options (Value and Display Text are same) */
	void SetItems(const TArray<FString>& InItems);

	/** Moves selection to the next available option in the list */
	void AdvanceToNextOption();

	/** Moves selection to the previous available option in the list */
	void BackToPreviousOption();

	/** Updates current selection based on rotator UI input */
	void OnRotatorInitiatedValueChange(const FText& InNewSelectedText);

	/** Updates invalid display text */
	void SetInvalidDisplayValue(const FText& InDisplayText);

	virtual void CleanUp() override;

protected:
	virtual void OnDataObjectInitialized() override;
	virtual bool CanResetBackToDefaultValue() const override;
	virtual bool TryResetBackToDefaultValue() override;
	virtual bool CanSetToForcedStringValue(const FString& InForcedValue) const override;
	virtual void OnSetToForcedStringValue(const FString& InForcedValue) override;
	virtual void OnEditDependencyDataModified(UListDataObjectBase* ModifiedDependencyData, EOptionsListDataModifyReason ModifyReason) override;

	/** Finds and updates display text based on a given string value */
	bool TrySetDisplayTextFromStringValue(const FString& InStringValue);

	/** Internal string value representing the current selection */
	FString CurrentStringValue;

	/** Text shown to the user for the current selection */
	FText CurrentDisplayText;

	/** Text shown to the user for the invalid selection */
	FText InvalidDisplayText;

	/** Array of available string values */
	TArray<FString> AvailableOptionsStringArray;

	/** Array of associated display texts for each option */
	TArray<FText> AvailableOptionsTextArray;

public:
	/** Returns all available display texts */
	FORCEINLINE const TArray<FText>& GetAvailableOptionsTextArray() const { return AvailableOptionsTextArray; }

	/** Returns the current display text */
	FORCEINLINE FText GetCurrentDisplayText() const { return CurrentDisplayText; }
};

/**
 * UListDataObjectBool
 *
 * Specialized boolean data object using true/false string options.
 * Allows customization of display text for both states.
 */
UCLASS()
class COMMONFRONTENDUI_API UListDataObjectBool : public UListDataObjectString
{
	GENERATED_BODY()

public:
	/** Sets a custom display text for the 'true' state */
	void OverrideTrueDisplayText(const FText& InNewTrueDisplayText);

	/** Sets a custom display text for the 'false' state */
	void OverrideFalseDisplayText(const FText& InNewFalseDisplayText);

	/** Marks 'true' as the default value */
	void SetTrueAsDefaultValue();

	/** Marks 'false' as the default value */
	void SetFalseAsDefaultValue();

protected:
	virtual void OnDataObjectInitialized() override;

private:
	/** Initializes the available true/false options */
	void TryInitBoolValues();

	const FString TrueString = TEXT("true");
	const FString FalseString = TEXT("false");
};

/**
 * UListDataObjectEnum
 *
 * Template-based enum data object that allows enum values to be used as selectable options.
 * Converts enum values to string and back for storage and display.
 */
UCLASS()
class COMMONFRONTENDUI_API UListDataObjectEnum : public UListDataObjectString
{
	GENERATED_BODY()

public:
	/** Adds a new enum value as an option with a display label */
	template<typename EnumType>
	void AddEnumOption(EnumType InEnumOption, const FText& InDisplayText)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);

		AddDynamicOption(ConvertedEnumString, InDisplayText);
	}

	/** Returns the current selection as an enum value */
	template<typename EnumType>
	EnumType GetCurrentValueAsEnum() const
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		return (EnumType)StaticEnumOption->GetValueByNameString(CurrentStringValue);
	}

	/** Sets the default value using an enum option */
	template<typename EnumType>
	void SetDefaultValueFromEnumOption(EnumType InEnumOption)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);

		SetDefaultValueFromString(ConvertedEnumString);
	}
};

/**
 * UListDataObjectInteger
 *
 * Integer-based data object supporting multiple predefined values and display texts.
 * Inherits string-based interface but internally works with int32.
 */
UCLASS()
class COMMONFRONTENDUI_API UListDataObjectInteger : public UListDataObjectString
{
	GENERATED_BODY()

public:
	/** Adds an integer value as a selectable option with display text */
	void AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText);

protected:
	virtual void OnDataObjectInitialized() override;
	virtual void OnEditDependencyDataModified(UListDataObjectBase* ModifiedDependencyData, EOptionsListDataModifyReason ModifyReason) override;
};
