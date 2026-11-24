#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Widgets/DataObject/ListDataObjectBase.h"
#include "ListDataObjectKeyRemap.generated.h"

class UEnhancedPlayerMappableKeyProfile;
class UEnhancedInputUserSettings;

/**
 * UListDataObjectKeyRemap
 *
 * A frontend list data object that represents a key remapping entry for a single player input binding.
 * Supports reading, binding, and resetting a remappable key for a specific player key mapping within an input profile.
 */
UCLASS()
class COMMONFRONTENDUI_API UListDataObjectKeyRemap : public UListDataObjectBase
{
	GENERATED_BODY()

public:

	/**
	 * Initializes this remap data object with input settings, key profile, desired input type, and the target key mapping.
	 *
	 * @param InOwningInputUserSettings      Reference to the input user settings.
	 * @param InKeyProfile                   Reference to the player's mappable key profile.
	 * @param InDesiredInputKeyType          Specifies the input type (keyboard, gamepad, etc.).
	 * @param InOwningPlayerKeyMapping       The key mapping this data object represents.
	 */
	void InitKeyRemapData(UEnhancedInputUserSettings* InOwningInputUserSettings, UEnhancedPlayerMappableKeyProfile* InKeyProfile, ECommonInputType InDesiredInputKeyType, const FPlayerKeyMapping& InOwningPlayerKeyMapping);

	/** Returns an icon representing the current input key mapping. */
	FSlateBrush GetIconFromCurrentKey() const;

	/**
	 * Binds a new input key to this mapping, replacing the current key.
	 *
	 * @param InNewKey   The new key to bind.
	 */
	void BindNewInputKey(const FKey& InNewKey);

	/** Checks whether the current key mapping matches the default value. */
	virtual bool HasDefaultValue() const override;

	/** Returns true if the key mapping can be reset to its default value. */
	virtual bool CanResetBackToDefaultValue() const override;

	/** Attempts to reset the current key mapping to its default state. */
	virtual bool TryResetBackToDefaultValue() override;

	/** Returns the desired input key type for this remap (e.g., keyboard or gamepad). */
	FORCEINLINE ECommonInputType GetDesiredInputKeyType() const { return CachedDesiredInputKeyType; }

private:

	/** Retrieves a pointer to the player’s actual key mapping this object represents. */
	FPlayerKeyMapping* GetOwningKeyMapping() const;

	/** Cached reference to the owning input user settings. */
	UPROPERTY(Transient)
	UEnhancedInputUserSettings* CachedOwningInputUserSettings;

	/** Cached reference to the player's input key profile. */
	UPROPERTY(Transient)
	UEnhancedPlayerMappableKeyProfile* CachedOwningKeyProfile;

	/** Cached input type (keyboard, gamepad, etc.) used to filter or identify the remapping target. */
	ECommonInputType CachedDesiredInputKeyType;

	/** Cached mapping name used to retrieve the correct key mapping. */
	FName CachedOwningMappingName;

	/** Cached slot (primary/secondary) used to identify the correct key binding within a mapping. */
	EPlayerMappableKeySlot CachedOwningMappableKeySlot;
};
