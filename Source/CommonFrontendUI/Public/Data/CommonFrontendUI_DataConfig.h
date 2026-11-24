#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Input/UIActionBindingHandle.h"
#include "CommonFrontendUI_DataConfig.generated.h"

class UCommonFrontendActivatableWidget;
class UCommonPrimaryLayoutWidget;

/**
 * Struct defining an individual entry within a widget stack system.
 * Allows dynamic control over UI stack behavior, widget creation, and input settings via tags.
 */
USTRUCT(BlueprintType)
struct FWidgetStackData
{
	GENERATED_BODY()

	/** Unique tag used to identify the widget stack (e.g., CFUI.WidgetStack.GameHud) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Stack", meta = (Categories = "CFUI.WidgetStack"))
	FGameplayTag StackTag;

	/** Optional gameplay tag used for filtering, identification, or behavior mapping (e.g., CFUI.AssociatedTag.OptionsMenu) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Stack", meta = (Categories = "CFUI.AssociatedTag"))
	FGameplayTag AssociatedTag;


	/** Soft class reference to the widget to spawn for this entry */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Stack")
	TSoftClassPtr<UCommonFrontendActivatableWidget> WidgetClass;

	/** UI input configuration for this widget (e.g., input mode, focus, cursor visibility) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Stack")
	FUIInputConfig UI_InputConfig;
};

/**
 * Data asset used to configure the frontend UI system's stack-based widget layout.
 * Defines the primary layout widget and all associated stack entries.
 */
UCLASS(BlueprintType)
class COMMONFRONTENDUI_API UCommonFrontendUI_DataConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	/** The primary layout widget for the frontend system (e.g., root HUD or menu container) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frontend UI")
	TSoftClassPtr<UCommonPrimaryLayoutWidget> PrimaryLayOutWidget;

	/** Array of stack entries used to define widget behavior and mapping via tags */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frontend UI", meta = (TitleProperty = "AssociatedTag"))
	TArray<FWidgetStackData> StackData;

	/** Retrieves the widget class mapped to the given associated tag */
	TSoftClassPtr<UCommonFrontendActivatableWidget> GetWidgetClassByAssociatedTag(const FGameplayTag& InAssociatedTag) const;

	/** Retrieves the stack tag mapped to the given associated tag */
	FGameplayTag GetStackTagByAssociatedTag(const FGameplayTag& InAssociatedTag) const;

	/** Retrieves the input config mapped to the given associated tag */
	FUIInputConfig GetUI_InputConfigByAssociatedTag(const FGameplayTag& InAssociatedTag) const;

	/** Returns the entire widget stack data struct by associated tag */
	FWidgetStackData GetStackDataAssociatedTag(const FGameplayTag& InAssociatedTag) const;

	/** Retrieves the associated tag for a given stack tag */
	FGameplayTag GetAssociatedTagByStackTag(const FGameplayTag& InStackTag) const;
};
