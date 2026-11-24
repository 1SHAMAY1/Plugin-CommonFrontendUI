#include "Widgets/CommonFrontendDataRegistry.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"

#include "CommonFrontendLibrary.h"
#include "Utility/DeveloperSettings/CommonFrontendDeveloperSettings.h"
#include "EnhancedInputSubsystems.h"
#include "Widgets/DataObject/ListDataObjectCollection.h"
#include "Widgets/DataObject/ListDataObjectKeyRemap.h"
#include "Widgets/DataObject/ListDataObjectResolution.h"
#include "Widgets/DataObject/ListDataObjectScalar.h"
#include "Widgets/DataObject/ListDataObjectString.h"
#include "Data/OptionsDataInteractionHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Data/CommonFrontendSettingsRegistry.h"
#include "Widgets/DataObject/ListDataObjectValue.h"
#include "Widgets/DataObject/ListDataObjectNestedTab.h"
#include "Widgets/DataObject/ListDataObjectNestedTabCollection.h"
#include "InputAction.h"
#include "GameFramework/GameUserSettings.h"


TArray<UListDataObjectBase*> UCommonFrontendDataRegistry::GetListSourceItemsBySelectedTabID(
	const FName& InSelectedTabID) const
{
	// Locate the specific tab collection matching the requested ID
	UListDataObjectCollection* const* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate(
		[InSelectedTabID](UListDataObjectCollection* AvailableTabCollection)->bool
		{
			return AvailableTabCollection->GetDataID() == InSelectedTabID;
		}
	);

	if (!FoundTabCollectionPtr)
	{
		if (UCommonFrontendLibrary::ShouldEnableDebug())
		UE_LOG(LogTemp, Error, TEXT("No valid tab found under the ID %s"), *InSelectedTabID.ToString());
		UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
		return {};
	}

	UListDataObjectCollection* FoundTabCollection = *FoundTabCollectionPtr;

	TArray<UListDataObjectBase*> AllChildListItems;

	// Collect all children recursively to provide a flat list for the UI (e.g. ListView)
	for (UListDataObjectBase* ChildListData : FoundTabCollection->GetAllChildListData())
	{
		if (!ChildListData)
		{
			continue;
		}

		AllChildListItems.Add(ChildListData);

		// Skip recursive expansion for nested tab collections - they handle their own child display
		if (ChildListData->IsA<UListDataObjectNestedTabCollection>())
		{
			continue;
		}

		if (ChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(ChildListData,AllChildListItems);
		}
	}

	return AllChildListItems;
}

void UCommonFrontendDataRegistry::FindChildListDataRecursively(UListDataObjectBase* InParentData,
	TArray<UListDataObjectBase*>& OutFoundChildListData) const
{
	if (!InParentData || !InParentData->HasAnyChildListData())
	{
		return;
	}

	for (UListDataObjectBase* SubChildListData : InParentData->GetAllChildListData())
	{
		if (!SubChildListData)
		{
			continue;
		}

		OutFoundChildListData.Add(SubChildListData);

		// Skip recursive expansion for nested tab collections - they handle their own child display
		if (SubChildListData->IsA<UListDataObjectNestedTabCollection>())
		{
			continue;
		}

		if (SubChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(SubChildListData,OutFoundChildListData);
		}
	}
}

