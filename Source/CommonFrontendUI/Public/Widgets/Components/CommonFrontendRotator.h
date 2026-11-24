#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "CommonFrontendRotator.generated.h"

/**
 * A frontend-specific UI rotator widget derived from UCommonRotator.
 * 
 * This class provides extended functionality for setting the selected option using display text.
 * Intended for use in settings menus or other rotator-based UI elements within the Common Frontend UI framework.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class COMMONFRONTENDUI_API UCommonFrontendRotator : public UCommonRotator
{
	GENERATED_BODY()

public:

	/**
	 * Sets the currently selected option by matching display text.
	 * 
	 * @param InTextOption The display text of the option to select.
	 */
	UFUNCTION(BlueprintCallable, Category = "Frontend Rotator")
	void SetSelectedOptionByText(const FText& InTextOption);
};
