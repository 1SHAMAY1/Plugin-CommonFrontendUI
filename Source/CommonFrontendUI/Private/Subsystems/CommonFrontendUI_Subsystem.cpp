#include "Subsystems/CommonFrontendUI_Subsystem.h"
#include "Engine/GameInstance.h"

#include "Widgets/CommonFrontendChoiceScreen.h"
#include "CommonFrontendUI_Tags.h"
#include "Data/CommonFrontendUI_DataConfig.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/DeveloperSettings/CommonFrontendDeveloperSettings.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widgets/CommonFrontendActivatableWidget.h"
#include "Widgets/CommonPrimaryLayoutWidget.h"

UCommonFrontendUI_Subsystem* UCommonFrontendUI_Subsystem::Get(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;

	if (const UWorld* World = WorldContext->GetWorld())
		if (UGameInstance* GameInstance = World->GetGameInstance())
			return GameInstance->GetSubsystem<UCommonFrontendUI_Subsystem>();

	return nullptr;
	
}

void UCommonFrontendUI_Subsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UCommonFrontendDeveloperSettings* Settings = GetDefault<UCommonFrontendDeveloperSettings>();
	if (Settings && Settings->DefaultFrontendUI_DataConfig.IsValid())
	{
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

		Streamable.RequestAsyncLoad(Settings->DefaultFrontendUI_DataConfig,
			FStreamableDelegate::CreateUObject(this, &UCommonFrontendUI_Subsystem::OnDataConfigLoaded, TSoftObjectPtr<UCommonFrontendUI_DataConfig>(Settings->DefaultFrontendUI_DataConfig)));
	}
}

void UCommonFrontendUI_Subsystem::OnDataConfigLoaded(TSoftObjectPtr<UCommonFrontendUI_DataConfig> LoadedAsset)
{
	if (LoadedAsset.IsValid())
		CurrentDataConfig = LoadedAsset.Get(); 
}


bool UCommonFrontendUI_Subsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance()) return false;
	TArray<UClass*> FoundClasses;
	GetDerivedClasses(GetClass(), FoundClasses);
	return FoundClasses.IsEmpty();

}

void UCommonFrontendUI_Subsystem::RegisterWidgetDataConfigAsync(
	UCommonFrontendUI_DataConfig* InConfig,
	FOnWidgetConfigLoaded OnLoadedCallback)
{
	if (!InConfig)
	{
		UE_LOG(LogTemp, Warning, TEXT("InConfig is null."));
		OnLoadedCallback.ExecuteIfBound(nullptr);
		return;
	}

	if (InConfig->PrimaryLayOutWidget.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("PrimaryLayOutWidget is invalid."));
		OnLoadedCallback.ExecuteIfBound(nullptr);
		return;
	}


	if (CurrentLayOutWidget && CurrentLayOutWidget->IsValidLowLevel())
	{
		CurrentLayOutWidget->RemoveFromParent();
		CurrentLayOutWidget->ConditionalBeginDestroy();
		CurrentLayOutWidget = nullptr;
	}

	CurrentDataConfig = InConfig;

	const FSoftClassPath WidgetClassPath = InConfig->PrimaryLayOutWidget.ToString();
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	Streamable.RequestAsyncLoad(WidgetClassPath, [this, WidgetClassPath, OnLoadedCallback]()
	{
		UClass* LoadedClass = Cast<UClass>(WidgetClassPath.ResolveClass());
		if (!LoadedClass || !LoadedClass->IsChildOf(UCommonPrimaryLayoutWidget::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load PrimaryLayOutWidget class."));
			OnLoadedCallback.ExecuteIfBound(nullptr);
			return;
		}

		UWorld* World = GetWorld();
		if (!World)
		{
			OnLoadedCallback.ExecuteIfBound(nullptr);
			return;
		}

		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
		if (!PC)
		{
			OnLoadedCallback.ExecuteIfBound(nullptr);
			return;
		}

		CurrentLayOutWidget = CreateWidget<UCommonPrimaryLayoutWidget>(PC, LoadedClass);
		if (!CurrentLayOutWidget)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create layout widget."));
			OnLoadedCallback.ExecuteIfBound(nullptr);
			return;
		}

		CurrentLayOutWidget->AddToViewport();
		OnLoadedCallback.ExecuteIfBound(CurrentLayOutWidget);
	});
}

