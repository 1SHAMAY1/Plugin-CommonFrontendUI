#include "Widgets/DataObject/ListDataObjectValue.h"

#include "CommonFrontendLibrary.h"
#include "OptionsDataInteractionHelper.h"

void UListDataObjectValue::SetDataDynamicGetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicGetter)
{
	DataDynamicGetter = InDynamicGetter;

}

void UListDataObjectValue::SetDataDynamicSetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicSetter)
{
	DataDynamicSetter = InDynamicSetter;
}

void UListDataObjectValue::CommitBufferedChanges()
{
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ListDataObjectValue] CommitBufferedChanges: ID=%s, HasPending=%d, HasSetter=%d"), 
			*GetDataID().ToString(), PendingBufferedValue.IsSet(), DataDynamicSetter.IsValid());
	}

	if (PendingBufferedValue.IsSet() && DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(PendingBufferedValue.GetValue());
		PendingBufferedValue.Reset();
		
		NotifyListDataModified(this, EOptionsListDataModifyReason::SetFromBuffer);
	}
}

void UListDataObjectValue::ClearBufferedChanges()
{
	PendingBufferedValue.Reset();
}

void UListDataObjectValue::SetValueInternal(const FString& InValue)
{
	if (bBufferChanges)
	{
		PendingBufferedValue = InValue;
	}
	else
	{
		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(InValue);
		}
	}
}

FString UListDataObjectValue::GetValueInternal() const
{
	if (PendingBufferedValue.IsSet())
	{
		return PendingBufferedValue.GetValue();
	}

	if (DataDynamicGetter)
	{
		return DataDynamicGetter->GetValueAsString();
	}

	return FString();
}

FString UListDataObjectValue::GetCommittedValue() const
{
	if (DataDynamicGetter)
	{
		return DataDynamicGetter->GetValueAsString();
	}

	return FString();
}

bool UListDataObjectValue::IsDirty() const
{
	if (!PendingBufferedValue.IsSet())
	{
		return false;
	}

	FString CurrentValue = GetValueInternal();
	FString CommittedValue = GetCommittedValue();

	const bool bIsDirty = !CurrentValue.Equals(CommittedValue, ESearchCase::CaseSensitive);
	
	if (bIsDirty)
	{
		FString DefaultValue = TEXT("N/A");
		if (HasDefaultValue())
		{
			DefaultValue = GetDefaultValueAsString();
		}
		if (UCommonFrontendLibrary::ShouldEnableDebug())
			UE_LOG(LogTemp, Warning, TEXT("[ListDataObjectValue] IsDirty: ID=%s | Buffered='%s' | Committed='%s' | Default='%s' | Dirty=%d"),
				*GetDataID().ToString(), *CurrentValue, *CommittedValue, *DefaultValue, bIsDirty);
	}

	return bIsDirty;
}
