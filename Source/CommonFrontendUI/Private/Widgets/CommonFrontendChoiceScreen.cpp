#include "Widgets/CommonFrontendChoiceScreen.h"

#include "CommonFrontendLibrary.h"
#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"


void UCommonFrontendChoiceScreen::InitChoiceScreen(UChoiceScreenInfoObject* InScreenInfoObject,
	TFunction<void(const FString&)> ClickedButtonCallback)
{
	if (!InScreenInfoObject || !CommonTextBlock_Title || !CommonTextBlock_Message || !DynamicEntryBox_Buttons)
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Error, TEXT("InitChoiceScreen: Invalid inputs or unbound widgets"));
		return;
	}

	CommonTextBlock_Title->SetText(InScreenInfoObject->ScreenTitle);
	CommonTextBlock_Message->SetText(InScreenInfoObject->ScreenMessage);

	// Clear previous buttons
	if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
	{
		DynamicEntryBox_Buttons->Reset<UCommonFrontendButtonBase>(
			[](UCommonFrontendButtonBase& ExistingButton)
			{
				ExistingButton.OnClicked().Clear();
			}
		);
	}

	TArray<FString> Labels = InScreenInfoObject->ButtonLabels;
	if (Labels.IsEmpty())
	{
		Labels.Add(TEXT("Back"));
	}

	for (const FString& Label : Labels)
	{
		UCommonFrontendButtonBase* AddedButton = DynamicEntryBox_Buttons->CreateEntry<UCommonFrontendButtonBase>();
		if (!AddedButton)
		{
			continue;
		}
		AddedButton->SetButtonText(FText::FromString(Label));
		AddedButton->OnClicked().AddLambda(
			[ClickedButtonCallback, Label, this]()
			{
				if (ClickedButtonCallback)
				{
					ClickedButtonCallback(Label);
				}
				DeactivateWidget();
			}
		);
	}
}

UWidget* UCommonFrontendChoiceScreen::NativeGetDesiredFocusTarget() const
{
	if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
	{
		DynamicEntryBox_Buttons->GetAllEntries().Last()->SetFocus();
	}
	return Super::NativeGetDesiredFocusTarget();
}