void UCommonFrontendUI_Subsystem::PushSoftWidgetToStackAsync(
	const FGameplayTag& StackTag,
	TSoftClassPtr<UCommonFrontendActivatableWidget> WidgetClass,
	TFunction<void(EAsyncPushActionState, UCommonFrontendActivatableWidget*)> AsyncPushActionCallback)
{
	if (WidgetClass.IsNull() || !AsyncPushActionCallback || !CurrentLayOutWidget || !CurrentDataConfig)
	{
		return;
	}

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		WidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[WidgetClass, StackTag, AsyncPushActionCallback, this]()
			{
				UClass* LoadedWidgetClass = WidgetClass.Get();
				if (!LoadedWidgetClass || !CurrentLayOutWidget || !CurrentDataConfig)
				{
					return;
				}

				UCommonActivatableWidgetContainerBase* TargetStack = CurrentLayOutWidget->FindStackByTag(StackTag);
				if (!TargetStack)
				{
					return;
				}

				UCommonFrontendActivatableWidget* CreatedWidget = TargetStack->AddWidget<UCommonFrontendActivatableWidget>(
					LoadedWidgetClass,
					[AsyncPushActionCallback](UCommonFrontendActivatableWidget& CreatedInstance)
					{
						AsyncPushActionCallback(EAsyncPushActionState::OnCreated, &CreatedInstance);
					}
				);

				if (CreatedWidget)
				{
					AsyncPushActionCallback(EAsyncPushActionState::OnPushed, CreatedWidget);

					FGameplayTag AssociatedTag = CurrentDataConfig->GetAssociatedTagByStackTag(StackTag);
					FWidgetStackData StackData = CurrentDataConfig->GetStackDataAssociatedTag(AssociatedTag);

					LoadedWidgetMap.FindOrAdd(LoadedWidgetClass, StackData);
				}
			}
		)
	);
}

void UCommonFrontendUI_Subsystem::PushSoftWidgetAsync(
	const FGameplayTag& AssociatedTag,
	TFunction<void(EAsyncPushActionState, UCommonFrontendActivatableWidget*)> AsyncPushActionCallback)
{
	if (!CurrentDataConfig || AssociatedTag.IsValid() == false || !AsyncPushActionCallback)
	{
		return;
	}

	const FGameplayTag FoundStackTag = CurrentDataConfig->GetStackTagByAssociatedTag(AssociatedTag);
	const TSoftClassPtr<UCommonFrontendActivatableWidget> FoundClass = CurrentDataConfig->GetWidgetClassByAssociatedTag(AssociatedTag);

	if (!FoundStackTag.IsValid() || FoundClass.IsNull())
	{
		return;
	}

	PushSoftWidgetToStackAsync(
		FoundStackTag,
		FoundClass,
		AsyncPushActionCallback
	);
}

void UCommonFrontendUI_Subsystem::PushChoiceScreenToModalStackAsync(
	const FText& InScreenTitle,
	const FText& InScreenMsg,
	const TArray<FString>& InButtons,
	TFunction<void(const FString&)> ButtonClickedCallback)
{
	if (!CurrentDataConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("PushChoiceScreenToModalStackAsync: No data config available"));
		return;
	}

	UChoiceScreenInfoObject* Info = UChoiceScreenInfoObject::Create(InScreenTitle, InScreenMsg, InButtons);
	if (!Info)
	{
		UE_LOG(LogTemp, Error, TEXT("PushChoiceScreenToModalStackAsync: Failed to create screen info"));
		return;
	}

	PushSoftWidgetAsync(
		CFUI_GameplayTags::CFUI_AssociatedTag_ConfirmationDialog,
		[Info, ButtonClickedCallback](EAsyncPushActionState InPushState, UCommonFrontendActivatableWidget* PushedWidget)
		{
			if (InPushState != EAsyncPushActionState::OnCreated || !PushedWidget)
			{
				return;
			}

			UCommonFrontendChoiceScreen* Screen = Cast<UCommonFrontendChoiceScreen>(PushedWidget);
			if (!Screen)
			{
				UE_LOG(LogTemp, Error, TEXT("PushChoiceScreenToModalStackAsync: Pushed widget is not a choice screen"));
				return;
			}

			Screen->InitChoiceScreen(Info, ButtonClickedCallback);
		}
	);
}


FWidgetStackData UCommonFrontendUI_Subsystem::GetWidgetStackDataForWidget(
	TSubclassOf<UCommonFrontendActivatableWidget> WidgetToCheck) const
{
	if (!WidgetToCheck)
		return FWidgetStackData();
	

	if (const FWidgetStackData* FoundData = LoadedWidgetMap.Find(WidgetToCheck))
		return *FoundData;

	return FWidgetStackData(); 
}

FUIInputConfig UCommonFrontendUI_Subsystem::GetInputConfigForWidget(
	TSubclassOf<UCommonFrontendActivatableWidget> WidgetToCheck) const
{
	if (!WidgetToCheck)
		return FUIInputConfig(); 

	if (const FWidgetStackData* StackData = LoadedWidgetMap.Find(WidgetToCheck))
		return StackData->UI_InputConfig;

	return FUIInputConfig(); 
}




