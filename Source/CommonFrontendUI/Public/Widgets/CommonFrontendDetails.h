#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CommonFrontendDetails.generated.h"

class UCommonRichTextBlock;
class UCommonLazyImage;
class UCommonTextBlock;
class UListDataObjectBase;

/**
 * UCommonFrontendDetails
 * 
 * A reusable UI widget that displays detailed information about a selected item
 * in the frontend settings or options interface. Designed to be shown alongside 
 * list-based collections (such as video, audio, or control settings) and dynamically
 * update its content when a new data object is selected.
 * 
 * The widget is bound to several common UI components (title, description, image, etc.)
 * and provides public functions to update or clear its display.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class COMMONFRONTENDUI_API UCommonFrontendDetails : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	/** 
	 * Updates the details panel to reflect the specified data object.
	 * @param InDataObject - The data object providing the content to display.
	 * @param InEntryWidgetClassName - Optional class name of the entry widget, for logging or extended behavior.
	 */
	void UpdateDetailsViewInfo(UListDataObjectBase* InDataObject, const FString& InEntryWidgetClassName = FString());

	/** 
	 * Clears the displayed details, effectively resetting the widget to an empty state.
	 */
	void ClearDetailsViewInfo();

protected:
	/** 
	 * Called when the widget is constructed and added to the hierarchy.
	 * Used to perform initialization logic.
	 */
	virtual void NativeOnInitialized() override;

private:
	/** The title text block for displaying the setting or item name. */
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Item Description Widgets")
	UCommonTextBlock* CommonTextBlock_Title;

	/** A lazy-loaded image that visually represents the selected item. */
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Item Description Widgets")
	UCommonLazyImage* CommonLazyImage_DescriptionImage;

	/** A rich text block used to display the item's main description. */
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Item Description Widgets")
	UCommonRichTextBlock* CommonRichText_Description;

	/** An optional rich text block for showing extra dynamic details about the item. */
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Item Description Widgets")
	UCommonRichTextBlock* CommonRichText_DynamicDetails;

	/** A rich text block for explaining why an item is disabled or locked. */
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Item Description Widgets")
	UCommonRichTextBlock* CommonRichText_DisabledReason;

};
