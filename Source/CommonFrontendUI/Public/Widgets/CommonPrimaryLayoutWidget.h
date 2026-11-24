#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "CommonPrimaryLayoutWidget.generated.h"

class UCommonActivatableWidgetContainerBase;

/**
 * UCommonPrimaryLayoutWidget
 *
 * Abstract base class for the root layout of a frontend UI in the CommonFrontendUI system.
 * It provides a system for managing multiple named widget stacks (containers) identified by Gameplay Tags.
 * These stacks are used to manage and activate widgets dynamically in a layered or contextual layout.
 *
 * Features:
 * - Allows Blueprint or native registration of named widget stacks using gameplay tags.
 * - Provides a lookup mechanism to retrieve a stack based on its tag.
 * - Designed to be extended by a project-specific primary layout to manage screen layers like HUD, menus, modals, etc.
 *
 * Expected Usage:
 * - Derive your own primary layout widget (e.g., `UMyGamePrimaryLayout`) from this base class.
 * - Register relevant widget stacks via `RegisterWidgetStack()` in Blueprint or C++.
 * - Use `FindStackByTag()` to push or activate widgets in specific screen layers.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class COMMONFRONTENDUI_API UCommonPrimaryLayoutWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:

	/**
	 * Finds a previously registered widget stack using the provided gameplay tag.
	 *
	 * @param InTag The gameplay tag associated with the stack.
	 * @return Pointer to the found widget stack, or nullptr if not registered.
	 */
	UCommonActivatableWidgetContainerBase* FindStackByTag(const FGameplayTag& InTag) const;

protected:

	/**
	 * Registers a widget stack that can be referenced later by a gameplay tag.
	 * Should be called during initialization or construction.
	 *
	 * @param InStackTag A gameplay tag identifying the stack (e.g., CFUI.WidgetStack.HUD).
	 * @param InStack The widget stack container to register.
	 */
	UFUNCTION(BlueprintCallable, Category = "CommonFrontendUI|Layout")
	void RegisterWidgetStack(UPARAM(meta = (Categories = "CFUI.WidgetStack")) FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack);

private:

	/** Mapping of gameplay tags to their corresponding registered widget stacks. */
	UPROPERTY(Transient)
	TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> RegisterWidgetStackMap;

};
