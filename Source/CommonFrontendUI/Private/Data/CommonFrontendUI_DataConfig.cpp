#include "Data/CommonFrontendUI_DataConfig.h"

TSoftClassPtr<UCommonFrontendActivatableWidget> UCommonFrontendUI_DataConfig::GetWidgetClassByAssociatedTag(
	const FGameplayTag& InAssociatedTag) const
{
	for (const FWidgetStackData& Data : StackData)
	{
		if (Data.AssociatedTag == InAssociatedTag)
		{
			return Data.WidgetClass;
		}
	}
	return nullptr;
}

FGameplayTag UCommonFrontendUI_DataConfig::GetStackTagByAssociatedTag(const FGameplayTag& InAssociatedTag) const
{
	for (const FWidgetStackData& Data : StackData)
	{
		if (Data.AssociatedTag == InAssociatedTag)
		{
			return Data.StackTag;
		}
	}
	return FGameplayTag::EmptyTag;
}

FUIInputConfig UCommonFrontendUI_DataConfig::GetUI_InputConfigByAssociatedTag(const FGameplayTag& InAssociatedTag) const
{
	for (const FWidgetStackData& Data : StackData)
	{
		if (Data.AssociatedTag == InAssociatedTag)
		{
			return Data.UI_InputConfig;
		}
	}
	return FUIInputConfig();
}

FWidgetStackData UCommonFrontendUI_DataConfig::GetStackDataAssociatedTag(const FGameplayTag& InAssociatedTag) const
{
	for (const FWidgetStackData& Data : StackData)
	{
		if (Data.AssociatedTag == InAssociatedTag)
		{
			return Data;
		}
	}
	return FWidgetStackData();
}

FGameplayTag UCommonFrontendUI_DataConfig::GetAssociatedTagByStackTag(const FGameplayTag& InStackTag) const
{
	for (const FWidgetStackData& Data : StackData)
	{
		if (Data.StackTag == InStackTag)
		{
			return Data.AssociatedTag;
		}
	}
	return FGameplayTag::EmptyTag;
}
