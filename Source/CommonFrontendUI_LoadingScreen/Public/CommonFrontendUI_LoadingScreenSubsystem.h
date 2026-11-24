#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Engine/EngineTypes.h"
#include "CommonFrontendUI_LoadingScreenSubsystem.generated.h"

class UUserWidget;

/**
 * Game instance subsystem responsible for managing and displaying the frontend loading screen.
 *
 * This subsystem tracks map loading states, triggers the loading screen when necessary,
 * and ensures it remains visible for the appropriate duration. It also exposes events 
 * for Blueprints to react to loading screen state changes.
 *
 * Responsibilities:
 * - Listen for map load/unload events.
 * - Determine whether a loading screen should be displayed.
 * - Create, cache, and remove loading screen widgets.
 * - Provide tick-based updates to handle timing and visibility logic.
 */
UCLASS()
class COMMONFRONTENDUI_LOADINGSCREEN_API UCommonFrontendUI_LoadingScreenSubsystem 
	: public UGameInstanceSubsystem
	, public FTickableGameObject
{
	GENERATED_BODY()

public:

	/** 
	 * Delegate broadcast whenever the loading reason changes. 
	 * Provides the current loading reason as a string for UI or logging purposes.
	 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingReasonUpdatedDelegate, const FString&, CurrentLoadingReason);

	/** Event triggered when the current loading reason changes. */
	UPROPERTY(BlueprintAssignable, Category = "Loading Screen")
	FOnLoadingReasonUpdatedDelegate OnLoadingReasonUpdated;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

		/**
	 * @brief Returns the current loading reason message.
	 * @return FString The current loading reason.
	 */
	UFUNCTION(BlueprintPure, Category = "Loading Screen")
	FString GetCurrentLoadingReason() const;

	/**
	 * @brief Returns the current loading progress.
	 * @return float The current loading progress, ranging from 0.0 to 1.0.
	 */
	UFUNCTION(BlueprintPure, Category = "Loading Screen")
	float GetCurrentProgress() const;

	/**
	 * @brief Opens a new level after displaying a loading screen, with an optional delay.
	 * This function first activates the loading screen, waits for the specified delay, and then initiates level travel.
	 * It's a high-level wrapper around OpenLevel for seamless transitions.
	 *
	 * @param Level The soft object pointer to the level to open.
	 * @param bAbsolute If true, the travel is absolute.
	 * @param Options Additional options for level travel.
	 * @param Delay The delay in seconds before the level transition begins. A short delay allows the loading screen to render before the game thread is blocked by loading.
	 */
	UFUNCTION(BlueprintCallable, Category = "Loading Screen|Travel", meta = (AutoCreateRefTerm = "Options", WorldContext = "WorldContextObject"))
	void OpenLevelWithLoadingScreen(UObject* WorldContextObject, TSoftObjectPtr<UWorld> Level, bool bAbsolute, FString Options, float Delay = 0.2f);


	/**
	 * @brief Initiates server travel to a new map, taking all connected clients along.
	 * This function is for server-side execution. It shows the loading screen for the local player on the server (if any)
	 * and then initiates the server travel.
	 *
	 * @param MapName The name of the map to travel to. Can be an absolute path or a short name.
	 * @param bAbsolute If true, the travel is absolute.
	 * @param Options Additional options to append to the URL.
	 * @param Delay The delay in seconds before the travel begins. A short delay allows the loading screen to render.
	 */
	UFUNCTION(BlueprintCallable, Category = "Loading Screen|Travel", meta = (AutoCreateRefTerm = "Options"))
	void ServerTravelWithLoadingScreen(const FString& MapName, bool bAbsolute, FString Options, float Delay = 0.2f);


private:

	/** Called before a map begins loading. */
	void OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName);

	/** Called after a map has finished loading. */
	void OnMapPostLoaded(UWorld* LoadedWorld);

	/** Attempts to update the loading screen's visibility and content. */
	void TryUpdateLoadingScreen();

	/** Returns true if a pre-load screen is currently active. */
	bool IsPreLoadScreenActive() const;

	/** Returns true if the system determines a loading screen should be shown. */
	bool ShouldShowLoadingScreen();

	/** Performs a conditional check to decide if the loading screen needs to appear. */
	bool CheckTheNeedToShowLoadingScreen();

	/** Performs a conditional check to decide if the shaders are still compiling. */
	bool AreShadersStillCompiling() const;

	/** Displays a loading screen if none is currently active. */
	void TryDisplayLoadingScreenIfNone();

	/** Removes the loading screen if it is currently displayed. */
	void TryRemoveLoadingScreen();

	/** Notifies listeners about a change in loading screen visibility. */
	void NotifyLoadingScreenVisibilityChanged(bool bIsVisible);

	/** Whether the system is currently loading a map. */
	bool bIsCurrentlyLoadingMap = false;

	/** Tracks the start time of the loading screen to enforce a minimum display duration. */
	float HoldLoadingScreenStartUpTime = -1.f;

#if WITH_EDITOR
	/** Current reason for displaying the loading screen (Editor only). */
	FString CurrentLoadingReason;
#endif

	/** Cached Slate widget instance for the active loading screen. */
	TSharedPtr<SWidget> CachedCreatedLoadingScreenWidget;

	/** Sets the tickable object’s tick type at runtime. */
	void SetTickableTickType(ETickableTickType NewTickType);
	
	/** Current tick type for this subsystem. */
	ETickableTickType CurrentTickType = ETickableTickType::Conditional;
	
	float CurrentProgress = 0.f;
	int32 InitialShaderCount = -1;
	int32 LastKnownShaderCount = -1;

};
