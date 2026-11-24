#pragma once

#include "CoreMinimal.h"
#include "CommonNumericTextBlock.h"
#include "Widgets/DataObject/ListDataObjectValue.h"
#include "ListDataObjectScalar.generated.h"

/**
 * UListDataObjectScalar
 *
 * Represents a scalar (float) value object used in settings UI, such as sliders or numeric input fields.
 * Extends UListDataObjectValue to allow editing, formatting, and converting scalar values to and from strings.
 *
 * Key Features:
 * - Supports separate display and output value ranges (e.g., [0–100] for display but [0.0–1.0] for output).
 * - Customizable numeric formatting options, including decimal precision and numeric type (e.g., percentage).
 * - Integrates with slider UI to convert values between slider position and final output.
 * - Includes logic to reset to default, respond to data dependencies, and handle formatting conversions.
 *
 * Intended for use in data-driven UI systems where values need to be exposed in a controlled and user-friendly manner.
 */
UCLASS()
class COMMONFRONTENDUI_API UListDataObjectScalar : public UListDataObjectValue
{
	GENERATED_BODY()

public:
	/** The range of values displayed to the user (e.g., 0–100 for a volume slider). */
	LIST_DATA_ACCESSOR(TRange<float>, DisplayValueRange)

	/** The actual output range used in backend logic (e.g., 0.0–1.0 for normalized values). */
	LIST_DATA_ACCESSOR(TRange<float>, OutputValueRange)

	/** The step size for the slider or increment control (e.g., 0.1 = 10 steps in a 0–1 range). */
	LIST_DATA_ACCESSOR(float, SliderStepSize)

	/** The numeric type used for display (raw number, percentage, etc.). */
	LIST_DATA_ACCESSOR(ECommonNumericType, DisplayNumericType)

	/** Formatting options for rendering the numeric value, including rounding and digit count. */
	LIST_DATA_ACCESSOR(FCommonNumberFormattingOptions, NumberFormattingOptions)

	/** Provides a common formatting config with no decimal precision (integer display). */
	static FCommonNumberFormattingOptions NoDecimal();

	/** Provides formatting config with a specified number of fractional digits. */
	static FCommonNumberFormattingOptions WithDecimal(int32 NumFracDigit);

	/** Returns the currently represented value (converted from string if needed). */
	float GetCurrentValue() const;

	/** Updates the value based on a normalized slider input (maps to output range and stores as string). */
	void SetCurrentValueFromSlider(float InNewValue);

	virtual void CleanUp() override;

private:
	/** Returns true if the scalar value can be reset to its default state. */
	virtual bool CanResetBackToDefaultValue() const override;

	/** Tries to reset the scalar value to its default state. Returns true on success. */
	virtual bool TryResetBackToDefaultValue() override;
	
	virtual bool IsDirty() const override;

	/** Called when a dependent data object is modified, allowing dynamic response to linked changes. */
	virtual void OnEditDependencyDataModified(UListDataObjectBase* ModifiedDependencyData, EOptionsListDataModifyReason ModifyReason) override;

	/** Converts the underlying string value to a float (clamping or validating as needed). */
	float StringToFloat(const FString& InString) const;

	// Internal config and formatting parameters
	TRange<float> DisplayValueRange = TRange<float>(0.f, 1.f);
	TRange<float> OutputValueRange = TRange<float>(0.f, 1.f);
	float SliderStepSize = 0.1f;
	ECommonNumericType DisplayNumericType = ECommonNumericType::Number;
	FCommonNumberFormattingOptions NumberFormattingOptions;
};
