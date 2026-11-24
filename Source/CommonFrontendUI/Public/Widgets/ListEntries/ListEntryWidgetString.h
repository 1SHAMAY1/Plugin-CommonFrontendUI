#pragma once

#include "CoreMinimal.h"
#include "Widgets/ListEntries/ListEntryWidgetBase.h"
#include "ListEntryWidgetString.generated.h"

class UCommonFrontendRotator;
class UListDataObjectString;
class UCommonFrontendButtonBase;

/**
 * UListEntryWidgetString
 *
 * A UI list entry widget representing a string-based setting (e.g., resolution, language).
 * This widget allows cycling through a list of string options via left/right buttons
 * or using a rotator-style display.
 *
 * It binds to a UListDataObjectString instance to reflect available options and update changes.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class COMMONFRONTENDUI_API UListEntryWidgetString : public UListEntryWidgetBase
{
	GENERATED_BODY()

protected:
	/** Initializes widget, binds button and rotator callbacks */
	virtual void NativeOnInitialized() override;

	/** Called when the owning data object is set for this entry */
	virtual void OnOwningListDataObjectSet(UListDataObjectBase* InOwningListDataObject) override;

	/** Called when the owning data object is updated externally */
	virtual void OnOwningListDataObjectModified(UListDataObjectBase* OwningModifiedData, EOptionsListDataModifyReason ModifyReason) override;

	/** Called when the editable state of the widget changes (e.g., focus/highlight) */
	virtual void OnToggleEditableState(bool bIsEditable) override;

	virtual UWidget* NativeGetWidgetToFocusForGamepad() const override;

private:
	/** Called when the "Previous" button is clicked to go to the previous option */
	void OnPreviousOptionButtonClicked();

	/** Called when the "Next" button is clicked to go to the next option */
	void OnNextOptionButtonClicked();

	/**
	 * Called when the rotator value is changed (by user or programmatically)
	 * @param Value - Index of the new selected string option
	 * @param bUserInitiated - Whether the change was user-initiated or not
	 */
	void OnRotatorValueChanged(int32 Value, bool bUserInitiated);

	/** UI: Button to cycle to the previous string option */
	UPROPERTY(BlueprintReadOnly, Category = "String Option Selector|UI", meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonFrontendButtonBase* CommonButton_PreviousOption;

	/** UI: Rotator widget showing available string options */
	UPROPERTY(BlueprintReadOnly, Category = "String Option Selector|UI", meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonFrontendRotator* CommonRotator_AvailableOptions;

	/** UI: Button to cycle to the next string option */
	UPROPERTY(BlueprintReadOnly, Category = "String Option Selector|UI", meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonFrontendButtonBase* CommonButton_NextOption;

	/** Cached reference to the string list data object backing this widget */
	UPROPERTY(Transient)
	UListDataObjectString* CachedOwningStringDataObject;

};
