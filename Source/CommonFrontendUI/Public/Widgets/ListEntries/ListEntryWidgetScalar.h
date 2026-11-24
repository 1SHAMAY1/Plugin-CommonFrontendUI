#pragma once

#include "CoreMinimal.h"
#include "Widgets/ListEntries/ListEntryWidgetBase.h"
#include "ListEntryWidgetScalar.generated.h"

class UListDataObjectScalar;
class UCommonNumericTextBlock;
class UAnalogSlider;

/**
 * UListEntryWidgetScalar
 * 
 * A UI list entry widget that represents a scalar (float) setting,
 * such as volume, brightness, or any tunable numeric value.
 * 
 * It provides an analog slider and numeric display to show and adjust the value.
 * This class binds to a UListDataObjectScalar instance to reflect and update its value.
 * 
 * This widget supports data binding and user interaction through slider movement,
 * and reacts to data changes and UI focus/selection updates.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class COMMONFRONTENDUI_API UListEntryWidgetScalar : public UListEntryWidgetBase
{
	GENERATED_BODY()

protected:
	/** Called once during initialization; binds delegates and setups up default state */
	virtual void NativeOnInitialized() override;

	/** Called when the associated data object is set for this widget */
	virtual void OnOwningListDataObjectSet(UListDataObjectBase* InOwningListDataObject) override;

	/** Called when the associated data object is modified */
	virtual void OnOwningListDataObjectModified(UListDataObjectBase* OwningModifiedData, EOptionsListDataModifyReason ModifyReason) override;
	virtual UWidget* NativeGetWidgetToFocusForGamepad() const override;

private:
	/** Callback when the slider value is changed by user interaction */
	UFUNCTION()
	void OnSliderValueChanged(float Value);

	/** Callback when the user begins dragging the slider */
	UFUNCTION()
	void OnSliderMouseCaptureBegin();

	/** Reference to the numeric text block displaying the current scalar value */
	UPROPERTY(BlueprintReadOnly, Category = "Scalar Setting|UI", meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonNumericTextBlock* CommonNumeric_SettingValue;

	/** Reference to the analog slider widget controlling the scalar value */
	UPROPERTY(BlueprintReadOnly, Category = "Scalar Setting|UI", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnalogSlider* AnalogSlider_SettingSlider;

	/** Cached reference to the scalar data object providing value/state for this widget */
	UPROPERTY(Transient)
	UListDataObjectScalar* CachedOwningScalarDataObject;

};
