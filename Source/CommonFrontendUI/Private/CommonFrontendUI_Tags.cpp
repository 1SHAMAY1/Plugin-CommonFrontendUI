#include "CommonFrontendUI_Tags.h"

namespace CFUI_GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_WidgetStack_Modal, "CFUI.WidgetStack.Modal", "Stack for modal dialogs such as confirmations, tooltips, and overlays.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_WidgetStack_GameMenu, "CFUI.WidgetStack.GameMenu", "Stack for in-game menus like pause, inventory, or settings during gameplay.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_WidgetStack_GameHud, "CFUI.WidgetStack.GameHud", "Stack for displaying HUD widgets during gameplay (e.g., health bar, minimap).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_WidgetStack_Frontend, "CFUI.WidgetStack.Frontend", "Stack for main frontend screens like main menu, options, and splash.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_AssociatedTag_MainMenu, "CFUI.AssociatedTag.MainMenu", "Tag associated with the main menu screen widget.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_AssociatedTag_OptionsMenu, "CFUI.AssociatedTag.OptionsMenu", "Tag associated with the options/settings menu widget.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_AssociatedTag_Credits, "CFUI.AssociatedTag.Credits", "Tag associated with the credits screen widget.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_AssociatedTag_LoadingScreen, "CFUI.AssociatedTag.LoadingScreen", "Tag for loading screen widget shown during async level transitions.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_AssociatedTag_KeyRemapScreen, "CFUI.AssociatedTag.KeyRemapScreen", "Tag for input remapping UI (e.g., keybinding screen).");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_AssociatedTag_ConfirmationDialog, "CFUI.AssociatedTag.ConfirmationDialog", "Tag for confirmation dialog popups (Yes/No prompts).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_AssociatedTag_NotificationPopup, "CFUI.AssociatedTag.NotificationPopup", "Tag for transient notification popups (e.g., errors, achievements).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CFUI_AssociatedTag_Tooltip, "CFUI.AssociatedTag.Tooltip", "Tag for tooltips or hover-based context UI elements.");
}
