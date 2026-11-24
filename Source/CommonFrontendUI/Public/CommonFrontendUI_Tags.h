#pragma once

#include "NativeGameplayTags.h"

/**
 * CFUI_GameplayTags
 *
 * Namespace that declares all gameplay tags used by the Common Frontend UI system.
 * Tags are grouped by usage context: widget stacks (screen layers), frontend widgets, and modal/popups.
 * These tags enable flexible registration, lookup, and activation of UI elements at runtime.
 */
namespace CFUI_GameplayTags
{
	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_WidgetStack_Modal);
	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_WidgetStack_GameMenu);
	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_WidgetStack_GameHud);
	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_WidgetStack_Frontend);

	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_AssociatedTag_MainMenu);
	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_AssociatedTag_OptionsMenu);
	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_AssociatedTag_Credits);
	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_AssociatedTag_LoadingScreen);
	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_AssociatedTag_KeyRemapScreen);

	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_AssociatedTag_ConfirmationDialog);
	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_AssociatedTag_NotificationPopup);
	COMMONFRONTENDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CFUI_AssociatedTag_Tooltip);
}

