#include "CommonFrontendUI_LoadingScreenSubsystem.h"

#include "CommonFrontendLoadingScreenInterface.h"
#include "CommonFrontendLoadingScreenSettings.h"
#include "PreLoadScreenManager.h"
#include "ShaderPipelineCache.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/Pawn.h"
#include "Tickable.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Engine/EngineTypes.h"


bool UCommonFrontendUI_LoadingScreenSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{	
		TArray<UClass*> FoundClasses;
		GetDerivedClasses(GetClass(),FoundClasses);

		return FoundClasses.IsEmpty();
	}

	return false;
}

void UCommonFrontendUI_LoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this,&ThisClass::OnMapPreLoaded);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this,&ThisClass::OnMapPostLoaded);
}

void UCommonFrontendUI_LoadingScreenSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	TryRemoveLoadingScreen();
}

UWorld* UCommonFrontendUI_LoadingScreenSubsystem::GetTickableGameObjectWorld() const
{
	if (UGameInstance* OwningGameInstance = GetGameInstance())
	{
		return OwningGameInstance->GetWorld();
	}

	return nullptr;
}

void UCommonFrontendUI_LoadingScreenSubsystem::Tick(float DeltaTime)
{
	const UCommonFrontendLoadingScreenSettings* Settings = GetDefault<UCommonFrontendLoadingScreenSettings>();

	TryUpdateLoadingScreen();
}

ETickableTickType UCommonFrontendUI_LoadingScreenSubsystem::GetTickableTickType() const
{
	if (IsTemplate())
	{
		return ETickableTickType::Never;
	}
	return CurrentTickType;
	
}

bool UCommonFrontendUI_LoadingScreenSubsystem::IsTickable() const
{
	if (!GetGameInstance() || !GetGameInstance()->GetGameViewportClient())
		return false;
	return CurrentTickType != ETickableTickType::Never;

}

TStatId UCommonFrontendUI_LoadingScreenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFrontendLoadingScreenSubsystem,STATGROUP_Tickables);
}

void UCommonFrontendUI_LoadingScreenSubsystem::OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance != GetGameInstance())
	{
		return;
	}

	SetTickableTickType(ETickableTickType::Conditional);
	
	bIsCurrentlyLoadingMap = true;

	TryUpdateLoadingScreen();
}

void UCommonFrontendUI_LoadingScreenSubsystem::OnMapPostLoaded(UWorld* LoadedWorld)
{
	if (LoadedWorld && LoadedWorld->GetGameInstance() == GetGameInstance())
	{
		bIsCurrentlyLoadingMap = false;
	}
}

void UCommonFrontendUI_LoadingScreenSubsystem::TryUpdateLoadingScreen()
{
	if (IsPreLoadScreenActive())
	{
		return;
	}

	if (ShouldShowLoadingScreen())
	{
		TryDisplayLoadingScreenIfNone();
#if WITH_EDITOR
		OnLoadingReasonUpdated.Broadcast(CurrentLoadingReason);
#endif

	}
	else
	{
		TryRemoveLoadingScreen();

		HoldLoadingScreenStartUpTime = -1.f;

		NotifyLoadingScreenVisibilityChanged(false);

		SetTickableTickType(ETickableTickType::Never);
	}
}

bool UCommonFrontendUI_LoadingScreenSubsystem::IsPreLoadScreenActive() const
{
	if (FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get())
	{
		return PreLoadScreenManager->HasValidActivePreLoadScreen();
	}

	return false;
}

