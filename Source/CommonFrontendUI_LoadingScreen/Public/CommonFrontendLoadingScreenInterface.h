#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CommonFrontendLoadingScreenInterface.generated.h"

/**
 * Interface for objects that need to respond to loading screen visibility changes.
 *
 * Classes implementing this interface can react to the start and end of loading screens,
 * allowing you to pause gameplay logic, disable input, or trigger UI animations
 * when the loading screen appears or disappears.
 *
 * This is typically implemented by:
 * - Game modes
 * - Player controllers
 * - Pawns
 * - UI widgets
 */
UINTERFACE()
class UCommonFrontendLoadingScreenInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface methods for receiving loading screen lifecycle events.
 */
class COMMONFRONTENDUI_LOADINGSCREEN_API ICommonFrontendLoadingScreenInterface
{
	GENERATED_BODY()

public:

	/**
	 * Called when the loading screen becomes visible.
	 * 
	 * Use this to:
	 * - Pause gameplay logic
	 * - Disable input
	 * - Show additional UI elements
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Loading Screen")
	void OnLoadingScreenStarted();

	/**
	 * Called when the loading screen is removed from view.
	 * 
	 * Use this to:
	 * - Resume gameplay logic
	 * - Re-enable input
	 * - Hide or transition out related UI
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Loading Screen")
	void OnLoadingScreenEnded();
};
