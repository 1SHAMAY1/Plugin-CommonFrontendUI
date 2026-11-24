#include "Widgets/DataObject/ListDataObjectString.h"

#include "CommonFrontendLibrary.h"
#include "Data/OptionsDataInteractionHelper.h"

void UListDataObjectString::SetItems(const TArray<FString>& InItems)
{
	AvailableOptionsStringArray.Empty();
	AvailableOptionsTextArray.Empty();

	for (const FString& Item : InItems)
	{
		AddDynamicOption(Item, FText::FromString(Item));
	}
}

void UListDataObjectString::AddDynamicOption(const FString& InStringValue, const FText& InDisplayText)
{
	AvailableOptionsStringArray.Add(InStringValue);
	AvailableOptionsTextArray.Add(InDisplayText);
}

void UListDataObjectString::AdvanceToNextOption()
{
	if (AvailableOptionsStringArray.IsEmpty() || AvailableOptionsTextArray.IsEmpty())
	{
		return;
	}

	const int32 CurrentDisplayIndex = AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue);
	const int32 NextIndexToDisplay = CurrentDisplayIndex + 1;

	if (const bool bIsNextIndexValid = AvailableOptionsStringArray.IsValidIndex(NextIndexToDisplay))
	{
		CurrentStringValue = AvailableOptionsStringArray[NextIndexToDisplay];
	}
	else
	{
		CurrentStringValue = AvailableOptionsStringArray[0];
	}

	TrySetDisplayTextFromStringValue(CurrentStringValue);

	TrySetDisplayTextFromStringValue(CurrentStringValue);
	
	if (DataDynamicSetter || GetBufferChanges())
	{
		SetValueInternal(CurrentStringValue);
		
		NotifyListDataModified(this);
	}
}

void UListDataObjectString::BackToPreviousOption()
{
	if (AvailableOptionsStringArray.IsEmpty() || AvailableOptionsTextArray.IsEmpty())
	{
		return;
	}

	if (UCommonFrontendLibrary::ShouldEnableDebug())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ListDataObjectString] BackToPreviousOption: Current='%s' | ArrayNum=%d"), *CurrentStringValue, AvailableOptionsStringArray.Num());
	}

	const int32 CurrentDisplayIndex = AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue);
	const int32 PreviousIndexToDisplay = CurrentDisplayIndex - 1;

	if (UCommonFrontendLibrary::ShouldEnableDebug())
	{
		UE_LOG(LogTemp, Warning, TEXT("  - CurrentIndex=%d | PrevIndex=%d"), CurrentDisplayIndex, PreviousIndexToDisplay);
	}

	if (const bool bIsPreviousIndexValid = AvailableOptionsStringArray.IsValidIndex(PreviousIndexToDisplay))
	{
		CurrentStringValue = AvailableOptionsStringArray[PreviousIndexToDisplay];
		if (UCommonFrontendLibrary::ShouldEnableDebug())
			UE_LOG(LogTemp, Warning, TEXT("  - Valid Index. NewValue='%s'"), *CurrentStringValue);
	}
	else
	{
		CurrentStringValue = AvailableOptionsStringArray.Last();
		if (UCommonFrontendLibrary::ShouldEnableDebug())
			UE_LOG(LogTemp, Warning, TEXT("  - Wrapped to Last. NewValue='%s'"), *CurrentStringValue);
	}

	TrySetDisplayTextFromStringValue(CurrentStringValue);

	if (DataDynamicSetter || GetBufferChanges())
	{
		SetValueInternal(CurrentStringValue);

		NotifyListDataModified(this);
	}
}

void UListDataObjectString::OnRotatorInitiatedValueChange(const FText& InNewSelectedText)
{
	const int32 FoundIndex = AvailableOptionsTextArray.IndexOfByPredicate(
		[InNewSelectedText](const FText& AvailableText)->bool
		{
			return AvailableText.EqualTo(InNewSelectedText);
		}
	);

	if (FoundIndex != INDEX_NONE && AvailableOptionsStringArray.IsValidIndex(FoundIndex))
	{
		CurrentDisplayText = InNewSelectedText;
		CurrentStringValue = AvailableOptionsStringArray[FoundIndex];

		if (DataDynamicSetter || GetBufferChanges())
		{
			SetValueInternal(CurrentStringValue);

			NotifyListDataModified(this);
		}
	}
}

