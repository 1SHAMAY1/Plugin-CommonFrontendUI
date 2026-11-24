#include "Widgets/CommonFrontendActivatableWidget.h"

#include "Data/CommonFrontendUI_DataConfig.h"
#include "Subsystems/CommonFrontendUI_Subsystem.h"

TOptional<FUIInputConfig> UCommonFrontendActivatableWidget::GetDesiredInputConfig() const
{
	if (const UCommonFrontendUI_Subsystem* FrontendSubsystem = UCommonFrontendUI_Subsystem::Get(this))
		return FrontendSubsystem->GetInputConfigForWidget(this->GetClass());
	
	return TOptional<FUIInputConfig>();
}

UWidget* UCommonFrontendActivatableWidget::NativeGetDesiredFocusTarget() const
{
	return const_cast<UWidget*>(Cast<UWidget>(this));
}
