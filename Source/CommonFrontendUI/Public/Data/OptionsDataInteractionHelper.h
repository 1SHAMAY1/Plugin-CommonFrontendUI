#pragma once

#include "CoreMinimal.h"
#include "Data/CommonFrontendSettingsRegistry.h"
#include "PropertyPathHelpers.h"

class UCommonFrontendGameUserSettings;

/**
 * FOptionsDataInteractionHelper
 *
 * Lightweight helper that uses a string-based property path to dynamically
 * get or set a value in the game user settings class. Intended for advanced
 * settings UI systems where options are bound at runtime using flexible paths
 * rather than hardcoded accessors.
 *
 * Example use case: binding a UI slider to "GraphicsSettings.AntiAliasingQuality".
 */
class COMMONFRONTENDUI_API FOptionsDataInteractionHelper
{
public:
	using FGetterFunction = TFunction<FString()>;
	using FSetterFunction = TFunction<void(const FString&)>;

	/** Constructs the helper with a setter/getter property path (e.g., "bEnableVSync") */
	explicit FOptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath);

	/** Constructs the helper with direct lambda functions for getting/setting values */
	FOptionsDataInteractionHelper(FGetterFunction InGetter, FSetterFunction InSetter);

	/** Retrieves the value from the property path as a string */
	FString GetValueAsString() const;

	/** Sets the property at the path to a stringified value */
	void SetValueFromString(const FString& InStringValue);

private:
	/** Cached function name or property path (e.g., "GetFullscreenMode" or "bEnableVSync") */
	FString CachedFunctionOrPropertyPath;

	/** Cached getter lambda */
	FGetterFunction CachedGetterFunc;

	/** Cached setter lambda */
	FSetterFunction CachedSetterFunc;

	/** Cached weak pointer to game user settings object */
	TWeakObjectPtr<UCommonFrontendGameUserSettings> CachedWeakGameUserSettings;
};
