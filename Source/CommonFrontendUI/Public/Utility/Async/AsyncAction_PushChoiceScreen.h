#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Widgets/CommonFrontendChoiceScreen.h"
#include "AsyncAction_PushChoiceScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoiceScreenButtonClickedDelegate, const FString&, SelectedLabel);

UCLASS()
class COMMONFRONTENDUI_API UAsyncAction_PushChoiceScreen : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCosmetic, BlueprintCallable, meta = (
		DisplayName = "Show Choice Screen",
		WorldContext = "WorldContextObject",
		HidePin = "WorldContextObject",
		BlueprintInternalUseOnly = "true"
	))
	static UAsyncAction_PushChoiceScreen* PushChoiceScreen(
		const UObject* WorldContextObject,
		FText ScreenTitle,
		FText ScreenMessage,
		const TArray<FString>& ButtonLabels
	);

	UPROPERTY(BlueprintAssignable, Category = "Choice Screen")
	FOnChoiceScreenButtonClickedDelegate OnButtonClicked;

	virtual void Activate() override;

private:
	TWeakObjectPtr<UWorld> CachedOwningWorld;
	FText CachedTitle;
	FText CachedMessage;
	TArray<FString> CachedButtonLabels;
};