void UCommonFrontendDataRegistry::ProcessSettingDef(const UFrontendSettingDef* SettingDef, UListDataObjectCollection* ParentCollection, UEnhancedInputUserSettings* EIUserSettings)
{
	if (!SettingDef || !ParentCollection)
	{
		return;
	}

	// Instantiate the appropriate DataObject based on the concrete definition type
	if (const UFrontendSettingValue* ValSetting = Cast<UFrontendSettingValue>(SettingDef))
	{
		UListDataObjectValue* DataObject = nullptr;

		if (const UFrontendSettingScalar* ScalarSetting = Cast<UFrontendSettingScalar>(ValSetting))
		{
			UListDataObjectScalar* ScalarObj = NewObject<UListDataObjectScalar>(this);
			ScalarObj->SetDisplayValueRange(TRange<float>(ScalarSetting->DisplayMinValue, ScalarSetting->DisplayMaxValue));
			ScalarObj->SetOutputValueRange(TRange<float>(ScalarSetting->OutputMinValue, ScalarSetting->OutputMaxValue));
			ScalarObj->SetSliderStepSize(ScalarSetting->SliderStepSize > 0.0f ? ScalarSetting->SliderStepSize : 0.01f);
			ScalarObj->SetNumberFormattingOptions(UListDataObjectScalar::WithDecimal(ScalarSetting->NumFractionalDigits));
			ScalarObj->SetDisplayNumericType(ScalarSetting->DisplayFormat);
			DataObject = ScalarObj;
		}
		else if (const UFrontendSettingString* StringSetting = Cast<UFrontendSettingString>(ValSetting))
		{
			UListDataObjectString* StringObj = NewObject<UListDataObjectString>(this);
			
			if (!StringSetting->InvalidDisplayValue.IsEmpty())
			{
				StringObj->SetInvalidDisplayValue(StringSetting->InvalidDisplayValue);
			}

			for (const FFrontendStringSelection& Option : StringSetting->PossibleValues)
			{
				StringObj->AddDynamicOption(Option.Value, FText::FromString(Option.DisplayText));
			}
			DataObject = StringObj;
		}
		else if (const UFrontendSettingBool* BoolSetting = Cast<UFrontendSettingBool>(ValSetting))
		{
			UListDataObjectBool* BoolObj = NewObject<UListDataObjectBool>(this);

			// Transfer custom boolean labels from the definition to the runtime data object
			
			if (!BoolSetting->TrueText.IsEmpty())
			{
				BoolObj->OverrideTrueDisplayText(BoolSetting->TrueText);
			}

			if (!BoolSetting->FalseText.IsEmpty())
			{
				BoolObj->OverrideFalseDisplayText(BoolSetting->FalseText);
			}

			DataObject = BoolObj;
		}
		else if (const UFrontendSettingResolution* ResSetting = Cast<UFrontendSettingResolution>(ValSetting))
		{
			UListDataObjectResolution* ResolutionObj = NewObject<UListDataObjectResolution>(this);
			ResolutionObj->InitResolutionValues();
			DataObject = ResolutionObj;
		}

		if (DataObject)
		{
			// Determine the unique identifier for this setting, prioritising manual bindings
			FName DataID = ValSetting->TargetProperty;
			
			if (!ValSetting->GetterFunctionName.IsNone())
			{
				DataID = ValSetting->GetterFunctionName;
			}


			DataObject->SetDataID(DataID);
			DataObject->SetDataDisplayName(ValSetting->DisplayName);
			DataObject->SetDescriptionRichText(ValSetting->Description);
			DataObject->SetDisabledRichText(ValSetting->DisabledText);

			// Register a runtime edit condition if defined (e.g. graying out settings based on quality)
			if (ValSetting->EditCondition)
			{
				TWeakObjectPtr<UFrontendSettingEditCondition> WeakCondition = ValSetting->EditCondition;
				FText DisabledText = ValSetting->DisabledText;
				FString OverrideValue = ValSetting->EditCondition->OverrideValueIfDisabled;

				FOptionsDataEditConditionDescriptor Descriptor;
				Descriptor.SetEditConditionFunc([WeakCondition]() -> bool
				{
					if (WeakCondition.IsValid())
					{
						UGameUserSettings* GUS = GEngine ? GEngine->GetGameUserSettings() : nullptr;
						return WeakCondition->CanEdit(GUS);
					}
					return true;
				});
				Descriptor.SetDisabledRichReason(DisabledText.ToString());
				
				if (!OverrideValue.IsEmpty())
				{
					Descriptor.SetDisabledForcedStringValue(OverrideValue);
				}

				DataObject->AddEditCondition(Descriptor);
			}

			// Configure data access via either Reflection, Manual Function calls, or Audio System overrides
			if (!ValSetting->DefaultValue.IsEmpty())
			{
				DataObject->SetDefaultValueFromString(ValSetting->DefaultValue);
			}

			if (ValSetting->IsA(UFrontendSettingResolution::StaticClass()))
			{
				// Ensure explicit ID for Resolution to fix "None" in logs
				if (DataObject->GetDataID().IsNone())
				{
					DataObject->SetDataID(FName("Setting_Resolution"));
				}

				auto GetterFunc = []() -> FString
				{
					if (UGameUserSettings* GUS = GEngine ? GEngine->GetGameUserSettings() : nullptr)
					{
						FIntPoint Res = GUS->GetScreenResolution();
						return FString::Printf(TEXT("(X=%d,Y=%d)"), Res.X, Res.Y);
					}
					return TEXT("(X=1920,Y=1080)");
				};

				auto SetterFunc = [](const FString& InValue)
				{
					if (UGameUserSettings* GUS = GEngine ? GEngine->GetGameUserSettings() : nullptr)
					{
						FIntPoint NewRes;
						const TCHAR* Buffer = *InValue;
						if (FParse::Value(Buffer, TEXT("X="), NewRes.X) && FParse::Value(Buffer, TEXT("Y="), NewRes.Y))
						{
							if (NewRes.X > 0 && NewRes.Y > 0)
							{
								GUS->SetScreenResolution(NewRes);
							}
						}
					}
				};

				DataObject->SetDataDynamicGetter(MakeShared<FOptionsDataInteractionHelper>(GetterFunc, SetterFunc));
				DataObject->SetDataDynamicSetter(MakeShared<FOptionsDataInteractionHelper>(GetterFunc, SetterFunc));
			}
			else
			{
				if (!ValSetting->GetterFunctionName.IsNone() || !ValSetting->SetterFunctionName.IsNone())
				{
					if (!ValSetting->GetterFunctionName.IsNone())
						DataObject->SetDataDynamicGetter(MakeShared<FOptionsDataInteractionHelper>(ValSetting->GetterFunctionName.ToString()));
					if (!ValSetting->SetterFunctionName.IsNone())
						DataObject->SetDataDynamicSetter(MakeShared<FOptionsDataInteractionHelper>(ValSetting->SetterFunctionName.ToString()));
				}
				else
				{
					const FString PropName = ValSetting->TargetProperty.ToString();
					DataObject->SetDataDynamicGetter(MakeShared<FOptionsDataInteractionHelper>(PropName));
					DataObject->SetDataDynamicSetter(MakeShared<FOptionsDataInteractionHelper>(PropName));
				}
			}

			DataObject->SetShouldApplySettingsImmediately(ValSetting->bAutoSaveOnChange);
			DataObject->SetBufferChanges(!ValSetting->bAutoSaveOnChange);

			// Track this object for dependency linking
			CreatedDataObjectsMap.Add(DataObject->GetDataID(), DataObject);

			// Store pending dependencies to link after all objects are created
			if (ValSetting->DependentSettingIDs.Num() > 0)
			{
				PendingDependencyLinks.Add(DataObject, ValSetting->DependentSettingIDs);
			}
			
			ParentCollection->AddChildListData(DataObject);
		}
	}
	// Handle input remapping definitions by querying the Enhanced Input User Settings
	else if (const UFrontendSettingControls* ControlSetting = Cast<UFrontendSettingControls>(SettingDef))
	{
		if (EIUserSettings)
		{
			for (const auto& ProfilePair : EIUserSettings->GetAllAvailableKeyProfiles())
			{
				UEnhancedPlayerMappableKeyProfile* MappableKeyProfile = ProfilePair.Value;
				if (!MappableKeyProfile)
				{
					continue;
				}

				for (const auto& MappingRowPair : MappableKeyProfile->GetPlayerMappingRows())
				{
					for (const FPlayerKeyMapping& KeyMapping : MappingRowPair.Value.Mappings)
					{	
						const FKey CurrentKey = KeyMapping.GetCurrentKey();
						const bool bIsGamepadKey = CurrentKey.IsGamepadKey();

						if (ControlSetting->InputType == EFrontendControlInputType::KeyboardMouse && bIsGamepadKey)
						{
							continue;
						}
						if (ControlSetting->InputType == EFrontendControlInputType::Gamepad && !bIsGamepadKey)
						{
							continue;
						}

						UListDataObjectKeyRemap* KeyRemapDataObject = NewObject<UListDataObjectKeyRemap>(this);
						KeyRemapDataObject->SetDataID(KeyMapping.GetMappingName());
						KeyRemapDataObject->SetDataDisplayName(KeyMapping.GetDisplayName());
					
						if (const UInputAction* InputAction = KeyMapping.GetAssociatedInputAction())
						{
							if (!InputAction->ActionDescription.IsEmpty())
							{
								KeyRemapDataObject->SetDescriptionRichText(InputAction->ActionDescription);
							}
						}
					
						ECommonInputType InputType = (ControlSetting->InputType == EFrontendControlInputType::Gamepad) ? ECommonInputType::Gamepad : ECommonInputType::MouseAndKeyboard;
					
						KeyRemapDataObject->InitKeyRemapData(EIUserSettings, MappableKeyProfile, InputType, KeyMapping);

						ParentCollection->AddChildListData(KeyRemapDataObject);
					}
				}
			}
		}
	}
	else if (const UFrontendSettingNestedTabCollection* NestedTabCollectionSetting = Cast<UFrontendSettingNestedTabCollection>(SettingDef))
	{
		UListDataObjectNestedTabCollection* CollectionData = NewObject<UListDataObjectNestedTabCollection>(this);
		CollectionData->SetDataID(FName("NestedTabCollection")); 
		CollectionData->SetDataDisplayName(NestedTabCollectionSetting->DisplayName);
		CollectionData->SetDescriptionRichText(NestedTabCollectionSetting->Description);
		
		for (const TObjectPtr<UFrontendSettingNestedTab>& ChildTabDef : NestedTabCollectionSetting->Tabs)
		{
			if (!ChildTabDef) continue;

			UListDataObjectNestedTab* NestedTabData = NewObject<UListDataObjectNestedTab>(this);
			NestedTabData->SetDataID(ChildTabDef->NestedTabId);
			NestedTabData->SetDataDisplayName(ChildTabDef->DisplayName);
			NestedTabData->SetNestedTabId(ChildTabDef->NestedTabId);
			NestedTabData->SetDescriptionRichText(ChildTabDef->Description);

			for (const TObjectPtr<UFrontendSettingDef>& ChildSettingDef : ChildTabDef->ChildSettings)
			{
				ProcessSettingDef(ChildSettingDef, NestedTabData, EIUserSettings);
			}

			CollectionData->AddNestedTab(NestedTabData);
		}

		ParentCollection->AddChildListData(CollectionData);
	}
	else if (const UFrontendSettingNestedTab* NestedTabSetting = Cast<UFrontendSettingNestedTab>(SettingDef))
	{
		UListDataObjectNestedTab* NestedTabData = NewObject<UListDataObjectNestedTab>(this);
		NestedTabData->SetDataID(NestedTabSetting->NestedTabId);
		NestedTabData->SetDataDisplayName(NestedTabSetting->DisplayName);
		NestedTabData->SetNestedTabId(NestedTabSetting->NestedTabId);
		NestedTabData->SetDescriptionRichText(NestedTabSetting->Description);
		NestedTabData->SetIsCollapsed(true); 

		for (const TObjectPtr<UFrontendSettingDef>& ChildSettingDef : NestedTabSetting->ChildSettings)
		{
			ProcessSettingDef(ChildSettingDef, NestedTabData, EIUserSettings);
		}

		ParentCollection->AddChildListData(NestedTabData);
	}
	else if (const UFrontendSettingCategory* CategorySetting = Cast<UFrontendSettingCategory>(SettingDef))
	{
		UListDataObjectCollection* CategoryData = NewObject<UListDataObjectCollection>(this);
		CategoryData->SetDataID(FName(CategorySetting->DisplayName.ToString()));
		CategoryData->SetDataDisplayName(CategorySetting->DisplayName);
		CategoryData->SetDescriptionRichText(CategorySetting->Description);

		for (const TObjectPtr<UFrontendSettingDef>& ChildSettingDef : CategorySetting->ChildSettings)
		{
			ProcessSettingDef(ChildSettingDef, CategoryData, EIUserSettings);
		}

		ParentCollection->AddChildListData(CategoryData);
	}
}

