#pragma once

#include "CoreMinimal.h"
#include "CommonFrontendActivatableWidget.h"
#include "CommonInputTypeEnum.h"
#include "CommonFrontendKeyRemapScreen.generated.h"

class UCommonRichTextBlock;
class FKeyRemapScreenInputPreprocessor;

/**
 * UCommonFrontendKeyRemapScreen
 * 
 * A full-screen activatable widget used to capture and remap a new input key from the user.
 * Typically shown when the player chooses to rebind a control. This widget handles input detection,
 * cancellation, and filtering by input type (e.g., Keyboard, Gamepad).
 */
UCLASS()
class COMMONFRONTENDUI_API UCommonFrontendKeyRemapScreen : public UCommonFrontendActivatableWidget
{
	GENERATED_BODY()
		
public:
	/** 
	 * Sets the desired input type to filter (e.g., only accept gamepad or keyboard keys).
	 * This restricts valid key input to a specific device type.
	 */
	void SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType);

	/** 
	 * Delegate broadcast when a valid key is pressed and captured for remapping.
	 */
	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeyPressedDelegate, const FKey& /*PressedKey*/)
	FOnKeyRemapScreenKeyPressedDelegate OnKeyRemapScreenKeyPressed;

	/** 
	 * Delegate broadcast when key remapping is canceled (e.g., user presses ESC or times out).
	 * Provides a reason string for the cancellation.
	 */
	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeySelectCanceledDelegate, const FString& /*CanceledReason*/)
	FOnKeyRemapScreenKeySelectCanceledDelegate OnKeyRemapScreenKeySelectCanceled;

protected:
	/** Called when the widget is activated (shown on screen). Used to set up input listening. */
	virtual void NativeOnActivated() override;

	/** Called when the widget is deactivated (hidden from screen). Used to clean up input handling. */
	virtual void NativeOnDeactivated() override;

private:
	/**
	 * Called when a valid remappable key press is detected.
	 * Broadcasts OnKeyRemapScreenKeyPressed and deactivates the widget.
	 */
	void OnValidKeyPressedDetected(const FKey& PressedKey);

	/**
	 * Called when key remapping is canceled by the user or system.
	 * Broadcasts OnKeyRemapScreenKeySelectCanceled and deactivates the widget.
	 */
	void OnKeySelectCanceled(const FString& CanceledReason);

	/**
	 * Handles deactivating the widget with an optional pre-deactivation callback.
	 * Ensures cleanup occurs before the screen is dismissed.
	 */
	void RequestDeactivateWidget(TFunction<void()> PreDeactivateCallback);

	/** The rich text block that shows instructional or status text for key remapping. */
	UPROPERTY(meta = (BindWidget),EditAnywhere, Category = "KeyRemap")
	UCommonRichTextBlock* CommonRichText_RemapMessage;

	/** Cached input preprocessor used to detect raw key input while the widget is active. */
	TSharedPtr<FKeyRemapScreenInputPreprocessor> CachedInputPreprocessor;

	/** The current input type filter (e.g., keyboard or gamepad) to restrict accepted key presses. */
	ECommonInputType CachedDesiredInputType;
};
