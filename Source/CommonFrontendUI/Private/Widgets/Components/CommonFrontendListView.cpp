#include "Widgets/Components/CommonFrontendListView.h"
#if WITH_EDITOR
#include "Editor\WidgetCompilerLog.h"
# endif
#include "Widgets/DataObject/ListDataObjectCollection.h"
#include "Widgets/ListEntries/ListEntryWidgetBase.h"

UUserWidget& UCommonFrontendListView::OnGenerateEntryWidgetInternal(UObject* Item,
                                                                    TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (IsDesignTime())
	{
		return Super::OnGenerateEntryWidgetInternal(Item,DesiredEntryClass,OwnerTable);
	}

	if (TSubclassOf<UListEntryWidgetBase> FoundWidgetClass = DataListEntryMapping->FindEntryWidgetClassByDataObject(CastChecked<UListDataObjectBase>(Item)))
	{
		return GenerateTypedEntry<UListEntryWidgetBase>(FoundWidgetClass,OwnerTable);
	}
	else
	{
		return Super::OnGenerateEntryWidgetInternal(Item,DesiredEntryClass,OwnerTable);
	}
}

bool UCommonFrontendListView::OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem)
{
	return !FirstSelectedItem->IsA<UListDataObjectCollection>();
}

#if WITH_EDITOR	

void UCommonFrontendListView::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!DataListEntryMapping)
	{
		CompileLog.Error(FText::FromString(
		TEXT("The variable DataListEntryMapping has no valid data asset assigned ") +
		GetClass()->GetName() +
		TEXT(" needs a valid data asset to function properly")
		));
	}}
#endif