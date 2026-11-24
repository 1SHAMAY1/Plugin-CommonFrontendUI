#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/CommonFrontendUI_StructsAndEnums.h"
#include "CommonNumericTextBlock.h"
#include "CommonFrontendSettingsRegistry.generated.h"

class UGameUserSettings;

/**
 * Base class for conditions that determine if a setting is editable.
 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class COMMONFRONTENDUI_API UFrontendSettingEditCondition : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Optional: If the condition fails (returns false), use this value to "Force" the setting
	 * to a specific state (e.g., force Resolution to "1920x1080" if Window Mode is Borderless).
	 * If empty, the setting just disables (grays out).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edit Condition")
	FString OverrideValueIfDisabled;

	/** Returns true if the setting should be editable. */
	UFUNCTION(BlueprintNativeEvent, Category = "Frontend Settings")
	bool CanEdit(UGameUserSettings* Settings) const;
	virtual bool CanEdit_Implementation(UGameUserSettings* Settings) const { return true; }
};

/**
 * Edit condition based on the value of another setting.
 */
UCLASS(DisplayName = "Dependency")
class COMMONFRONTENDUI_API UFrontendSettingDependency : public UFrontendSettingEditCondition
{
	GENERATED_BODY()

public:
	/** The name of the setting this depends on (TargetProperty or GetterFunctionName). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dependency")
	FName SourceSettingName;

	/** The expected type of the source setting. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dependency")
	EFrontendSettingType SettingType = EFrontendSettingType::Boolean;

	/** Logic: Returns true if the source setting value matches ONE of these. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dependency")
	TArray<FString> AllowedValues;

	/** Logic: Returns true if the source setting value does NOT match ANY of these. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dependency")
	TArray<FString> DisallowedValues;

	virtual bool CanEdit_Implementation(UGameUserSettings* Settings) const override;
};


/**
 * Allows chaining multiple conditions together (e.g. A AND B, or A OR B).
 */
UCLASS(DisplayName = "Composite Condition")
class COMMONFRONTENDUI_API UFrontendSettingCompositeCondition : public UFrontendSettingEditCondition
{
	GENERATED_BODY()

public:
	/** Logic operator to apply to the list of conditions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Composite")
	EFrontendConditionOperator Operator = EFrontendConditionOperator::And;

	/** List of conditions to evaluate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Composite", meta = (TitleProperty = "SourceSettingName"))
	TArray<TObjectPtr<UFrontendSettingEditCondition>> Conditions;

	virtual bool CanEdit_Implementation(UGameUserSettings* Settings) const override;
};

/**
 * Edit condition that checks build configuration.
 * Use this to disable settings in Editor-only builds or enable only in specific build configurations.
 */
UCLASS(DisplayName = "Build Condition")
class COMMONFRONTENDUI_API UFrontendSettingBuildCondition : public UFrontendSettingEditCondition
{
	GENERATED_BODY()

public:
	/** If true, setting is editable in Shipping builds. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build")
	bool bEditInShipping = true;

	/** If true, setting is editable in Development builds. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build")
	bool bEditInDevelopment = true;

	/** If true, setting is editable in Debug builds. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build")
	bool bEditInDebug = true;

	/** If true, setting is NOT editable in the Editor (PIE/Viewport), regardless of build config. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build")
	bool bDisableInEditor = true;

	virtual bool CanEdit_Implementation(UGameUserSettings* Settings) const override
	{
#if WITH_EDITOR
		if (bDisableInEditor)
		{
			return false;
		}
		return true;
#else
		#if UE_BUILD_SHIPPING
			return bEditInShipping;
		#elif UE_BUILD_DEVELOPMENT
			return bEditInDevelopment;
		#elif UE_BUILD_DEBUG
			return bEditInDebug;
		#else
			return true; 
		#endif
#endif
	}
};

/**
 * Base class for a single setting definition.
 * Allows for polymorphic behavior (Reflected, Manual, Controls, etc.).
 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class COMMONFRONTENDUI_API UFrontendSettingDef : public UObject
{
	GENERATED_BODY()

public:
	/** User-facing name of the setting. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	FText DisplayName;

	/** Description text (localized). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting", meta = (MultiLine = true))
	FText Description;

	/** Optional icon for the setting. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	TSoftObjectPtr<UTexture2D> Icon;
};

/**
 * Base class for all value-based settings (Float, Bool, String, etc.).
 * Provides common binding logic for Properties, Functions, or Audio.
 */
UCLASS(Abstract)
class COMMONFRONTENDUI_API UFrontendSettingValue : public UFrontendSettingDef
{
	GENERATED_BODY()

public:
	/** Name of the property in GameUserSettings to bind to (e.g. "DisplayGamma"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Binding - Property")
	FName TargetProperty;

	/** Optional: Name of a getter function to call. Overrides Property binding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Binding - Manual", AdvancedDisplay)
	FName GetterFunctionName;

	/** Optional: Name of a setter function to call. Overrides Property binding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Binding - Manual", AdvancedDisplay)
	FName SetterFunctionName;

	/** If true, the setting will be applied and saved immediately when changed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior")
	bool bAutoSaveOnChange = false;

	/** Text shown when this setting is disabled/uneditable. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition", meta = (MultiLine = true))
	FText DisabledText;

	/** Optional edit condition. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Condition")
	TObjectPtr<UFrontendSettingEditCondition> EditCondition;

	/** Default value as a string. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
	FString DefaultValue;

	/**
	 * IDs of settings that should be notified when this setting changes.
	 * Use TooltipName matches (GetterFunctionName or TargetProperty).
	 * Example: OverallQuality should list all individual quality settings.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dependencies", AdvancedDisplay)
	TArray<FName> DependentSettingIDs;
};

/**
 * A setting backed by a float or integer value, displayed as a Slider.
 */
UCLASS(DisplayName = "Scalar Setting (Slider)")
class COMMONFRONTENDUI_API UFrontendSettingScalar : public UFrontendSettingValue
{
	GENERATED_BODY()

public:
	/** Minimum value used for user display (e.g., 0.0 for 0%). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values - Display")
	float DisplayMinValue = 0.0f;

	/** Maximum value used for user display (e.g., 100.0 for 100%). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values - Display")
	float DisplayMaxValue = 100.0f;

	/** Minimum value actually sent to the backend/property (e.g., 0.0). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values - Output")
	float OutputMinValue = 0.0f;

	/** Maximum value actually sent to the backend/property (e.g., 1.0). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values - Output")
	float OutputMaxValue = 1.0f;

	/** Step size for the slider. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
	float SliderStepSize = 0.01f;

	/** Number of decimal places to show. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
	int32 NumFractionalDigits = 1;

	/** Format style (e.g., Number, Percentage). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
	ECommonNumericType DisplayFormat = ECommonNumericType::Percentage;

    bool IsInteger() const { return NumFractionalDigits == 0; }
};

/**
 * A setting backed by a boolean, displayed as a Checkbox or ON/OFF switch.
 */
UCLASS(DisplayName = "Boolean Setting")
class COMMONFRONTENDUI_API UFrontendSettingBool : public UFrontendSettingValue
{
	GENERATED_BODY()

public:
	/** Optional: Custom display text for the TRUE (enabled) state (e.g., "ON", "High"). Defaults to "ON" if empty. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
	FText TrueText;

	/** Optional: Custom display text for the FALSE (disabled) state (e.g., "OFF", "Low"). Defaults to "OFF" if empty. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
	FText FalseText;
};

/**
 * A setting backed by a String or Enum, displayed as a Dropdown (ComboBox).
 */
USTRUCT(BlueprintType)
struct FFrontendStringSelection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString DisplayText;
};

/**
 * A setting backed by a String or Enum, displayed as a Dropdown (ComboBox).
 */
UCLASS(DisplayName = "String/Enum Setting")
class COMMONFRONTENDUI_API UFrontendSettingString : public UFrontendSettingValue
{
	GENERATED_BODY()

public:
	/** Text to display when the current value does not match any available option (e.g. "Custom" or "Invalid"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
	FText InvalidDisplayValue;
	
	/** List of available options, preserving order. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
	TArray<FFrontendStringSelection> PossibleValues;

};

/**
 * A setting specifically for Screen Resolution.
 */
UCLASS(DisplayName = "Resolution Setting")
class COMMONFRONTENDUI_API UFrontendSettingResolution : public UFrontendSettingValue
{
	GENERATED_BODY()

};

/**
 * A setting entry that auto-populates input mappings (Controls).
 * Replaces the need for hardcoded "Control" tabs.
 */
UCLASS(DisplayName = "Input Controls")
class COMMONFRONTENDUI_API UFrontendSettingControls : public UFrontendSettingDef
{
	GENERATED_BODY()

public:
	/** The type of input to display (e.g. Keyboard/Mouse or Gamepad). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controls")
	EFrontendControlInputType InputType = EFrontendControlInputType::KeyboardMouse;
};

/**
 * A setting entry that represents a nested tab containing child settings.
 * Used to create hierarchical sub-categories within a main settings tab.
 */
UCLASS(DisplayName = "Nested Tab")
class COMMONFRONTENDUI_API UFrontendSettingNestedTab : public UFrontendSettingDef
{
	GENERATED_BODY()

public:
	/** Unique identifier for this nested tab. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nested Tab")
	FName NestedTabId;

	/** Child settings contained within this nested tab. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Nested Tab", meta = (TitleProperty = "DisplayName"))
	TArray<TObjectPtr<UFrontendSettingDef>> ChildSettings;
};

/**
 * A grouping of nested tabs displayed as a horizontal switcher.
 */
UCLASS(DisplayName = "Nested Tab Collection")
class COMMONFRONTENDUI_API UFrontendSettingNestedTabCollection : public UFrontendSettingDef
{
	GENERATED_BODY()

public:
	/** The nested tabs to display as a switcher. */
	UPROPERTY(EditAnywhere, Instanced, Category = "Nested Tab Collection", meta = (TitleProperty = "DisplayName"))
	TArray<TObjectPtr<UFrontendSettingNestedTab>> Tabs;
};

/**
 * A grouping or category of settings, represented by a collection.
 * This can be used to organize settings within a tab without a separate navigation step.
 */
UCLASS(DisplayName = "Setting Category")
class COMMONFRONTENDUI_API UFrontendSettingCategory : public UFrontendSettingDef
{
	GENERATED_BODY()

public:
	/** Child settings contained within this category. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Category", meta = (TitleProperty = "DisplayName"))
	TArray<TObjectPtr<UFrontendSettingDef>> ChildSettings;
};

/**
 * Registry asset that defines the structure of user settings (Audio, Graphics, etc.).
 * Allows designers to define settings via data instead of code.
 */
UCLASS(BlueprintType, Const)
class COMMONFRONTENDUI_API UCommonFrontendSettingsRegistry : public UDataAsset
{
	GENERATED_BODY()

public:

	UCommonFrontendSettingsRegistry();
	
	/** Collection of setting tabs. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (TitleProperty = "TabName"))
	TArray<FFrontendSettingTab> Tabs;
};
