#pragma once

#include "CoreMinimal.h"
#include "Widgets/ListEntries/ListEntryWidgetBase.h"
#include "ListEntryWidgetKeyRemap.generated.h"

class UListDataObjectKeyRemap;
class UCommonFrontendButtonBase;

/**
 * UListEntryWidgetKeyRemap
 * 
 * This abstract list entry widget is used for key remapping functionality in the frontend UI.
 * It allows users to interact with key-binding options via dedicated UI buttons. 
 * The widget supports remapping a key, resetting it to default, and handles user input 
 * (such as key press or canceling remap).
 * 
 * This widget extends UListEntryWidgetBase and is driven by a UListDataObjectKeyRemap data object.
 * UI behavior is driven by both native C++ logic and Blueprint implementations.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class COMMONFRONTENDUI_API UListEntryWidgetKeyRemap : public UListEntryWidgetBase
{
	GENERATED_BODY()

protected:

	/** Called when the widget is constructed and initialized */
	virtual void NativeOnInitialized() override;
	
	/** Called when the associated list data object is set on this widget */
	virtual void OnOwningListDataObjectSet(UListDataObjectBase* InOwningListDataObject) override;

	/**
	 * Called when the associated list data object is modified.
	 * This handles UI updates based on keybinding data changes.
	 */
	virtual void OnOwningListDataObjectModified(UListDataObjectBase* OwningModifiedData, EOptionsListDataModifyReason ModifyReason) override;

	virtual UWidget* NativeGetWidgetToFocusForGamepad() const override;
	
private:

	/** Handles the event when the Remap Key button is clicked */
	void OnRemapKeyButtonClicked();

	/** Handles the event when the Reset Key Binding button is clicked */
	void OnResetKeyBindingButtonClicked();

	/**
	 * Called when a new key is pressed while remapping is in progress.
	 * This function updates the data object with the new key.
	 */
	void OnKeyToRemapPressed(const FKey& PressedKey);

	/**
	 * Called when the key remapping process is canceled by the user.
	 * Displays appropriate message or feedback.
	 */
	void OnKeyRemapCanceled(const FString& CanceledReason);
	
	/** Button used to initiate the key remapping process */
	UPROPERTY(BlueprintReadOnly, Category = "CommonFrontendUI|KeyRemap", meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonFrontendButtonBase* CommonButton_RemapKey;

	/** Button used to reset the current key binding to its default state */
	UPROPERTY(BlueprintReadOnly, Category = "CommonFrontendUI|KeyRemap", meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonFrontendButtonBase* CommonButton_ResetKeyBinding;


	/** Cached reference to the owning key remap data object for efficient access and updates */
	UPROPERTY(Transient)
	UListDataObjectKeyRemap* CachedOwningKeyRemapDataObject;
};
