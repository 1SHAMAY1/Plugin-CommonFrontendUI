#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonFrontendButtonBase.generated.h"

class UCommonTextBlock;
class UCommonLazyImage;

/**
 * Base button class used in the Common Frontend UI system.
 * 
 * Extends UCommonButtonBase to provide optional text and image support via bound widgets.
 * Also supports customization of the displayed text, description, and casing behavior.
 */
UCLASS()
class COMMONFRONTENDUI_API UCommonFrontendButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()
		
public:

	/**
	 * Sets the main display text shown on the button.
	 * 
	 * @param InText The new text to display.
	 */
	UFUNCTION(BlueprintCallable, Category = "CommonFrontendButton")
	void SetButtonText(FText InText);

	/**
	 * Gets the current display text of the button.
	 * 
	 * @return The current button text.
	 */
	UFUNCTION(BlueprintCallable, Category = "CommonFrontendButton")
	FText GetButtonDisplayText() const;

	/**
	 * Sets the display image on the button via a Slate brush.
	 * 
	 * @param InBrush The brush to use for the button image.
	 */
	UFUNCTION(BlueprintCallable, Category = "CommonFrontendButton")
	void SetButtionDisplayImage(const FSlateBrush& InBrush);

protected:

	/** Called before the widget is constructed. Used to initialize style and visual state. */
	virtual void NativePreConstruct() override;

	/** Called when the button's text style is updated. */
	virtual void NativeOnCurrentTextStyleChanged() override;

	/** Called when the button is hovered. */
	virtual void NativeOnHovered() override;

	/** Called when the button is unhovered. */
	virtual void NativeOnUnhovered() override;
	
	/**
	 * Called when this entry's editability changes (e.g., becomes read-only or unlocked).
	 */
	virtual void OnToggleEditableState(bool bIsEditable);
	
private:
	/** The text block widget displaying the button text (optional). */
	UPROPERTY(Category = "Frontend Button|UI", EditAnywhere, meta = (BindWidgetOptional))
	UCommonTextBlock* CommonTextBlock_ButtonText;

	/** The image widget displaying the button icon or thumbnail (optional). */
	UPROPERTY(BlueprintReadOnly, Category = "Frontend Button|UI", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UCommonLazyImage* CommonLazyImage_ButtonImage;

	/** The primary display text for the button. This can be set manually or via Blueprint. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Button|Data", meta = (AllowPrivateAccess = "true"))
	FText ButtonDisplayText;

	/** If true, the button text will be automatically converted to uppercase. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Button|Data", meta = (AllowPrivateAccess = "true"))
	bool bUserUpperCaseForButtonText = false;

	/** Optional description text for tooltip or extended UI display. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Button|Data", meta = (AllowPrivateAccess = "true"))
	FText ButtonDescriptionText;

};
