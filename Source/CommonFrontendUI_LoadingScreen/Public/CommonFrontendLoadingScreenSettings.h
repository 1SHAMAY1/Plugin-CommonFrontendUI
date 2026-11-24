#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Styling/SlateBrush.h"
#include "CommonFrontendLoadingScreenSettings.generated.h"

class UUserWidget;

UENUM(BlueprintType)
enum class ELoadingOverlayAlignment : uint8
{
	TopLeft      UMETA(DisplayName = "Top Left"),
	TopCenter    UMETA(DisplayName = "Top Center"),
	TopRight     UMETA(DisplayName = "Top Right"),
	CenterLeft   UMETA(DisplayName = "Center Left"),
	Center       UMETA(DisplayName = "Center"),
	CenterRight  UMETA(DisplayName = "Center Right"),
	BottomLeft   UMETA(DisplayName = "Bottom Left"),
	BottomCenter UMETA(DisplayName = "Bottom Center"),
	BottomRight  UMETA(DisplayName = "Bottom Right")
};

/**
 * Developer settings for configuring the general behavior of the frontend loading screen.
 *
 * Stored in config and modifiable via Project Settings.
 */
UCLASS(Config = Game, DefaultConfig, EditInlineNew)
class COMMONFRONTENDUI_LOADINGSCREEN_API UCommonFrontendLoadingScreenSettings 
	: public UDeveloperSettings
{
	GENERATED_BODY()

public:

	/**
	 * Returns the configured loading screen widget class, ensuring it is valid.
	 */
	TSubclassOf<UUserWidget> GetLoadingScreenWidgetClassChecked() const;

	/** Widget class for the loading screen */
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	TSoftClassPtr<UUserWidget> SoftLoadingScreenWidgetClass;

	/** Extra hold time after load completes */
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	float HoldLoadingScreenExtraSeconds = 3.f;

	/** Show in Standalone/PIE */
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	bool bShouldShowLoadingScreenInEditor = true;

	/** If false, the loading screen will not be shown at all */
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	bool bEnableLoadingScreen = true;

	/** If enabled, shows a Slate progress overlay on top of the loading screen */
	UPROPERTY(Config, EditAnywhere, Category = "Progress Overlay")
	bool bShowProgress = true;

	/** If true, uses the layout config below to position/size the overlay */
	UPROPERTY(Config, EditAnywhere, Category = "Progress Overlay")
	bool bOverrideLoadingProgressConfig = false;

	/** Overlay alignment */
	UPROPERTY(Config, EditAnywhere, Category = "Progress Overlay", meta = (EditCondition = "bOverrideLoadingProgressConfig"))
	ELoadingOverlayAlignment LoadingOverlayAlignment = ELoadingOverlayAlignment::BottomCenter;

	/** Size of the progress panel in pixels */
	UPROPERTY(Config, EditAnywhere, Category = "Progress Overlay", meta = (EditCondition = "bOverrideLoadingProgressConfig"))
	FVector2D LoadingProgressSize = FVector2D(500.f, 10.f);

	/** Padding for the progress panel */
	UPROPERTY(Config, EditAnywhere, Category = "Progress Overlay", meta = (EditCondition = "bOverrideLoadingProgressConfig"))
	FMargin LoadingProgressPadding = FMargin(0.f, 0.f, 0.f, 100.f);
	
};
