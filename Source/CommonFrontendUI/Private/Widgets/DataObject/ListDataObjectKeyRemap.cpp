#include "Widgets/DataObject/ListDataObjectKeyRemap.h"

#include "CommonInputSubsystem.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "GameplayTagContainer.h"

void UListDataObjectKeyRemap::InitKeyRemapData(UEnhancedInputUserSettings* InOwningInputUserSettings,
                                               UEnhancedPlayerMappableKeyProfile* InKeyProfile, ECommonInputType InDesiredInputKeyType,
                                               const FPlayerKeyMapping& InOwningPlayerKeyMapping)
{
	
	CachedOwningInputUserSettings = InOwningInputUserSettings;
	CachedOwningKeyProfile = InKeyProfile;
	CachedDesiredInputKeyType = InDesiredInputKeyType;
	CachedOwningMappingName = InOwningPlayerKeyMapping.GetMappingName();
	CachedOwningMappableKeySlot = InOwningPlayerKeyMapping.GetSlot();
}

FSlateBrush UListDataObjectKeyRemap::GetIconFromCurrentKey() const
{
	check(CachedOwningInputUserSettings);

	FSlateBrush FoundBrush;

	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(CachedOwningInputUserSettings->GetLocalPlayer());

	check(CommonInputSubsystem);

	const bool bHasFoundBrush = UCommonInputPlatformSettings::Get()->TryGetInputBrush(
		FoundBrush,
		GetOwningKeyMapping()->GetCurrentKey(),
		CachedDesiredInputKeyType,
		CommonInputSubsystem->GetCurrentGamepadName()
	);

	return FoundBrush;
}

void UListDataObjectKeyRemap::BindNewInputKey(const FKey& InNewKey)
{
	check(CachedOwningInputUserSettings);
    
    	FMapPlayerKeyArgs KeyArgs;
    	KeyArgs.MappingName = CachedOwningMappingName;
    	KeyArgs.Slot = CachedOwningMappableKeySlot;
    	KeyArgs.NewKey = InNewKey;
    
    	FGameplayTagContainer Container;
    	CachedOwningInputUserSettings->MapPlayerKey(KeyArgs,Container);
    	CachedOwningInputUserSettings->SaveSettings();
    
    	NotifyListDataModified(this);
}

bool UListDataObjectKeyRemap::HasDefaultValue() const
{
	return GetOwningKeyMapping()->GetDefaultKey().IsValid();
}

bool UListDataObjectKeyRemap::CanResetBackToDefaultValue() const
{
	return HasDefaultValue() && GetOwningKeyMapping()->IsCustomized();
}

bool UListDataObjectKeyRemap::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{	
		check(CachedOwningInputUserSettings);

		GetOwningKeyMapping()->ResetToDefault();

		CachedOwningInputUserSettings->SaveSettings();

		NotifyListDataModified(this,EOptionsListDataModifyReason::ResetToDefault);

		return true;
	}

	return false;
}

FPlayerKeyMapping* UListDataObjectKeyRemap::GetOwningKeyMapping() const
{
	check(CachedOwningKeyProfile);

	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = CachedOwningMappingName;
	KeyArgs.Slot = CachedOwningMappableKeySlot;

	return CachedOwningKeyProfile->FindKeyMapping(KeyArgs);
}
