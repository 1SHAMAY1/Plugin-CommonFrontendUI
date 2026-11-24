#pragma once

#include "CoreMinimal.h"
#include "CommonFrontendActivatableWidget.h"
#include "Widgets/Components/CommonFrontendButtonBase.h"
#include "CommonFrontendChoiceScreen.generated.h"

class UCommonTextBlock;
class UDynamicEntryBox;

/**
 * Data object describing a choice screen (string-based buttons).
 */
UCLASS()
class COMMONFRONTENDUI_API UChoiceScreenInfoObject : public UObject
{
	GENERATED_BODY()

public:
	/** Title text of the screen */
	UPROPERTY(Transient)
	FText ScreenTitle;

	/** Main message text of the screen */
	UPROPERTY(Transient)
	FText ScreenMessage;

	/** Labels for buttons to show. If empty, a single "Back" button will be created. */
	UPROPERTY(Transient)
	TArray<FString> ButtonLabels;

	/** Helper to create and populate */
	static UChoiceScreenInfoObject* Create(const FText& InTitle, const FText& InMsg, const TArray<FString>& InButtons)
	{
		UChoiceScreenInfoObject* Obj = NewObject<UChoiceScreenInfoObject>();
		Obj->ScreenTitle = InTitle;
		Obj->ScreenMessage = InMsg;
		Obj->ButtonLabels = InButtons;
		return Obj;
	}
};

/**
 * Activatable widget that displays a configurable choice screen.
 * Builds buttons from string labels and returns the selected label.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class COMMONFRONTENDUI_API UCommonFrontendChoiceScreen : public UCommonFrontendActivatableWidget
{
	GENERATED_BODY()

public:
	/** Initialize with info and callback returning selected button label */
	/**
	 * Initializes the choice screen with the provided configuration and click callback.
	 * 
	 * @param InScreenInfoObject - Configuration data for title, message, and buttons.
	 * @param ClickedButtonCallback - Callback triggered with the label of the clicked button.
	 */
	void InitChoiceScreen(UChoiceScreenInfoObject* InScreenInfoObject, TFunction<void(const FString&)> ClickedButtonCallback);

protected:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

private:
	/** Text block displaying the screen title */
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Screen Widgets")
	UCommonTextBlock* CommonTextBlock_Title;

	/** Text block displaying the screen message */
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Screen Widgets")
	UCommonTextBlock* CommonTextBlock_Message;

	/** Dynamic entry box where buttons will be added */
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Screen Widgets")
	UDynamicEntryBox* DynamicEntryBox_Buttons;
};
