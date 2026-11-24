#include "Widgets/DataObject/ListDataObjectBase.h"

#include "Utility/DeveloperSettings/CommonFrontendDeveloperSettings.h"
#include "Utility/CommonFrontendLibrary.h"
#include "Utility/CommonFrontendGameUserSettings.h"

void UListDataObjectBase::InitDataObject()
{
	OnDataObjectInitialized();
}

void UListDataObjectBase::AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition)
{
	EditConditionDescArray.Add(InEditCondition);
}

void UListDataObjectBase::AddEditDependencyData(UListDataObjectBase* InDependencyData)
{
	if (!InDependencyData->OnListDataModified.IsBoundToObject(this))
		InDependencyData->OnListDataModified.AddUObject(this,&ThisClass::OnEditDependencyDataModified);
	
}

bool UListDataObjectBase::IsDataCurrentlyEditable()
{
	bool bIsEditable = true;

	if (EditConditionDescArray.IsEmpty())
	{
		return bIsEditable;
	}

	FString CachedDisabledRichReason;

	for (const FOptionsDataEditConditionDescriptor& Condition : EditConditionDescArray)
	{
		if (!Condition.IsValid() || Condition.IsEditConditionMet())
		{
			continue;
		}

		bIsEditable = false;

		CachedDisabledRichReason.Append(Condition.GetDisabledRichReason());
		
		SetDisabledRichText(FText::FromString(CachedDisabledRichReason));

		if (Condition.HasForcedStringValue())
		{
			const FString ForcedStringValue = Condition.GetDisabledForcedStringValue();

			if (CanSetToForcedStringValue(ForcedStringValue))
			{
				OnSetToForcedStringValue(ForcedStringValue);
			}
		}
	}

	return bIsEditable;
}

void UListDataObjectBase::CleanUp()
{
}

void UListDataObjectBase::OnDataObjectInitialized()
{
}

void UListDataObjectBase::NotifyListDataModified(UListDataObjectBase* ModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	if (UCommonFrontendLibrary::ShouldEnableDebug())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ListDataObjectBase] NotifyListDataModified: ID=%s, Reason=%d, Immediate=%d"), 
			ModifiedData ? *ModifiedData->GetDataID().ToString() : TEXT("Null"), 
			(int32)ModifyReason, 
			bShouldApplyChangeImmediatly);
	}

	OnListDataModified.Broadcast(ModifiedData,ModifyReason);

	if (bShouldApplyChangeImmediatly)
	{
		UCommonFrontendLibrary::GetCommonFrontendGameUserSettings()->ApplySettings(true);
	}
}

void UListDataObjectBase::OnEditDependencyDataModified(UListDataObjectBase* ModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	OnDependencyDataModified.Broadcast(ModifiedDependencyData,ModifyReason);        
}


