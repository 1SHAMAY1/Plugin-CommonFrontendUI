#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_RegisterFrontendUI_Config.generated.h"

class UCommonFrontendUI_DataConfig;
class UCommonPrimaryLayoutWidget;
class UCommonFrontendUI_Subsystem;

/**
 * Delegate broadcast when the frontend UI widget is successfully loaded and registered.
 * 
 * @param LoadedWidget The primary layout widget that was successfully loaded.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFrontendUIConfigLoaded, UCommonPrimaryLayoutWidget*, LoadedWidget);

/**
 * Asynchronous Blueprint node that registers and loads the frontend UI using the specified configuration.
 * 
 * This async node allows deferred, non-blocking loading of the frontend layout based on a provided
 * UCommonFrontendUI_DataConfig asset. It ensures proper registration and setup through the UI subsystem,
 * and broadcasts delegates on success or failure.
 */
UCLASS()
class COMMONFRONTENDUI_API UAsyncAction_RegisterFrontendUI_Config : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	/**
	 * Registers and loads a frontend UI using the specified config asset.
	 * 
	 * @param WorldContextObject Required context object (typically the GameInstance or World).
	 * @param Config The frontend UI data config asset to load and register.
	 * @return An instance of this async node to bind result delegates to.
	 */
	UFUNCTION(BlueprintCosmetic, BlueprintCallable, Category = "CommonFrontendUI", meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UAsyncAction_RegisterFrontendUI_Config* RegisterFrontendUI_Config(UObject* WorldContextObject, UCommonFrontendUI_DataConfig* Config);

	/** Called when the widget is successfully loaded and registered. */
	UPROPERTY(BlueprintAssignable)
	FOnFrontendUIConfigLoaded OnSuccess;

	/** Called if loading or registration fails. */
	UPROPERTY(BlueprintAssignable)
	FOnFrontendUIConfigLoaded OnFailure;

	/** Overrides the activation lifecycle of the async node. */
	virtual void Activate() override;

private:

	/** Internal reference to the world context object. */
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject;

	/** The UI configuration asset to load. */
	UPROPERTY()
	TObjectPtr<UCommonFrontendUI_DataConfig> Config;

	/**
	 * Internal callback when the loading process completes.
	 * 
	 * @param LoadedWidget The loaded primary layout widget, or nullptr if failed.
	 */
	void OnComplete_Internal(UCommonPrimaryLayoutWidget* LoadedWidget);
};
