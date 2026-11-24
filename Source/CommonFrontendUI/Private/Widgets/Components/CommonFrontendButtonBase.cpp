#include "Widgets/Components/CommonFrontendButtonBase.h"

#include "Subsystems/CommonFrontendUI_Subsystem.h"
#include "CommonLazyImage.h"
#include "CommonTextBlock.h"

void UCommonFrontendButtonBase::SetButtonText(FText InText)
{
	if (CommonTextBlock_ButtonText && !InText.IsEmpty())
		CommonTextBlock_ButtonText->SetText(bUserUpperCaseForButtonText? InText.ToUpper() : InText);
	
}

FText UCommonFrontendButtonBase::GetButtonDisplayText() const
{
	if (CommonTextBlock_ButtonText)
		return CommonTextBlock_ButtonText->GetText();
	return FText();
}

void UCommonFrontendButtonBase::SetButtionDisplayImage(const FSlateBrush& InBrush)
{
	if (CommonLazyImage_ButtonImage)
		CommonLazyImage_ButtonImage->SetBrush(InBrush);
	
}

void UCommonFrontendButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetButtonText(ButtonDisplayText);

}

void UCommonFrontendButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();
	
	if (CommonTextBlock_ButtonText && GetCurrentTextStyleClass())
	{
		CommonTextBlock_ButtonText->SetStyle(GetCurrentTextStyleClass());
	}
}

void UCommonFrontendButtonBase::NativeOnHovered()
{
	Super::NativeOnHovered();
	if (!ButtonDescriptionText.IsEmpty())
		UCommonFrontendUI_Subsystem::Get(this)->OnButtonDescriptionTextUpdated.Broadcast(this,ButtonDescriptionText);
	
}

void UCommonFrontendButtonBase::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();
	UCommonFrontendUI_Subsystem::Get(this)->OnButtonDescriptionTextUpdated.Broadcast(this,FText::GetEmpty());

}

void UCommonFrontendButtonBase::OnToggleEditableState(bool bIsEditable)
{
	if (CommonTextBlock_ButtonText)
	{
		CommonTextBlock_ButtonText->SetIsEnabled(bIsEditable);
	}
}