#include "Widgets/DataObject/ListDataObjectScalar.h"

#include "CommonFrontendLibrary.h"
#include "Data/OptionsDataInteractionHelper.h"

FCommonNumberFormattingOptions UListDataObjectScalar::NoDecimal()
{
	FCommonNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = 0;

	return Options;
}

FCommonNumberFormattingOptions UListDataObjectScalar::WithDecimal(int32 NumFracDigit)
{
	FCommonNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = NumFracDigit;

	return Options;
}

float UListDataObjectScalar::GetCurrentValue() const
{
	return FMath::GetMappedRangeValueClamped(
		FVector2D(OutputValueRange.GetLowerBoundValue(), OutputValueRange.GetUpperBoundValue()),
		FVector2D(DisplayValueRange.GetLowerBoundValue(), DisplayValueRange.GetUpperBoundValue()),
		StringToFloat(GetValueInternal())
	);
}

void UListDataObjectScalar::SetCurrentValueFromSlider(float InNewValue)
{
	const float ClampedValue = FMath::GetMappedRangeValueClamped(
		FVector2D(DisplayValueRange.GetLowerBoundValue(), DisplayValueRange.GetUpperBoundValue()),
		FVector2D(OutputValueRange.GetLowerBoundValue(), OutputValueRange.GetUpperBoundValue()),
		InNewValue
	);

	SetValueInternal(LexToString(ClampedValue));

	if (UCommonFrontendLibrary::ShouldEnableDebug())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ListDataObjectScalar] SetCurrentValueFromSlider: Input=%f, Clamped=%f, ID=%s"), 
			InNewValue, ClampedValue, *GetDataID().ToString());
	}

	NotifyListDataModified(this);
}

bool UListDataObjectScalar::CanResetBackToDefaultValue() const
{
	if (HasDefaultValue())
	{	
		const float DefaultValue = StringToFloat(GetDefaultValueAsString());
		// Check Committed Value (saved state) instead of Current Value (buffer)
		const float CommittedValue = StringToFloat(GetCommittedValue());

		const bool bNotEqual = !FMath::IsNearlyEqual(DefaultValue,CommittedValue,0.01f);
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Warning, TEXT("[ListDataObjectScalar] CanResetBackToDefaultValue: %s | Default=%f, Committed=%f, Diff=%f, Res=%d"), 
			*GetDataID().ToString(), DefaultValue, CommittedValue, FMath::Abs(DefaultValue - CommittedValue), bNotEqual);

		return bNotEqual;
	}

	return false;
}

bool UListDataObjectScalar::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		SetValueInternal(GetDefaultValueAsString());

		NotifyListDataModified(this,EOptionsListDataModifyReason::ResetToDefault);

		return true;
	}

	return false;
}

bool UListDataObjectScalar::IsDirty() const
{
	if (!HasBufferedChanges())
	{
		return false;
	}

	const FString CurrentValueStr = GetValueInternal();
	const FString CommittedValueStr = GetCommittedValue();

	const float CurrentValue = StringToFloat(CurrentValueStr);
	const float CommittedValue = StringToFloat(CommittedValueStr);

	const bool bIsDirty = !FMath::IsNearlyEqual(CurrentValue, CommittedValue, 0.0001f);

	if (bIsDirty)
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Warning, TEXT("[ListDataObjectScalar] IsDirty (Float Check): ID=%s | Buffered='%s' (%.6f) | Committed='%s' (%.6f) | Dirty=%d"), 
			*GetDataID().ToString(), *CurrentValueStr, CurrentValue, *CommittedValueStr, CommittedValue, bIsDirty);
	}

	return bIsDirty;
}

void UListDataObjectScalar::OnEditDependencyDataModified(UListDataObjectBase* ModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ListDataObjectScalar] OnEditDependencyDataModified: ID=%s triggered by %s (Reason=%d)."), 
			*GetDataID().ToString(), 
			ModifiedDependencyData ? *ModifiedDependencyData->GetDataID().ToString() : TEXT("Null"), 
			(int32)ModifyReason);
	}

	// Prevent cyclic updates: If the dependency (e.g. Overall Quality) was modified purely as a reaction 
	// to another setting (DependencyModified), do not reset this setting. 
	// We only want to reset if the parent was DirectlyModified (e.g. User changed Preset).
	if (ModifyReason == EOptionsListDataModifyReason::DependencyModified)
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
			UE_LOG(LogTemp, Warning, TEXT("  - Ignoring recursive DependencyModified update."));
		
		return;
	}
	
	NotifyListDataModified(this,EOptionsListDataModifyReason::DependencyModified);
	ClearBufferedChanges();
	Super::OnEditDependencyDataModified(ModifiedDependencyData,ModifyReason);
}

void UListDataObjectScalar::CleanUp()
{
	Super::CleanUp();

	if (DataDynamicGetter || GetBufferChanges())
	{
		ClearBufferedChanges();
		SetValueInternal(GetCommittedValue());
		
		NotifyListDataModified(this, EOptionsListDataModifyReason::CleanUp);
	}
}

float UListDataObjectScalar::StringToFloat(const FString& InString) const
{
	float OutConvertedValue = 0.f;

	LexFromString(OutConvertedValue,*InString);

	return OutConvertedValue;
}