bool UCommonFrontendUI_LoadingScreenSubsystem::ShouldShowLoadingScreen()
{
	const UCommonFrontendLoadingScreenSettings* LoadingScreenSettings = GetDefault<UCommonFrontendLoadingScreenSettings>();

	// Don't show in editor if disabled
	if (GIsEditor && !LoadingScreenSettings->bShouldShowLoadingScreenInEditor)
	{
		return false;
	}

	if (!LoadingScreenSettings->bEnableLoadingScreen)
	{
		return false;
	}

	// Primary checks
	if (CheckTheNeedToShowLoadingScreen())
	{
		GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = true;
		return true;
	}

#if WITH_EDITOR
	CurrentLoadingReason = TEXT("Waiting for Texture Streaming");
#endif

	GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = false;

	const double CurrentTime = FPlatformTime::Seconds();

	if (HoldLoadingScreenStartUpTime < 0.f)
	{
		HoldLoadingScreenStartUpTime = CurrentTime;
	}

	// Time-based fallback only when shaders are done
	if (!AreShadersStillCompiling())
	{
		const double ElapsedTime = CurrentTime - HoldLoadingScreenStartUpTime;
		const double RemainingTime = FMath::Max(
			LoadingScreenSettings->HoldLoadingScreenExtraSeconds - ElapsedTime,
			0.0
		);

		// Progress based on elapsed hold time
		if (LoadingScreenSettings->HoldLoadingScreenExtraSeconds > 0.f)
		{
			float TimeProgress = FMath::Clamp(
				static_cast<float>(ElapsedTime / LoadingScreenSettings->HoldLoadingScreenExtraSeconds),
				0.f,
				1.f
			);
			CurrentProgress = TimeProgress;
#if WITH_EDITOR
			CurrentLoadingReason = FString::Printf(
				TEXT("Waiting for startup time (%.1f sec left)"),
				static_cast<float>(RemainingTime)
			);
#endif
		}
		else
		{
			CurrentProgress = 1.f;
		}

		// Keep showing until hold time is reached
		if (ElapsedTime < LoadingScreenSettings->HoldLoadingScreenExtraSeconds)
		{
			return true;
		}
	}

	// Shaders done + hold time passed
	CurrentProgress = 1.f;
	return false;
}


bool UCommonFrontendUI_LoadingScreenSubsystem::CheckTheNeedToShowLoadingScreen()
{
	if (bIsCurrentlyLoadingMap)
	{
#if WITH_EDITOR
		CurrentLoadingReason = TEXT("Loading level...");
#endif
		CurrentProgress = 0.f;
		return true;
	}

	UWorld* OwningWorld = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;

	if (!OwningWorld)
	{
#if WITH_EDITOR
		CurrentLoadingReason = TEXT("Initializing world...");
#endif
		CurrentProgress = 0.f;
		return true;
	}

	if (!OwningWorld->HasBegunPlay())
	{
#if WITH_EDITOR
		CurrentLoadingReason = TEXT("World not started yet...");
#endif
		CurrentProgress = 0.f;
		return true;
	}

	if (!OwningWorld->GetFirstPlayerController())
	{
#if WITH_EDITOR
		CurrentLoadingReason = TEXT("Waiting for PlayerController...");
#endif
		CurrentProgress = 0.f;
		return true;
	}

	// Shader compiling case
	if (AreShadersStillCompiling())
	{
		int32 RemainingShaders = FShaderPipelineCache::NumPrecompilesRemaining();

		// Store initial shader count once
		if (InitialShaderCount < 0)
		{
			InitialShaderCount = FMath::Max(RemainingShaders, 1); // Avoid zero total
		}

		// Calculate progress
		CurrentProgress = 1.f - (float)RemainingShaders / (float)InitialShaderCount;
		CurrentProgress = FMath::Clamp(CurrentProgress, 0.f, 1.f);

#if WITH_EDITOR
		CurrentLoadingReason = FString::Printf(TEXT("Compiling shaders: %d remaining"), RemainingShaders);
#endif
		return true;
	}

	// Everything ready
	CurrentProgress = 1.f;
	return false;
}

bool UCommonFrontendUI_LoadingScreenSubsystem::AreShadersStillCompiling() const
{
	return FShaderPipelineCache::NumPrecompilesRemaining() > 0;
}