void UListDataObjectString::SetInvalidDisplayValue(const FText& InDisplayText)
{
	InvalidDisplayText = InDisplayText;
}

void UListDataObjectString::OnDataObjectInitialized()
{
	if (!AvailableOptionsStringArray.IsEmpty())
	{
		CurrentStringValue = AvailableOptionsStringArray[0];
	}

	if (HasDefaultValue())
	{
		CurrentStringValue = GetDefaultValueAsString();
	}

	if (DataDynamicGetter || GetBufferChanges())
	{
		const FString InternalVal = GetValueInternal();
		if (!InternalVal.IsEmpty())
		{
			CurrentStringValue = InternalVal;
		}
	}

	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = InvalidDisplayText;
	}
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	UE_LOG(LogTemp, Log, TEXT("[ListDataObjectString] Init: ID=%s | Value='%s' | Text='%s' | ValidOption=%d"), 
		*GetDataID().ToString(), *CurrentStringValue, *CurrentDisplayText.ToString(), AvailableOptionsStringArray.Contains(CurrentStringValue));
}


bool UListDataObjectString::CanResetBackToDefaultValue() const
{
	// Check Committed Value (saved state) instead of Current Value (buffer)
	return HasDefaultValue() && GetCommittedValue() != GetDefaultValueAsString();
}

bool UListDataObjectString::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		CurrentStringValue = GetDefaultValueAsString();

		TrySetDisplayTextFromStringValue(CurrentStringValue);

		if (DataDynamicSetter || GetBufferChanges())
		{
			SetValueInternal(CurrentStringValue);

			NotifyListDataModified(this,EOptionsListDataModifyReason::ResetToDefault);

			return true;
		}
	}

	return false;
}

bool UListDataObjectString::CanSetToForcedStringValue(const FString& InForcedValue) const
{
	return CurrentStringValue != InForcedValue;
}

void UListDataObjectString::OnSetToForcedStringValue(const FString& InForcedValue)
{
	CurrentStringValue = InForcedValue;
	TrySetDisplayTextFromStringValue(CurrentStringValue);

	TrySetDisplayTextFromStringValue(CurrentStringValue);

	if (DataDynamicSetter || GetBufferChanges())
	{
		SetValueInternal(CurrentStringValue);

		NotifyListDataModified(this,EOptionsListDataModifyReason::DependencyModified);
	}
}

bool UListDataObjectString::TrySetDisplayTextFromStringValue(const FString& InStringValue)
{
	const int32 CurrentFoundIndex = AvailableOptionsStringArray.IndexOfByKey(InStringValue);

	if (UCommonFrontendLibrary::ShouldEnableDebug())
	UE_LOG(LogTemp, Log, TEXT("[ListDataObjectString] TrySetDisplayText: ID=%s | LookingFor='%s' | FoundIndex=%d"), 
		*GetDataID().ToString(), *InStringValue, CurrentFoundIndex);

	if (AvailableOptionsTextArray.IsValidIndex(CurrentFoundIndex))
	{
		CurrentDisplayText = AvailableOptionsTextArray[CurrentFoundIndex];

		return true;
	}

	return false;
}

