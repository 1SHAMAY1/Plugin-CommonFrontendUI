#pragma once

#include "CoreMinimal.h"
#include "Widgets/Components/CommonFrontendButtonBase.h"
#include "Data/CommonFrontendUI_StructsAndEnums.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Slate/ButtonStyleAsset.h"
#include "ListEntryWidgetBase.generated.h"

class UCommonTextBlock;
class UListDataObjectBase;

/**
 * UListEntryWidgetBase
 *
 * Base class for an entry widget in a dynamic list UI that binds to a UListDataObjectBase data source.
 * 
 * Designed to work with option/config menus, this widget supports gamepad focus handling, selection state changes,
 * data-driven updates, and dynamic visual highlighting.
 * 
 * Override this class in Blueprint to implement visual logic using:
 * - BP_OnListEntryWidgetHovered()
 * - BP_OnToggleEntryWidgetHighlightState()
 * - BP_GetWidgetToFocusForGamepad()
 * 
 * Integrates with:
 * - IUserObjectListEntry for ListView functionality
 * - UCommonUserWidget for consistent UI patterns
 */
UCLASS()
class COMMONFRONTENDUI_API UListEntryWidgetBase : public UCommonFrontendButtonBase, public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	/** Called from Blueprint to respond to entry hover changes. */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On List Entry Widget Hovered"))
	void BP_OnListEntryWidgetHovered(bool bWasHovered, bool bIsEntryWidgetStillSelected);
	
	/** Native C++ handler for list entry hover state (calls Blueprint event). */
	virtual void NativeOnListEntryWidgetHovered(bool bWasHovered);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="ListEntryWidget|Style")
	TSubclassOf<UCommonButtonStyle> ButtonStyleEntryHovered;

protected:

	/** 
	 * Blueprint hook to determine which widget inside this entry should receive focus for gamepad navigation.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Widget To Focus For Gamepad"))
	UWidget* BP_GetWidgetToFocusForGamepad() const;

	virtual UWidget* NativeGetWidgetToFocusForGamepad() const;


	/** 
	 * Blueprint hook for toggling visual highlight (e.g., when selected or hovered).
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Toggle Entry Widget Highlight State"))
	void BP_OnToggleEntryWidgetHighlightState(bool bShouldHighlight);
	
	/** Called when the underlying list item object is set (entry is initialized with data). */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	/** Called when selection state of this entry changes (selected/unselected). */
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	/** Called when this entry widget is released from the owning list view. */
	virtual void NativeOnEntryReleased() override;

	/** Handles custom focus behavior when widget receives input focus (e.g., via keyboard/gamepad). */
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	
	/** 
	 * Called when this entry is initialized with a UListDataObjectBase instance.
	 * Override to bind to the data or cache references.
	 */
	virtual void OnOwningListDataObjectSet(UListDataObjectBase* InOwningListDataObject);

	/**
	 * Called when the bound owning list data object is modified by user interaction or system updates.
	 */
	virtual void OnOwningListDataObjectModified(UListDataObjectBase* OwningModifiedData, EOptionsListDataModifyReason ModifyReason);

	/**
	 * Called when a dependency of this entry's owning data object is modified.
	 * Useful for dependent display logic (e.g., toggling another widget disables this one).
	 */
	virtual void OnOwningDependencyDataObjectModified(UListDataObjectBase* OwningModifiedDependencyData, EOptionsListDataModifyReason ModifyReason);

	/** Selects this entry manually in its parent list (e.g., from code). */
	void SelectThisEntryWidget();

	virtual void NativeOnHovered() override;

private:

	/** Cached reference to the data object backing this list entry. */
	UPROPERTY(Transient)
	UListDataObjectBase* CachedOwningDataObject;

};


