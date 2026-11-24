#include "Widgets/ListEntries/ListEntryWidgetScalar.h"

#include "AnalogSlider.h"
#include "CommonFrontendLibrary.h"
#include "Widgets/DataObject/ListDataObjectScalar.h"

void UListEntryWidgetScalar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	AnalogSlider_SettingSlider->OnValueChanged.AddUniqueDynamic(this,&ThisClass::OnSliderValueChanged);
	AnalogSlider_SettingSlider->OnMouseCaptureBegin.AddUniqueDynamic(this,&ThisClass::OnSliderMouseCaptureBegin);
}



void UListEntryWidgetScalar::OnOwningListDataObjectSet(UListDataObjectBase* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	CachedOwningScalarDataObject = CastChecked<UListDataObjectScalar>(InOwningListDataObject);

	CommonNumeric_SettingValue->SetNumericType(CachedOwningScalarDataObject->GetDisplayNumericType());
	CommonNumeric_SettingValue->FormattingSpecification = CachedOwningScalarDataObject->GetNumberFormattingOptions();
	
	float ValueToDisplay = CachedOwningScalarDataObject->GetCurrentValue();
	if (CachedOwningScalarDataObject->GetDisplayNumericType() == ECommonNumericType::Percentage)
	{
		// Handle 0-100 percentage scaling for display if max range is > 1.0
		if (CachedOwningScalarDataObject->GetDisplayValueRange().GetUpperBoundValue() > 1.0f)
		{
			ValueToDisplay /= 100.0f;
		}
	}
	CommonNumeric_SettingValue->SetCurrentValue(ValueToDisplay);

	AnalogSlider_SettingSlider->SetMinValue(CachedOwningScalarDataObject->GetDisplayValueRange().GetLowerBoundValue());
	AnalogSlider_SettingSlider->SetMaxValue(CachedOwningScalarDataObject->GetDisplayValueRange().GetUpperBoundValue());
	AnalogSlider_SettingSlider->SetStepSize(CachedOwningScalarDataObject->GetSliderStepSize());
	AnalogSlider_SettingSlider->SetValue(CachedOwningScalarDataObject->GetCurrentValue());
}

void UListEntryWidgetScalar::OnOwningListDataObjectModified(UListDataObjectBase* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	Super::OnOwningListDataObjectModified(OwningModifiedData, ModifyReason);

	if (CachedOwningScalarDataObject)
	{
		float ValueToDisplay = CachedOwningScalarDataObject->GetCurrentValue();
		if (CachedOwningScalarDataObject->GetDisplayNumericType() == ECommonNumericType::Percentage)
		{
			// If the display range implies 0-100 scaling (Max > 1.0), normalize for the text block which expects 0-1
			if (CachedOwningScalarDataObject->GetDisplayValueRange().GetUpperBoundValue() > 1.0f)
			{
				ValueToDisplay /= 100.0f;
			}
		}
		CommonNumeric_SettingValue->SetCurrentValue(ValueToDisplay);
		AnalogSlider_SettingSlider->SetValue(CachedOwningScalarDataObject->GetCurrentValue());
	}
}

UWidget* UListEntryWidgetScalar::NativeGetWidgetToFocusForGamepad() const
{
	if (UWidget* BP_ValidWidget = BP_GetWidgetToFocusForGamepad())
		return BP_ValidWidget;
	return AnalogSlider_SettingSlider;
}


void UListEntryWidgetScalar::OnSliderValueChanged(float Value)
{
	if (CachedOwningScalarDataObject)
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		{
			UE_LOG(LogTemp, Warning, TEXT("[ListEntryWidgetScalar] OnSliderValueChanged: %f for %s"), 
				Value, *CachedOwningScalarDataObject->GetDataID().ToString());
		}
		CachedOwningScalarDataObject->SetCurrentValueFromSlider(Value);
	}
}

void UListEntryWidgetScalar::OnSliderMouseCaptureBegin()
{
	SelectThisEntryWidget();
}
