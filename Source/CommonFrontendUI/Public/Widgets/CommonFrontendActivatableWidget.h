#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "CommonFrontendActivatableWidget.generated.h"

/**
 * UCommonFrontendActivatableWidget
 *
 * Base class for all activatable widgets in the Common Frontend UI system.
 * Extends UCommonActivatableWidget to allow frontend widgets (e.g., menus, settings screens, overlays)
 * to provide custom input configuration for the UI input subsystem.
 *
 * Designed to be subclassed for specific frontend screen implementations.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class COMMONFRONTENDUI_API UCommonFrontendActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:

	/**
	 * Override to return the desired input configuration for this widget.
	 * Allows each screen to define whether game or UI input should be active.
	 * 
	 * @return Optional input config for the UI input subsystem
	 */
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	virtual UWidget* NativeGetDesiredFocusTarget() const override;
};