void UCommonFrontendUI_LoadingScreenSubsystem::TryDisplayLoadingScreenIfNone()
{
	if (CachedCreatedLoadingScreenWidget)
	{
		return;
	}

	const UCommonFrontendLoadingScreenSettings* LoadingScreenSettings = GetDefault<UCommonFrontendLoadingScreenSettings>();

	TSubclassOf<UUserWidget> LoadedWidgetClass = LoadingScreenSettings->GetLoadingScreenWidgetClassChecked();

	UUserWidget* CreatedWidget = UUserWidget::CreateWidgetInstance(*GetGameInstance(), LoadedWidgetClass, NAME_None);
	if (!CreatedWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("LoadingScreen: Failed to create loading widget"));
		return;
	}

	TSharedRef<SWidget> LoadingContent = CreatedWidget->TakeWidget();

	TSharedPtr<SWidget> FinalOverlayWidget;

	if (LoadingScreenSettings->bShowProgress)
	{
		// Build a Slate overlay that wraps the loading content and shows a progress bar
		ELoadingOverlayAlignment Align = LoadingScreenSettings->LoadingOverlayAlignment;
		FVector2D Size = LoadingScreenSettings->LoadingProgressSize;
		FMargin Pad = LoadingScreenSettings->LoadingProgressPadding;

		EHorizontalAlignment HAlign = HAlign_Center;
		EVerticalAlignment VAlign = VAlign_Bottom;
		switch (Align)
		{
		case ELoadingOverlayAlignment::TopLeft:      HAlign = HAlign_Left;   VAlign = VAlign_Top;    break;
		case ELoadingOverlayAlignment::TopCenter:    HAlign = HAlign_Center; VAlign = VAlign_Top;    break;
		case ELoadingOverlayAlignment::TopRight:     HAlign = HAlign_Right;  VAlign = VAlign_Top;    break;
		case ELoadingOverlayAlignment::CenterLeft:   HAlign = HAlign_Left;   VAlign = VAlign_Center; break;
		case ELoadingOverlayAlignment::Center:       HAlign = HAlign_Center; VAlign = VAlign_Center; break;
		case ELoadingOverlayAlignment::CenterRight:  HAlign = HAlign_Right;  VAlign = VAlign_Center; break;
		case ELoadingOverlayAlignment::BottomLeft:   HAlign = HAlign_Left;   VAlign = VAlign_Bottom; break;
		case ELoadingOverlayAlignment::BottomCenter: HAlign = HAlign_Center; VAlign = VAlign_Bottom; break;
		case ELoadingOverlayAlignment::BottomRight:  HAlign = HAlign_Right;  VAlign = VAlign_Bottom; break;
		default: break;
		}

		TSharedRef<SOverlay> Overlay = SNew(SOverlay)
		+ SOverlay::Slot()
		[
			LoadingContent
		]
		+ SOverlay::Slot()
		.HAlign(HAlign)
		.VAlign(VAlign)
		.Padding(Pad)
		[
			SNew(SBox)
			.WidthOverride(Size.X)
			.HeightOverride(Size.Y)
			[
				SNew(SVerticalBox)

				// Progress bar
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SProgressBar)
					.Percent_Lambda([this]() -> TOptional<float>
					{
						return CurrentProgress;
					})
					.FillColorAndOpacity(FLinearColor(0.f, 0.5f, 1.f, 1.f)) // Blue fill
					.BackgroundImage(FCoreStyle::Get().GetBrush("NoBrush"))  // Transparent background
				]

				// Progress text outside the bar
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				.Padding(FMargin(0.f, 4.f, 0.f, 0.f))
				[
					SNew(STextBlock)
					.Text_Lambda([this]()
					{
						return FText::Format(NSLOCTEXT("LoadingScreen", "ProgressFormat", "{0}%"),
							FMath::RoundToInt(CurrentProgress * 100.0f));
					})
					.ColorAndOpacity(FLinearColor::White)
				]

#if WITH_EDITOR
				// Optional: show debug reason
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				.Padding(FMargin(0.f, 2.f, 0.f, 0.f))
				[
					SNew(STextBlock)
					.Text_Lambda([this]()
					{
						return FText::FromString(CurrentLoadingReason);
					})
					.ColorAndOpacity(FLinearColor::White)
				]
#endif
			]
		];

		FinalOverlayWidget = Overlay;
	}
	else
	{
		FinalOverlayWidget = LoadingContent;
	}

	CachedCreatedLoadingScreenWidget = FinalOverlayWidget;

	GetGameInstance()->GetGameViewportClient()->AddViewportWidgetContent(
		CachedCreatedLoadingScreenWidget.ToSharedRef(),
		1000
	);

	NotifyLoadingScreenVisibilityChanged(true);
}

