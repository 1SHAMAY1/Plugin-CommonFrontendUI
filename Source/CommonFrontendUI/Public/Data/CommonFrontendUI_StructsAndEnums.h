#pragma once

class UTexture2D;

#include "CommonFrontendUI_StructsAndEnums.generated.h"


/** Reasons why a piece of list data might have been modified. */
UENUM(BlueprintType)
enum class EOptionsListDataModifyReason : uint8
{
	DirectlyModified,
	DependencyModified,
	ResetToDefault,
	SetFromBuffer,
	CleanUp
};

/** Status of an asynchronous widget push operation. */
UENUM(BlueprintType)
enum EAsyncPushActionState
{
	OnCreated,
	OnPushed
};

/**
 * Defines the type of a frontend setting.
 */
UENUM(BlueprintType)
enum class EFrontendSettingType : uint8
{
	Boolean,
	Float,
	Int,
	String,
	Enum,
	Resolution
};

class UFrontendSettingDef;


/**
 * Defines the type of input to display (e.g. Keyboard/Mouse or Gamepad).
 */
UENUM(BlueprintType)
enum class EFrontendControlInputType : uint8
{
	KeyboardMouse,
	Gamepad,
	Touch
};

/** Operator choices for composite conditions. */
UENUM(BlueprintType)
enum class EFrontendConditionOperator : uint8
{
	And,
	Or
};

/**
 * Represents a tab (category) in the settings menu.
 */
USTRUCT(BlueprintType)
struct FFrontendSettingTab
{
	GENERATED_BODY()

	/** Unique identifier for the tab. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tab")
	FName TabId;

	/** Display name of the tab. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tab")
	FText TabName;

	/** Description/Tooltip for the tab. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tab")
	FText Description;

	/** Icon for the tab. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tab")
	TSoftObjectPtr<UTexture2D> TabIcon;

	/** List of setting definitions (Reflected, Manual, or Controls). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Tab", meta = (TitleProperty = "DisplayName"))
	TArray<TObjectPtr<UFrontendSettingDef>> Settings;
};


/**
 * Defines a binding between a GameUserSettings property (Volume) and a Sound Class.
 */
USTRUCT(BlueprintType)
struct FCommonFrontendAudioBinding
{
	GENERATED_BODY()

	/** The name of the property or function in GameUserSettings (e.g., "MasterVolume"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio Binding")
	FName VolumeSettingName;

	/** The Sound Class to apply this volume to. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio Binding", meta = (AllowedClasses = "/Script/Engine.SoundClass"))
	FSoftObjectPath SoundClass;

	/** Optional: Specific Sound Mix to use for this class. If empty, uses the MasterSoundMix. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio Binding", meta = (AllowedClasses = "/Script/Engine.SoundMix"))
	FSoftObjectPath SoundMixOverride;
    
	/** Name of the getter function in GameUserSettings. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio Binding")
	FName GetterFunctionName;

	/** Name of the setter function in GameUserSettings. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio Binding")
	FName SetterFunctionName;
};

/**
 * Lightweight descriptor used to pass logic and metadata for setting edit conditions.
 * Supports functional predicates, rich-text reasons for disabling, and forced fallback values.
 */
USTRUCT()
struct FOptionsDataEditConditionDescriptor
{
	GENERATED_BODY()

public:
	/** Sets the function used to evaluate whether the condition is met. */
	void SetEditConditionFunc(TFunction<bool()> InEditConditionFunc)
	{
		EditConditionFunc = InEditConditionFunc;
	}

	bool IsValid() const
	{
		return EditConditionFunc != nullptr;
	}

	bool IsEditConditionMet() const
	{		
		if (IsValid())
		{
			return EditConditionFunc();
		}

		return true;
	}

	FString GetDisabledRichReason() const
	{
		return DisabledRichReason;
	}

	void SetDisabledRichReason(const FString& InRichReason)
	{
		DisabledRichReason = InRichReason;
	}

	bool HasForcedStringValue() const
	{
		return DisabledForcedStringValue.IsSet();
	}

	FString GetDisabledForcedStringValue() const
	{
		return DisabledForcedStringValue.GetValue();
	}

	void SetDisabledForcedStringValue(const FString& InForcedValue)
	{
		DisabledForcedStringValue = InForcedValue;
	}

private:
	TFunction<bool()> EditConditionFunc;
	FString DisabledRichReason;
	TOptional<FString> DisabledForcedStringValue;
};