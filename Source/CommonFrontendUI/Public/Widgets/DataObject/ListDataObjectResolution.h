#pragma once

#include "CoreMinimal.h"
#include "Widgets/DataObject/ListDataObjectString.h"
#include "ListDataObjectResolution.generated.h"

/**
 * UListDataObjectResolution
 *
 * Represents a selectable resolution option within a UI list, typically for a video/display settings screen.
 * Inherits from UListDataObjectString to leverage string-based display and value representation.
 * 
 * Responsibilities:
 * - Initializes and manages resolution data for user selection.
 * - Converts FIntPoint resolution values to user-friendly display text and backend string representations.
 * - Tracks and exposes the maximum allowed resolution (e.g., for clamping or validation).
 * 
 * This class is intended to be data-driven and transient, used only for frontend display logic.
 */
UCLASS()
class COMMONFRONTENDUI_API UListDataObjectResolution : public UListDataObjectString
{
	GENERATED_BODY()

public:
	/** Initializes resolution data, such as the list of available screen resolutions. */
	void InitResolutionValues();

protected:
	/** Called when the data object is first initialized in the list. */
	virtual void OnDataObjectInitialized() override;

private:
	/** Converts a resolution (FIntPoint) to a compact string format used for internal value representation. */
	FString ResToValueString(const FIntPoint& InResolution) const;

	/** Converts a resolution (FIntPoint) to a localized, user-friendly text label for display in UI. */
	FText ResToDisplayText(const FIntPoint& InResolution) const;

	/** Optional override to define a maximum allowed resolution for UI validation or clamping logic. */
	FString MaximumAllowedResolution;

public:
	/** Returns the maximum allowed resolution string, if defined. */
	FORCEINLINE FString GetMaximumAllowedResolution() const { return MaximumAllowedResolution; }
};
