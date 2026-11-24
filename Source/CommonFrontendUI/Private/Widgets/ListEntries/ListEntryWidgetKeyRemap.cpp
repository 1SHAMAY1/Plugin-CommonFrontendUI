#include "Widgets/ListEntries/ListEntryWidgetKeyRemap.h"

#include "Widgets/Components/CommonFrontendButtonBase.h"
#include "Widgets/CommonFrontendKeyRemapScreen.h"
#include "Subsystems/CommonFrontendUI_Subsystem.h"
#include "Widgets/DataObject/ListDataObjectKeyRemap.h"
#include "CommonFrontendUI_Tags.h"

void UListEntryWidgetKeyRemap::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CommonButton_RemapKey->OnClicked().AddUObject(this,&ThisClass::OnRemapKeyButtonClicked);
	CommonButton_ResetKeyBinding->OnClicked().AddUObject(this,&ThisClass::OnResetKeyBindingButtonClicked);
}

void UListEntryWidgetKeyRemap::OnOwningListDataObjectSet(UListDataObjectBase* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	CachedOwningKeyRemapDataObject = CastChecked<UListDataObjectKeyRemap>(InOwningListDataObject);

	CommonButton_RemapKey->SetButtionDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
}

void UListEntryWidgetKeyRemap::OnOwningListDataObjectModified(UListDataObjectBase* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningKeyRemapDataObject)
	{
		// Update the button icon to match the currently bound input key
		CommonButton_RemapKey->SetButtionDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
	}
}

UWidget* UListEntryWidgetKeyRemap::NativeGetWidgetToFocusForGamepad() const
{
	if (UWidget* BP_ValidWidget = BP_GetWidgetToFocusForGamepad())
		return BP_ValidWidget;
	return CommonButton_RemapKey;
}

void UListEntryWidgetKeyRemap::OnRemapKeyButtonClicked()
{
	SelectThisEntryWidget();

	// Push a modal key capture screen asynchronously
	UCommonFrontendUI_Subsystem::Get(this)->PushSoftWidgetAsync(
		CFUI_GameplayTags::CFUI_AssociatedTag_KeyRemapScreen,
		[this](EAsyncPushActionState PushState, UCommonFrontendActivatableWidget* PushedWidget)
		{
			if (PushState == EAsyncPushActionState::OnCreated)
			{
				UCommonFrontendKeyRemapScreen* CreatedKeyRemapScreen = CastChecked<UCommonFrontendKeyRemapScreen>(PushedWidget);
				CreatedKeyRemapScreen->OnKeyRemapScreenKeyPressed.BindUObject(this,&ThisClass::OnKeyToRemapPressed);
				CreatedKeyRemapScreen->OnKeyRemapScreenKeySelectCanceled.BindUObject(this,&ThisClass::OnKeyRemapCanceled);

				if (CachedOwningKeyRemapDataObject)
				{
					CreatedKeyRemapScreen->SetDesiredInputTypeToFilter(CachedOwningKeyRemapDataObject->GetDesiredInputKeyType());
				}
			}
		}
	);
}

void UListEntryWidgetKeyRemap::OnResetKeyBindingButtonClicked()
{
	SelectThisEntryWidget();

	if (!CachedOwningKeyRemapDataObject)
	{
		return;
	}

	// Notify the user if the binding is already at its default state
	if (!CachedOwningKeyRemapDataObject->CanResetBackToDefaultValue())
	{
		TArray<FString> Buttons; Buttons.Add(TEXT("OK"));
		UCommonFrontendUI_Subsystem::Get(this)->PushChoiceScreenToModalStackAsync(
			FText::FromString(TEXT("Reset Key Mapping")),
			FText::FromString(TEXT("The key binding for ") + CachedOwningKeyRemapDataObject->GetDataDisplayName().ToString() + TEXT(" is already set to default.")),
			Buttons,
			[](const FString& Selected){}
		);

		return;
	}

	// Prompt the user for confirmation before resetting the mapping
	{
		TArray<FString> Buttons; Buttons.Add(TEXT("Yes")); Buttons.Add(TEXT("No"));
		UCommonFrontendUI_Subsystem::Get(this)->PushChoiceScreenToModalStackAsync(
			FText::FromString(TEXT("Reset Key Mapping")),
			FText::FromString(TEXT("Are you sure you want to reset the key binding for ") + CachedOwningKeyRemapDataObject->GetDataDisplayName().ToString() + TEXT(" ?")),
			Buttons,
			[this](const FString& Selected)
			{
				if (Selected.Equals(TEXT("Yes"), ESearchCase::IgnoreCase))
				{
					CachedOwningKeyRemapDataObject->TryResetBackToDefaultValue();
				}
			}
		);
	}
}

void UListEntryWidgetKeyRemap::OnKeyToRemapPressed(const FKey& PressedKey)
{
	if (CachedOwningKeyRemapDataObject)
	{
		CachedOwningKeyRemapDataObject->BindNewInputKey(PressedKey);
	}
}

void UListEntryWidgetKeyRemap::OnKeyRemapCanceled(const FString& CanceledReason)
{
}