void UCommonFrontendUI_LoadingScreenSubsystem::TryRemoveLoadingScreen()
{
	if (!CachedCreatedLoadingScreenWidget)
	{
		return;
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient())
		{
			GameViewportClient->RemoveViewportWidgetContent(CachedCreatedLoadingScreenWidget.ToSharedRef());
		}
	}

	CachedCreatedLoadingScreenWidget.Reset();
}

void UCommonFrontendUI_LoadingScreenSubsystem::NotifyLoadingScreenVisibilityChanged(bool bIsVisible)
{
	for (ULocalPlayer* ExistingLocalPlayer : GetGameInstance()->GetLocalPlayers())
	{
		if (!ExistingLocalPlayer)
		{
			continue;
		}

		if (APlayerController* PC = ExistingLocalPlayer->GetPlayerController(GetGameInstance()->GetWorld()))
		{
			if (PC->Implements<UCommonFrontendLoadingScreenInterface>())
			{	
				if (bIsVisible)
				{
					ICommonFrontendLoadingScreenInterface::Execute_OnLoadingScreenStarted(PC);
				}
				else
				{
					ICommonFrontendLoadingScreenInterface::Execute_OnLoadingScreenEnded(PC);
				}
				
			}

			if(APawn* OwningPawn = PC->GetPawn())
			{
				if (OwningPawn->Implements<UCommonFrontendLoadingScreenInterface>())
				{
					if (bIsVisible)
					{
						ICommonFrontendLoadingScreenInterface::Execute_OnLoadingScreenStarted(OwningPawn);
					}
					else
					{
						ICommonFrontendLoadingScreenInterface::Execute_OnLoadingScreenEnded(OwningPawn);
					}
				}
			}
		}

	}
}

void UCommonFrontendUI_LoadingScreenSubsystem::SetTickableTickType(ETickableTickType NewTickType)
{
	CurrentTickType = NewTickType;
}

FString UCommonFrontendUI_LoadingScreenSubsystem::GetCurrentLoadingReason() const
{
#if WITH_EDITOR
	return CurrentLoadingReason;
#else
	return FString();
#endif
}

float UCommonFrontendUI_LoadingScreenSubsystem::GetCurrentProgress() const
{
	return CurrentProgress;
}

void UCommonFrontendUI_LoadingScreenSubsystem::OpenLevelWithLoadingScreen(UObject* WorldContextObject, TSoftObjectPtr<UWorld> Level, bool bAbsolute, FString Options, float Delay)
{
	if (Level.IsNull())
	{
		return;
	}

	// Show loading screen
	bIsCurrentlyLoadingMap = true;
	TryUpdateLoadingScreen();

	// Use a timer to delay the level opening, allowing the loading screen to render first.
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([WorldContextObject, Level, bAbsolute, Options]()
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(WorldContextObject, Level, bAbsolute, Options);
	});

	FTimerHandle UnusedHandle;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(UnusedHandle, TimerDelegate, FMath::Max(Delay, 0.001f), false);
	}
}

void UCommonFrontendUI_LoadingScreenSubsystem::ServerTravelWithLoadingScreen(const FString& MapName, bool bAbsolute, FString Options, float Delay)
{
	UWorld* World = GetWorld();
	if (!World || !World->GetAuthGameMode())
	{
		return; // Not running on the server
	}

	// Show loading screen for server's local player (if any)
	bIsCurrentlyLoadingMap = true;
	TryUpdateLoadingScreen();

	// Use a timer to delay the travel so loading screen can display
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([World, MapName, bAbsolute, Options]()
	{
		FString TravelURL = MapName;
		if (!Options.IsEmpty())
		{
			TravelURL += FString::Printf(TEXT("?%s"), *Options);
		}
		World->ServerTravel(TravelURL, bAbsolute);
	});

	FTimerHandle UnusedHandle;
	World->GetTimerManager().SetTimer(UnusedHandle, TimerDelegate, FMath::Max(Delay, 0.001f), false);
}