void UListDataObjectString::OnEditDependencyDataModified(UListDataObjectBase* ModifiedDependencyData, EOptionsListDataModifyReason ModifyReason)
{
	if (DataDynamicGetter || GetBufferChanges())
	{	
		FString NewVal = GetValueInternal();
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Log, TEXT("[ListDataObjectString] Dependency Update for [%s] triggered by [%s]. Old: %s, New: %s"), 
			*GetDataID().ToString(), 
			*ModifiedDependencyData->GetDataID().ToString(), 
			*CurrentStringValue, 
			*NewVal);

		if (CurrentStringValue == NewVal && !ModifiedDependencyData->IsDirty())
		{
			if (UCommonFrontendLibrary::ShouldEnableDebug())
			UE_LOG(LogTemp, Log, TEXT("  - Values match and Dependency not dirty. Skipping update."));
			return;
		}
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Log, TEXT("  - Updating CurrentStringValue to '%s'"), *NewVal);
		CurrentStringValue = NewVal;

		if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
		{
			if (UCommonFrontendLibrary::ShouldEnableDebug())
			UE_LOG(LogTemp, Warning, TEXT("[ListDataObjectString] Dependency Update: Value '%s' not found. Setting Text to InvalidDisplayText: '%s'."), *CurrentStringValue, *InvalidDisplayText.ToString());
			CurrentDisplayText = InvalidDisplayText;
		}
		else if (UCommonFrontendLibrary::ShouldEnableDebug())
		{
			UE_LOG(LogTemp, Log, TEXT("  - Successfully set display text for '%s'"), *CurrentStringValue);
		}

		NotifyListDataModified(this,EOptionsListDataModifyReason::DependencyModified);
	}
}

void UListDataObjectBool::OverrideTrueDisplayText(const FText& InNewTrueDisplayText)
{
	if (!AvailableOptionsStringArray.Contains(TrueString))
	{
		AddDynamicOption(TrueString,InNewTrueDisplayText);
	}
}

void UListDataObjectBool::OverrideFalseDisplayText(const FText& InNewFalseDisplayText)
{
	if (!AvailableOptionsStringArray.Contains(FalseString))
	{
		AddDynamicOption(FalseString,InNewFalseDisplayText);
	}
}

void UListDataObjectBool::SetTrueAsDefaultValue()
{
	SetDefaultValueFromString(TrueString);
}

void UListDataObjectBool::SetFalseAsDefaultValue()
{
	SetDefaultValueFromString(FalseString);
}

void UListDataObjectBool::OnDataObjectInitialized()
{
	TryInitBoolValues();

	Super::OnDataObjectInitialized();
}

void UListDataObjectBool::TryInitBoolValues()
{
	if (!AvailableOptionsStringArray.Contains(TrueString))
	{
		AddDynamicOption(TrueString,FText::FromString(TEXT("ON")));
	}

	if (!AvailableOptionsStringArray.Contains(FalseString))
	{
		AddDynamicOption(FalseString,FText::FromString(TEXT("OFF")));
	}
}

void UListDataObjectInteger::AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText)
{
	AddDynamicOption(LexToString(InIntegerValue),InDisplayText);

}

void UListDataObjectInteger::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();

	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = FText::FromString(TEXT("Custom"));
	}
}

void UListDataObjectInteger::OnEditDependencyDataModified(UListDataObjectBase* ModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	if (DataDynamicGetter || GetBufferChanges())
	{	
		FString NewVal = GetValueInternal();
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Log, TEXT("Dependency Update for [%s] triggered by [%s]. Old: %s, New: %s"), 
			*GetDataID().ToString(), 
			*ModifiedDependencyData->GetDataID().ToString(), 
			*CurrentStringValue, 
			*NewVal);

		if (CurrentStringValue == NewVal)
		{
			return;
		}

		CurrentStringValue = NewVal;

		if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
		{
			if (UCommonFrontendLibrary::ShouldEnableDebug())
			UE_LOG(LogTemp, Warning, TEXT("[ListDataObjectInteger] Dependency Update: Value '%s' not found. Setting Text to InvalidDisplayText: '%s'."), *CurrentStringValue, *InvalidDisplayText.ToString());
			CurrentDisplayText = InvalidDisplayText;
		}

		NotifyListDataModified(this,EOptionsListDataModifyReason::DependencyModified);
	}

	Super::OnEditDependencyDataModified(ModifiedDependencyData, ModifyReason);
}

void UListDataObjectString::CleanUp()
{
	Super::CleanUp();

	if (DataDynamicGetter || GetBufferChanges())
	{
		ClearBufferedChanges();
		CurrentStringValue = GetCommittedValue();
		TrySetDisplayTextFromStringValue(CurrentStringValue);
		NotifyListDataModified(this, EOptionsListDataModifyReason::CleanUp);
	}
}
