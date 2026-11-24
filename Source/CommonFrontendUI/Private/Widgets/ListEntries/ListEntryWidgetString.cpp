#include "Widgets/ListEntries/ListEntryWidgetString.h"

#include "Widgets/Components/CommonFrontendButtonBase.h"
#include "Widgets/Components/CommonFrontendRotator.h"
#include "CommonInputSubsystem.h"
#include "Widgets/DAtaObject/ListDataObjectString.h"

void UListEntryWidgetString::NativeOnInitialized()
{

	Super::NativeOnInitialized();

	CommonButton_PreviousOption->OnClicked().AddUObject(this,&ThisClass::OnPreviousOptionButtonClicked);
	CommonButton_NextOption->OnClicked().AddUObject(this,&ThisClass::OnNextOptionButtonClicked);

	CommonRotator_AvailableOptions->OnClicked().AddLambda([this](){SelectThisEntryWidget();});
	CommonRotator_AvailableOptions->OnRotatedEvent.AddUObject(this,&ThisClass::OnRotatorValueChanged);}

void UListEntryWidgetString::OnOwningListDataObjectSet(UListDataObjectBase* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	CachedOwningStringDataObject = CastChecked<UListDataObjectString>(InOwningListDataObject);

	CommonRotator_AvailableOptions->PopulateTextLabels(CachedOwningStringDataObject->GetAvailableOptionsTextArray());
	CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
}

void UListEntryWidgetString::OnOwningListDataObjectModified(UListDataObjectBase* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningStringDataObject)
	{
		CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
	}
}

void UListEntryWidgetString::OnToggleEditableState(bool bIsEditable)
{

	Super::OnToggleEditableState(bIsEditable);

	CommonButton_PreviousOption->SetIsEnabled(bIsEditable);
	CommonRotator_AvailableOptions->SetIsEnabled(bIsEditable);
	CommonButton_NextOption->SetIsEnabled(bIsEditable);
}

UWidget* UListEntryWidgetString::NativeGetWidgetToFocusForGamepad() const
{
	if (UWidget* BP_ValidWidget = BP_GetWidgetToFocusForGamepad())
		return BP_ValidWidget;
	return CommonRotator_AvailableOptions;
}

void UListEntryWidgetString::OnPreviousOptionButtonClicked()
{
	if (CachedOwningStringDataObject)
	{
		CachedOwningStringDataObject->BackToPreviousOption();
	}

	SelectThisEntryWidget();
}

void UListEntryWidgetString::OnNextOptionButtonClicked()
{
	if (CachedOwningStringDataObject)
	{
		CachedOwningStringDataObject->AdvanceToNextOption();
	}

	SelectThisEntryWidget();
}

void UListEntryWidgetString::OnRotatorValueChanged(int32 Value, bool bUserInitiated)
{
	if (!CachedOwningStringDataObject)
	{
		return;
	}

	UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();

	if (!CommonInputSubsystem || !bUserInitiated)
	{
		return;
	}

	if (CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		CachedOwningStringDataObject->OnRotatorInitiatedValueChange(CommonRotator_AvailableOptions->GetSelectedText());
	}
}
