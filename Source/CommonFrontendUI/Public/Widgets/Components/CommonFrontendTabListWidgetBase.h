#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "CommonFrontendTabListWidgetBase.generated.h"

class UCommonFrontendButtonBase;

/**
 * A frontend-specific tab list widget used in the Common Frontend UI framework.
 * 
 * Extends UCommonTabListWidgetBase to support custom tab registration and editor preview features.
 * This widget allows dynamic tab setup using data-driven labels and button types, and is intended for modular tab-based UIs.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class COMMONFRONTENDUI_API UCommonFrontendTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:

	/**
	 * Registers a tab at runtime with the specified tab ID and display name.
	 * 
	 * @param InTabID           Unique name identifier for the tab.
	 * @param InTabDisplayName Text to be displayed on the tab button.
	 */
	UFUNCTION(BlueprintCallable, Category = "Frontend Tabs")
	void RequestRegisterTab(const FName& InTabID, const FText& InTabDisplayName);

	UFUNCTION(BlueprintCallable, Category = "Frontend Tabs")
	void SelectNextTab();

	UFUNCTION(BlueprintCallable, Category = "Frontend Tabs")
	void SelectPreviousTab();

private:

#if WITH_EDITOR
	/**
	 * Validates widget defaults during compilation in the UMG editor.
	 *
	 * @param CompileLog Compiler log used to report any configuration issues.
	 */
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif

	/** Number of dummy tabs to show in the editor preview (for layout debugging only). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Tab List Settings", meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "10"))
	int32 DebugEditorPreviewTabCount = 3;

	/** The button widget class used to render each tab in the list. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Tab List Settings", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCommonFrontendButtonBase> TabButtonEntryWidgetClass;
};