void UCommonFrontendDataRegistry::InitFromSettingsRegistry(const UCommonFrontendSettingsRegistry* Registry, ULocalPlayer* InOwningLocalPlayer)
{
	if (!Registry)
	{
		return;
	}

	RegisteredOptionsTabCollections.Empty();

	UEnhancedInputUserSettings* EIUserSettings = nullptr;
	if (InOwningLocalPlayer)
	{
		if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = InOwningLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			EIUserSettings = EISubsystem->GetUserSettings();
		}
	}

	for (const FFrontendSettingTab& Tab : Registry->Tabs)
	{
		UListDataObjectCollection* TabCollection = NewObject<UListDataObjectCollection>(this);
		TabCollection->SetDataID(Tab.TabId);
		TabCollection->SetDataDisplayName(Tab.TabName);
		TabCollection->SetDescriptionRichText(Tab.Description);

		for (const TObjectPtr<UFrontendSettingDef>& SettingDef : Tab.Settings)
		{
			ProcessSettingDef(SettingDef, TabCollection, EIUserSettings);
		}

		RegisteredOptionsTabCollections.Add(TabCollection);
	}

	LinkPendingDependencies();
}

void UCommonFrontendDataRegistry::LinkPendingDependencies()
{
	for (const auto& Pair : PendingDependencyLinks)
	{
		UListDataObjectBase* ParentObject = Pair.Key;
		const TArray<FName>& DependentIDs = Pair.Value;

		for (const FName& DependentID : DependentIDs)
		{
			if (UListDataObjectBase** FoundDependentPtr = CreatedDataObjectsMap.Find(DependentID))
			{
				if (UListDataObjectBase* DependentObject = *FoundDependentPtr)
				{
					if (UCommonFrontendLibrary::ShouldEnableDebug())
					UE_LOG(LogTemp, Log, TEXT("Linking Dependency: Parent [%s] <-> Dependent [%s]"), *ParentObject->GetDataID().ToString(), *DependentObject->GetDataID().ToString()); 
					DependentObject->AddEditDependencyData(ParentObject);
					//ParentObject->AddEditDependencyData(DependentObject);
				}
			}
		}
	}

	// Clear temporary tracking data
	PendingDependencyLinks.Empty();
}



