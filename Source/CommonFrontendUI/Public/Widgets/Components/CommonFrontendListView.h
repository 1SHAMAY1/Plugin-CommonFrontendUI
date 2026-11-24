#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "Data/DataListEntryMappingConfig.h"
#include "CommonFrontendListView.generated.h"

/**
 * A specialized list view used by the Common Frontend UI system.
 * 
 * This list view supports dynamic entry widget generation using a configurable data mapping asset,
 * and overrides navigability logic for custom UI navigation handling.
 */
UCLASS()
class COMMONFRONTENDUI_API UCommonFrontendListView : public UCommonListView
{
	GENERATED_BODY()

protected:

	/**
	 * Called when a list entry widget is being generated.
	 * Uses a data-driven entry mapping to determine which widget to instantiate.
	 *
	 * @param Item                The data object for the list item.
	 * @param DesiredEntryClass   The desired widget class to use as fallback.
	 * @param OwnerTable          The owning Slate table view.
	 * @return                    A reference to the newly generated widget.
	 */
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;

	/**
	 * Determines whether the list is selectable or navigable based on the given item.
	 * Can be overridden to control UI focus/navigation behavior.
	 *
	 * @param FirstSelectedItem The first selected item to evaluate.
	 * @return                   True if the item is selectable/navigable, false otherwise.
	 */
	virtual bool OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem) override;

private:

#if WITH_EDITOR
	/**
	 * Validates list view settings at compile time inside the UMG editor.
	 *
	 * @param CompileLog The compiler log used to report errors or warnings.
	 */
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif

	/** The data-driven entry mapping config used to resolve which widget to use per list entry. */
	UPROPERTY(EditAnywhere, Category = "Frontend List View Settings")
	UDataListEntryMappingConfig* DataListEntryMapping;
};
