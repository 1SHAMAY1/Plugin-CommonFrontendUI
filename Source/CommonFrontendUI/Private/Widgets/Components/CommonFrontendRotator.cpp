#include "Widgets/Components/CommonFrontendRotator.h"

#include "CommonTextBlock.h"

void UCommonFrontendRotator::SetSelectedOptionByText(const FText& InTextOption)
{
	const int32 FoundIndex = TextLabels.IndexOfByPredicate(
			[InTextOption](const FText& TextItem)->bool
			{
				return TextItem.EqualTo(InTextOption);
			}
		);

	if (FoundIndex != INDEX_NONE)
	{
		SetSelectedItem(FoundIndex);
	}
	else
	{
		MyText->SetText(InTextOption);
	}
}
