#include "Widgets/CommonPrimaryLayoutWidget.h"

UCommonActivatableWidgetContainerBase* UCommonPrimaryLayoutWidget::FindStackByTag(const FGameplayTag& InTag) const
{
	checkf(RegisterWidgetStackMap.Contains(InTag),TEXT("Cannot find widget stack"));
	return RegisterWidgetStackMap.FindRef(InTag);
}

void UCommonPrimaryLayoutWidget::RegisterWidgetStack(UPARAM(meta=(Categories = "CFUI.WidgetStack"))
	FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack)
{
	if (IsDesignTime() || RegisterWidgetStackMap.Contains(InStackTag)) return;
	RegisterWidgetStackMap.Add(InStackTag, InStack);
}
