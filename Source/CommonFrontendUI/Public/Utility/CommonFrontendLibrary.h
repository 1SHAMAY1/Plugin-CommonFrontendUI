#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommonFrontendLibrary.generated.h"

class UCommonFrontendGameUserSettings;

/**
 * Utility library for Common Frontend UI operations.
 */
UCLASS()
class COMMONFRONTENDUI_API UCommonFrontendLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Retrieves the Common Frontend Game User Settings.
	 * Returns nullptr if the engine or game user settings are unavailable.
	 */
	UFUNCTION(BlueprintPure, Category = "CommonFrontend|Settings")
	static UCommonFrontendGameUserSettings* GetCommonFrontendGameUserSettings();

	/**
	 * Checks if valid hardware benchmark results exist (CPU & GPU > 0).
	 */
	UFUNCTION(BlueprintCosmetic, BlueprintPure, Category = "CommonFrontend|Settings")
	static bool HasValidBenchmarkSettings();

	static const bool ShouldEnableDebug();
	
};
