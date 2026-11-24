#include "Widgets/DataObject/ListDataObjectResolution.h"

#include "Utility/CommonFrontendLibrary.h"
#include "Utility/CommonFrontendGameUserSettings.h"
#include "Utility/DeveloperSettings/CommonFrontendDeveloperSettings.h"
#include "Kismet/KismetSystemLibrary.h"

void UListDataObjectResolution::InitResolutionValues()
{
	TArray<FIntPoint> AvaialbleResolutions;

	UKismetSystemLibrary::GetSupportedFullscreenResolutions(AvaialbleResolutions);

	if (const UCommonFrontendDeveloperSettings* DevSettings = GetDefault<UCommonFrontendDeveloperSettings>())
	{
		for (const FIntPoint& CustomRes : DevSettings->CustomResolutions)
		{
			AvaialbleResolutions.AddUnique(CustomRes);
		}
	}

	AvaialbleResolutions.Sort(
		[](const FIntPoint& A, const FIntPoint& B)->bool
		{
			return A.SizeSquared() < B.SizeSquared();
		}
	);

	if (AvaialbleResolutions.Num() > 0)
	{
		for (const FIntPoint& Resolution : AvaialbleResolutions)
		{	
			AddDynamicOption(ResToValueString(Resolution),ResToDisplayText(Resolution));
		}

		MaximumAllowedResolution = ResToValueString(AvaialbleResolutions.Last());

		SetDefaultValueFromString(MaximumAllowedResolution);
	}
	else
	{
		FIntPoint FallbackRes(1920, 1080);
		AddDynamicOption(ResToValueString(FallbackRes), ResToDisplayText(FallbackRes));
		MaximumAllowedResolution = ResToValueString(FallbackRes);
		SetDefaultValueFromString(MaximumAllowedResolution);
	}
}

void UListDataObjectResolution::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();

	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = ResToDisplayText(UCommonFrontendLibrary::GetCommonFrontendGameUserSettings()->GetScreenResolution());
	}
}

FString UListDataObjectResolution::ResToValueString(const FIntPoint& InResolution) const
{
	return FString::Printf(TEXT("(X=%i,Y=%i)"),InResolution.X,InResolution.Y);
}

FText UListDataObjectResolution::ResToDisplayText(const FIntPoint& InResolution) const
{
	const FString DisplayString = FString::Printf(TEXT("%i x %i"),InResolution.X,InResolution.Y);

	return FText::FromString(DisplayString);
}
