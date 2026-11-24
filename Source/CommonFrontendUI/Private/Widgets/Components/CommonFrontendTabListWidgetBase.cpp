#include "Widgets/Components/CommonFrontendTabListWidgetBase.h"

#include "CommonButtonGroupBase.h"
#include "Widgets/Components/CommonFrontendButtonBase.h"
#if WITH_EDITOR
#include "Editor\WidgetCompilerLog.h"
# endif
void UCommonFrontendTabListWidgetBase::RequestRegisterTab(const FName& InTabID, const FText& InTabDisplayName)
{
	RegisterTab(InTabID,TabButtonEntryWidgetClass,nullptr);

	if (UCommonFrontendButtonBase* FoundButton = Cast<UCommonFrontendButtonBase>(GetTabButtonBaseByID(InTabID)))
	{
		FoundButton->SetButtonText(InTabDisplayName);
	}
}

#if WITH_EDITOR
void UCommonFrontendTabListWidgetBase::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!TabButtonEntryWidgetClass)
	{
		CompileLog.Error(FText::FromString(
			TEXT("The variable TabButtonEntryWidgetClass has no valid entry specified. ") +
			GetClass()->GetName() + 
			TEXT(" needs a valid entry widget class to function properly")
		));
	}}
#endif


void UCommonFrontendTabListWidgetBase::SelectNextTab()
{
	if (TabButtonGroup)
	{
		TabButtonGroup->SelectNextButton();
	}
}

void UCommonFrontendTabListWidgetBase::SelectPreviousTab()
{
	if (TabButtonGroup)
	{
		TabButtonGroup->SelectPreviousButton();
	}
}
