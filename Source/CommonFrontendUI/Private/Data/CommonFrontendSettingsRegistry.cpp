#include "Data/CommonFrontendSettingsRegistry.h"

#include "CommonNumericTextBlock.h"
#include "GameFramework/GameUserSettings.h"
#include "UObject/UnrealType.h"

UCommonFrontendSettingsRegistry::UCommonFrontendSettingsRegistry()
{

	FFrontendSettingTab VideoTab;
	VideoTab.TabName = NSLOCTEXT("FrontendSettings", "Tab_Video", "Video");
	VideoTab.TabId = FName("Video");
	VideoTab.Description = NSLOCTEXT("FrontendSettings", "Tab_Video_Desc", "<Bold>Video Settings</>:\n<Light>Configure display mode, resolution, and frame rate behavior.</>");

	auto CreatePackageOnlyCondition = [this](FName ConditionName) -> UFrontendSettingBuildCondition*
	{
		UFrontendSettingBuildCondition* Cond = CreateDefaultSubobject<UFrontendSettingBuildCondition>(ConditionName);
		Cond->bDisableInEditor = true;
		Cond->bEditInShipping = true;
		Cond->bEditInDevelopment = true;
		Cond->bEditInDebug = true;
		return Cond;
	};

	UFrontendSettingCategory* DisplayCat = CreateDefaultSubobject<UFrontendSettingCategory>(TEXT("Category_Display"));
	DisplayCat->DisplayName = NSLOCTEXT("FrontendSettings", "Category_Display", "Display");
	DisplayCat->Description = NSLOCTEXT("FrontendSettings", "Desc_Category_Display", "<Bold>Display Options</>:\n<Light>General display configuration.</>");

	UFrontendSettingString* WindowMode = CreateDefaultSubobject<UFrontendSettingString>(TEXT("Setting_WindowMode"));
	WindowMode->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_WindowMode", "Window Mode");
	WindowMode->Description = NSLOCTEXT("FrontendSettings", "WindowMode", "<Bold>Window Mode</>:\n<Light>Selects how the game is displayed: Fullscreen, Windowed, or Borderless Window.</>");
	WindowMode->DisabledText = NSLOCTEXT("FrontendSettings", "Disabled_WindowMode", "<Disabled>This setting can only be adjusted in a packaged build.</>");
	WindowMode->GetterFunctionName = FName("GetFullscreenMode");
	WindowMode->SetterFunctionName = FName("SetFullscreenMode");
	WindowMode->PossibleValues.Add({ TEXT("0"), TEXT("Fullscreen") });
	WindowMode->PossibleValues.Add({ TEXT("1"), TEXT("Borderless Window") });
	WindowMode->PossibleValues.Add({ TEXT("2"), TEXT("Windowed") });
	WindowMode->DefaultValue = TEXT("1");
	WindowMode->EditCondition = CreatePackageOnlyCondition(TEXT("Condition_WindowMode"));
	DisplayCat->ChildSettings.Add(WindowMode);

	UFrontendSettingResolution* Resolution = CreateDefaultSubobject<UFrontendSettingResolution>(TEXT("Setting_Resolution"));
	Resolution->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_Resolution", "Screen Resolution");
	Resolution->Description = NSLOCTEXT("FrontendSettings", "ScreenResolution", "<Bold>Screen Resolution</>:\n<Light>Sets the rendering resolution of the game, which affects visual quality and performance.</>");
	Resolution->DisabledText = NSLOCTEXT("FrontendSettings", "Disabled_Resolution", "<Disabled>This setting can only be adjusted in a packaged build or if not in Borderless Window mode.</>");
	
    UFrontendSettingCompositeCondition* ResComposite = CreateDefaultSubobject<UFrontendSettingCompositeCondition>(TEXT("Condition_Resolution_Composite"));
    ResComposite->Operator = EFrontendConditionOperator::And;

    ResComposite->Conditions.Add(CreatePackageOnlyCondition(TEXT("Condition_Resolution_Pkg")));

    UFrontendSettingDependency* ResDependency = CreateDefaultSubobject<UFrontendSettingDependency>(TEXT("Condition_Resolution_Dep"));
    ResDependency->SourceSettingName = FName("WindowMode");
    ResDependency->SettingType = EFrontendSettingType::Int; 
    ResComposite->Conditions.Add(ResDependency);
    
	Resolution->EditCondition = ResComposite;

	DisplayCat->ChildSettings.Add(Resolution);

	auto AddQualityOptions = [](UFrontendSettingString* Setting)
	{
		Setting->PossibleValues.Add({ TEXT("0"), TEXT("Low") });
		Setting->PossibleValues.Add({ TEXT("1"), TEXT("Medium") });
		Setting->PossibleValues.Add({ TEXT("2"), TEXT("High") });
		Setting->PossibleValues.Add({ TEXT("3"), TEXT("Epic") });
		Setting->PossibleValues.Add({ TEXT("4"), TEXT("Cinematic") });
	};

	UFrontendSettingString* OverallQual = CreateDefaultSubobject<UFrontendSettingString>(TEXT("Setting_OverallQual"));
	OverallQual->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_OverallQual", "Overall Quality");
	OverallQual->Description = NSLOCTEXT("FrontendSettings", "OverallQuality", "<Bold>Graphics Quality</>:\n<Light>Applies a preset level to all graphics options to quickly optimize performance or visual fidelity.</>");
	OverallQual->GetterFunctionName = FName("GetOverallScalabilityLevel");
	OverallQual->SetterFunctionName = FName("SetOverallScalabilityLevel");
	OverallQual->InvalidDisplayValue = NSLOCTEXT("FrontendSettings", "Quality_Custom", "Custom");
	
	AddQualityOptions(OverallQual);
	OverallQual->DefaultValue = TEXT("1");
	OverallQual->bAutoSaveOnChange = true;
	OverallQual->DependentSettingIDs = {
		FName("GetViewDistanceQuality"),
		FName("GetAntiAliasingQuality"),
		FName("GetShadowQuality"),
		FName("GetGlobalIlluminationQuality"),
		FName("GetReflectionQuality"),
		FName("GetPostProcessingQuality"),
		FName("GetTextureQuality"),
		FName("GetVisualEffectQuality"),
		FName("GetResolutionScaleNormalized")
	};
	DisplayCat->ChildSettings.Add(OverallQual);

	UFrontendSettingScalar* ResScale = CreateDefaultSubobject<UFrontendSettingScalar>(TEXT("Setting_ResScale"));
	ResScale->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_ResScale", "Resolution Scale");
	ResScale->Description = NSLOCTEXT("FrontendSettings", "ResolutionScale", "<Bold>Resolution Scale</>:\n<Light>Renders the game at a percentage of the native resolution to improve performance or image sharpness.</>");
	ResScale->GetterFunctionName = FName("GetResolutionScaleNormalized");
	ResScale->SetterFunctionName = FName("SetResolutionScaleNormalized");
	ResScale->DisplayMinValue = 0.0f;
	ResScale->DisplayMaxValue = 100.0f;
	ResScale->OutputMinValue = 0.0f;
	ResScale->OutputMaxValue = 1.0f;
	ResScale->DisplayFormat = ECommonNumericType::Percentage;
	ResScale->SliderStepSize = 0.01f;
	ResScale->NumFractionalDigits = 0;
	ResScale->DefaultValue = TEXT("0.71");
	ResScale->DependentSettingIDs = { FName("GetOverallScalabilityLevel") };
	DisplayCat->ChildSettings.Add(ResScale);

	UFrontendSettingBool* VSync = CreateDefaultSubobject<UFrontendSettingBool>(TEXT("Setting_VSync"));
	VSync->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_VSync", "VSync");
	VSync->Description = NSLOCTEXT("FrontendSettings", "VerticalSync", "<Bold>Vertical Sync (V-Sync)</>:\n<Light>Synchronizes the frame rate with your monitor’s refresh rate to eliminate screen tearing.</>");
	VSync->GetterFunctionName = FName("IsVSyncEnabled");
	VSync->SetterFunctionName = FName("SetVSyncEnabled");
	VSync->DefaultValue = TEXT("false");
	DisplayCat->ChildSettings.Add(VSync);

	UFrontendSettingString* FPSLimit = CreateDefaultSubobject<UFrontendSettingString>(TEXT("Setting_FPSLimit"));
	FPSLimit->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_FPSLimit", "Frame Rate Limit");
	FPSLimit->Description = NSLOCTEXT("FrontendSettings", "FrameRateLimit", "<Bold>Frame Rate Limit</>:\n<Light>Caps the maximum number of frames per second to balance performance and reduce power usage or heat generation.</>");
	FPSLimit->GetterFunctionName = FName("GetFrameRateLimit");
	FPSLimit->SetterFunctionName = FName("SetFrameRateLimit");
	FPSLimit->PossibleValues.Add({ TEXT("30.0"), TEXT("30 FPS") });
	FPSLimit->PossibleValues.Add({ TEXT("60.0"), TEXT("60 FPS") });
	FPSLimit->PossibleValues.Add({ TEXT("90.0"), TEXT("90 FPS") });
	FPSLimit->PossibleValues.Add({ TEXT("120.0"), TEXT("120 FPS") });
	FPSLimit->PossibleValues.Add({ TEXT("144.0"), TEXT("144 FPS") });
	FPSLimit->PossibleValues.Add({ TEXT("165.0"), TEXT("165 FPS") });
	FPSLimit->PossibleValues.Add({ TEXT("180.0"), TEXT("180 FPS") });
	FPSLimit->PossibleValues.Add({ TEXT("240.0"), TEXT("240 FPS") });
	FPSLimit->PossibleValues.Add({ TEXT("0.0"), TEXT("Unlimited") });
	FPSLimit->DefaultValue = TEXT("0.0");
	DisplayCat->ChildSettings.Add(FPSLimit);

	VideoTab.Settings.Add(DisplayCat);

	UFrontendSettingCategory* ColorsCat = CreateDefaultSubobject<UFrontendSettingCategory>(TEXT("Category_Colors"));
	ColorsCat->DisplayName = NSLOCTEXT("FrontendSettings", "Category_Colors", "Colors");
	ColorsCat->Description = NSLOCTEXT("FrontendSettings", "Desc_Category_Colors", "<Bold>Color Settings</>:\n<Light>Adjust brightness and HDR settings.</>");

	UFrontendSettingScalar* Brightness = CreateDefaultSubobject<UFrontendSettingScalar>(TEXT("Setting_Brightness"));
	Brightness->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_Brightness", "Brightness");
	Brightness->Description = NSLOCTEXT("FrontendSettings", "DisplayGamma", "<Bold>Display Gamma</>:\n<Light>Adjusts the brightness and contrast of the display for improved visibility in various lighting conditions.</>");
	Brightness->GetterFunctionName = FName("GetCurrentDisplayGamma");
	Brightness->SetterFunctionName = FName("SetCurrentDisplayGamma");
	Brightness->DisplayMinValue = 0.0f;
	Brightness->DisplayMaxValue = 100.0f; 
	Brightness->OutputMinValue = 1.7f; 
	Brightness->OutputMaxValue = 2.7f;
	Brightness->DisplayFormat = ECommonNumericType::Percentage;
	Brightness->SliderStepSize = 0.01f; 
	Brightness->NumFractionalDigits = 0;
	Brightness->DefaultValue = TEXT("2.2");
	ColorsCat->ChildSettings.Add(Brightness);
	
	UFrontendSettingBool* HDR = CreateDefaultSubobject<UFrontendSettingBool>(TEXT("Setting_HDR"));
	HDR->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_HDR", "HDR");
	HDR->Description = NSLOCTEXT("FrontendSettings", "Desc_HDR", "<Bold>High Dynamic Range</>:\n<Light>Enables HDR output if supported.</>");
	HDR->GetterFunctionName = FName("IsHDREnabled");
	HDR->SetterFunctionName = FName("EnableHDRDisplayOutput");
	HDR->DefaultValue = TEXT("false");
	ColorsCat->ChildSettings.Add(HDR);

	VideoTab.Settings.Add(ColorsCat);


	UFrontendSettingCategory* GraphicsCat = CreateDefaultSubobject<UFrontendSettingCategory>(TEXT("Category_Graphics"));
	GraphicsCat->DisplayName = NSLOCTEXT("FrontendSettings", "Category_Graphics", "Graphics");
	GraphicsCat->Description = NSLOCTEXT("FrontendSettings", "Desc_Category_Graphics", "<Bold>Advanced Graphics</>:\n<Light>Fine-tune detailed graphics settings.</>");

	UFrontendSettingString* ViewDist = CreateDefaultSubobject<UFrontendSettingString>(TEXT("Setting_ViewDist"));
	ViewDist->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_ViewDist", "View Distance");
	ViewDist->Description = NSLOCTEXT("FrontendSettings", "ViewDistance", "<Bold>View Distance</>:\n<Light>Determines how far into the distance the game renders objects and geometry.</>");
	ViewDist->GetterFunctionName = FName("GetViewDistanceQuality");
	ViewDist->SetterFunctionName = FName("SetViewDistanceQuality");
	AddQualityOptions(ViewDist);
	ViewDist->DefaultValue = TEXT("1");
	ViewDist->DependentSettingIDs = { FName("GetOverallScalabilityLevel") };
	GraphicsCat->ChildSettings.Add(ViewDist);

	UFrontendSettingString* AAQual = CreateDefaultSubobject<UFrontendSettingString>(TEXT("Setting_AA"));
	AAQual->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_AA", "Anti-Aliasing");
	AAQual->Description = NSLOCTEXT("FrontendSettings", "AntiAliasing", "<Bold>Anti-Aliasing</>:\n<Light>Smooths jagged edges of objects to reduce visual artifacts and improve clarity.</>");
	AAQual->GetterFunctionName = FName("GetAntiAliasingQuality");
	AAQual->SetterFunctionName = FName("SetAntiAliasingQuality");
	AddQualityOptions(AAQual);
	AAQual->DefaultValue = TEXT("1");
	AAQual->DependentSettingIDs = { FName("GetOverallScalabilityLevel") };
	GraphicsCat->ChildSettings.Add(AAQual);

	UFrontendSettingString* ShadowQual = CreateDefaultSubobject<UFrontendSettingString>(TEXT("Setting_Shadow"));
	ShadowQual->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_Shadow", "Shadow Quality");
	ShadowQual->Description = NSLOCTEXT("FrontendSettings", "ShadowQuality", "<Bold>Shadow Quality</>:\n<Light>Adjusts the resolution and sharpness of shadows cast by objects and characters.</>");
	ShadowQual->GetterFunctionName = FName("GetShadowQuality");
	ShadowQual->SetterFunctionName = FName("SetShadowQuality");
	AddQualityOptions(ShadowQual);
	ShadowQual->DefaultValue = TEXT("1");
	ShadowQual->DependentSettingIDs = { FName("GetOverallScalabilityLevel") };
	GraphicsCat->ChildSettings.Add(ShadowQual);

	UFrontendSettingString* GIQual = CreateDefaultSubobject<UFrontendSettingString>(TEXT("Setting_GI"));
	GIQual->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_GI", "Global Illumination");
	GIQual->Description = NSLOCTEXT("FrontendSettings", "GlobalIlluminationQuality", "<Bold>Global Illumination</>:\n<Light>Controls the realism and quality of ambient lighting and indirect light bounces in the environment.</>");
	GIQual->GetterFunctionName = FName("GetGlobalIlluminationQuality");
	GIQual->SetterFunctionName = FName("SetGlobalIlluminationQuality");
	AddQualityOptions(GIQual);
	GIQual->DefaultValue = TEXT("1");
	GIQual->DependentSettingIDs = { FName("GetOverallScalabilityLevel") };
	GraphicsCat->ChildSettings.Add(GIQual);

	UFrontendSettingString* ReflQual = CreateDefaultSubobject<UFrontendSettingString>(TEXT("Setting_Reflection"));
	ReflQual->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_Reflection", "Reflection Quality");
	ReflQual->Description = NSLOCTEXT("FrontendSettings", "ReflectionQuality", "<Bold>Reflection Quality</>:\n<Light>Adjusts the quality and accuracy of reflections on surfaces like water, glass, and metal.</>");
	ReflQual->GetterFunctionName = FName("GetReflectionQuality");
	ReflQual->SetterFunctionName = FName("SetReflectionQuality");
	AddQualityOptions(ReflQual);
	ReflQual->DefaultValue = TEXT("1");
	ReflQual->DependentSettingIDs = { FName("GetOverallScalabilityLevel") };
	GraphicsCat->ChildSettings.Add(ReflQual);

	UFrontendSettingString* PPQual = CreateDefaultSubobject<UFrontendSettingString>(TEXT("Setting_PostProcess"));
	PPQual->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_PostProcess", "Post-Processing");
	PPQual->Description = NSLOCTEXT("FrontendSettings", "PostProcessingQuality", "<Bold>Post-Processing</>:\n<Light>Controls effects such as bloom, lens flare, depth of field, and color grading to enhance visuals.</>");
	PPQual->GetterFunctionName = FName("GetPostProcessingQuality");
	PPQual->SetterFunctionName = FName("SetPostProcessingQuality");
	AddQualityOptions(PPQual);
	PPQual->DefaultValue = TEXT("1");
	PPQual->DependentSettingIDs = { FName("GetOverallScalabilityLevel") };
	GraphicsCat->ChildSettings.Add(PPQual);

	UFrontendSettingString* TexQual = CreateDefaultSubobject<UFrontendSettingString>(TEXT("Setting_Textures"));
	TexQual->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_Textures", "Texture Quality");
	TexQual->Description = NSLOCTEXT("FrontendSettings", "TextureQuality", "<Bold>Texture Quality</>:\n<Light>Sets the resolution and detail of surface textures on characters, environments, and props.</>");
	TexQual->GetterFunctionName = FName("GetTextureQuality");
	TexQual->SetterFunctionName = FName("SetTextureQuality");
	AddQualityOptions(TexQual);
	TexQual->DefaultValue = TEXT("1");
	TexQual->DependentSettingIDs = { FName("GetOverallScalabilityLevel") };
	GraphicsCat->ChildSettings.Add(TexQual);

	UFrontendSettingString* VFXQual = CreateDefaultSubobject<UFrontendSettingString>(TEXT("Setting_VFX"));
	VFXQual->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_VFX", "Visual Effects");
	VFXQual->Description = NSLOCTEXT("FrontendSettings", "VisualEffectQuality", "<Bold>Visual Effects</>:\n<Light>Controls the fidelity and detail of in-game particle effects such as fire, smoke, and weather.</>");
	VFXQual->GetterFunctionName = FName("GetVisualEffectQuality");
	VFXQual->SetterFunctionName = FName("SetVisualEffectQuality");
	AddQualityOptions(VFXQual);
	VFXQual->DefaultValue = TEXT("1");
	VFXQual->DependentSettingIDs = { FName("GetOverallScalabilityLevel") };
	GraphicsCat->ChildSettings.Add(VFXQual);

	VideoTab.Settings.Add(GraphicsCat);

	Tabs.Add(VideoTab);


	FFrontendSettingTab AudioTab;
	AudioTab.TabName = NSLOCTEXT("FrontendSettings", "Tab_Audio", "Audio");
	AudioTab.TabId = FName("Audio");
	AudioTab.Description = NSLOCTEXT("FrontendSettings", "Tab_Audio_Desc", "<Bold>Audio Settings</>:\n<Light>Control volume levels for master, music, SFX, and UI.</>");

	UFrontendSettingScalar* MasterVol = CreateDefaultSubobject<UFrontendSettingScalar>(TEXT("Setting_MasterVol"));
	MasterVol->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_MasterVol", "Overall Volume");
	MasterVol->Description = NSLOCTEXT("FrontendSettings", "OverallVolume", "<Bold>Overall Volume</>:\n<Light>Controls the master volume level for all in-game audio.</>");
	MasterVol->GetterFunctionName = FName("GetMasterVolume");
	MasterVol->SetterFunctionName = FName("SetMasterVolume");
	MasterVol->DisplayMinValue = 0.0f;
	MasterVol->DisplayMaxValue = 100.0f;
	MasterVol->OutputMinValue = 0.0f;
	MasterVol->OutputMaxValue = 1.0f;
	MasterVol->DisplayFormat = ECommonNumericType::Number;
	MasterVol->SliderStepSize = 1.0f;
	MasterVol->NumFractionalDigits = 0;
	MasterVol->DefaultValue = TEXT("1.0");
	MasterVol->bAutoSaveOnChange = true;
	AudioTab.Settings.Add(MasterVol);

	UFrontendSettingScalar* MusicVol = CreateDefaultSubobject<UFrontendSettingScalar>(TEXT("Setting_MusicVol"));
	MusicVol->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_MusicVol", "Music Volume");
	MusicVol->Description = NSLOCTEXT("FrontendSettings", "MusicVolume", "<Bold>Music Volume</>:\n<Light>Adjusts the volume level of background music.</>");
	MusicVol->GetterFunctionName = FName("GetMusicVolume");
	MusicVol->SetterFunctionName = FName("SetMusicVolume");
	MusicVol->DisplayMinValue = 0.0f;
	MusicVol->DisplayMaxValue = 100.0f;
	MusicVol->OutputMinValue = 0.0f;
	MusicVol->OutputMaxValue = 1.0f;
	MusicVol->DisplayFormat = ECommonNumericType::Number;
	MusicVol->SliderStepSize = 1.0f;
	MusicVol->NumFractionalDigits = 0;
	MusicVol->DefaultValue = TEXT("0.8");
	MusicVol->bAutoSaveOnChange = true;
	AudioTab.Settings.Add(MusicVol);

	UFrontendSettingScalar* SFXVol = CreateDefaultSubobject<UFrontendSettingScalar>(TEXT("Setting_SFXVol"));
	SFXVol->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_SFXVol", "Sound Effects Volume");
	SFXVol->Description = NSLOCTEXT("FrontendSettings", "SoundFXVolume", "<Bold>Sound Effects Volume</>:\n<Light>Sets the volume level for all in-game sound effects such as footsteps, gunshots, and explosions.</>");
	SFXVol->GetterFunctionName = FName("GetSFXVolume");
	SFXVol->SetterFunctionName = FName("SetSFXVolume");
	SFXVol->DisplayMinValue = 0.0f;
	SFXVol->DisplayMaxValue = 100.0f;
	SFXVol->OutputMinValue = 0.0f;
	SFXVol->OutputMaxValue = 1.0f;
	SFXVol->DisplayFormat = ECommonNumericType::Number;
	SFXVol->SliderStepSize = 1.0f;
	SFXVol->NumFractionalDigits = 0;
	SFXVol->DefaultValue = TEXT("0.8");
	SFXVol->bAutoSaveOnChange = true;
	AudioTab.Settings.Add(SFXVol);

	UFrontendSettingScalar* UIVol = CreateDefaultSubobject<UFrontendSettingScalar>(TEXT("Setting_UIVol"));
	UIVol->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_UIVol", "UI Volume");
	UIVol->Description = NSLOCTEXT("FrontendSettings", "UIVolume", "<Bold>UI Volume</>:\n<Light>Adjusts the volume of interface sounds such as button clicks, menu transitions, and notifications.</>");
	UIVol->GetterFunctionName = FName("GetUIVolume");
	UIVol->SetterFunctionName = FName("SetUIVolume");
	UIVol->DisplayMinValue = 0.0f;
	UIVol->DisplayMaxValue = 100.0f;
	UIVol->OutputMinValue = 0.0f;
	UIVol->OutputMaxValue = 1.0f;
	UIVol->DisplayFormat = ECommonNumericType::Number;
	UIVol->SliderStepSize = 1.0f;
	UIVol->NumFractionalDigits = 0;
	UIVol->DefaultValue = TEXT("1.0");
	UIVol->bAutoSaveOnChange = true;
	AudioTab.Settings.Add(UIVol);

	Tabs.Add(AudioTab);

	FFrontendSettingTab ControlsTab;
	ControlsTab.TabName = NSLOCTEXT("FrontendSettings", "Tab_Controls", "Controls");
	ControlsTab.TabId = FName("Controls");
	ControlsTab.Description = NSLOCTEXT("FrontendSettings", "Tab_Controls_Desc", "<Bold>Controls</>:\n<Light>View and customize input bindings for Keyboard & Mouse and Gamepad.</>");

	UFrontendSettingNestedTab* KBMNested = CreateDefaultSubobject<UFrontendSettingNestedTab>(TEXT("Nested_KBM"));
	KBMNested->DisplayName = NSLOCTEXT("FrontendSettings", "Nested_KBM", "Keyboard & Mouse");
	KBMNested->NestedTabId = FName("KeyboardMouse");
	KBMNested->Description = NSLOCTEXT("FrontendSettings", "Desc_Nested_KBM", "<Bold>Keyboard & Mouse</>:\n<Light>Customize key bindings.</>");
	
	UFrontendSettingControls* KBMControls = CreateDefaultSubobject<UFrontendSettingControls>(TEXT("Setting_KBM"));
	KBMControls->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_KBM", "Bindings");
	KBMControls->Description = NSLOCTEXT("FrontendSettings", "Desc_KBM", "<Bold>Bindings</>:\n<Light>List of remappable keys.</>");
	KBMControls->InputType = EFrontendControlInputType::KeyboardMouse;
	
	KBMNested->ChildSettings.Add(KBMControls);

	UFrontendSettingNestedTab* GamepadNested = CreateDefaultSubobject<UFrontendSettingNestedTab>(TEXT("Nested_Gamepad"));
	GamepadNested->DisplayName = NSLOCTEXT("FrontendSettings", "Nested_Gamepad", "Gamepad");
	GamepadNested->NestedTabId = FName("Gamepad");
	GamepadNested->Description = NSLOCTEXT("FrontendSettings", "Desc_Nested_Gamepad", "<Bold>Gamepad</>:\n<Light>Customize button bindings.</>");

	UFrontendSettingControls* GamepadControls = CreateDefaultSubobject<UFrontendSettingControls>(TEXT("Setting_Gamepad"));
	GamepadControls->DisplayName = NSLOCTEXT("FrontendSettings", "Setting_Gamepad", "Bindings");
	GamepadControls->Description = NSLOCTEXT("FrontendSettings", "Desc_Gamepad", "<Bold>Bindings</>:\n<Light>List of remappable buttons.</>");
	GamepadControls->InputType = EFrontendControlInputType::Gamepad;

	GamepadNested->ChildSettings.Add(GamepadControls);

	UFrontendSettingNestedTabCollection* ControlsCollection = CreateDefaultSubobject<UFrontendSettingNestedTabCollection>(TEXT("Collection_Controls"));
	ControlsCollection->DisplayName = NSLOCTEXT("FrontendSettings", "Collection_Controls", "Control Mode");
	ControlsCollection->Description = NSLOCTEXT("FrontendSettings", "Desc_Collection_Controls", "<Bold>Control Mode</>:\n<Light>Select input method.</>");
	ControlsCollection->Tabs.Add(KBMNested);
	ControlsCollection->Tabs.Add(GamepadNested);

	ControlsTab.Settings.Add(ControlsCollection);

	Tabs.Add(ControlsTab);
}

bool UFrontendSettingDependency::CanEdit_Implementation(UGameUserSettings* Settings) const
{
	if (!Settings || SourceSettingName.IsNone())
	{
		return true;
	}

	FString CurrentValueStr;
	bool bFound = false;

	// 1. Try to find a function (Getter)
	if (UFunction* Function = Settings->FindFunction(SourceSettingName))
	{
		// We only support simple getters with no params and a return value
		if (Function->NumParms == 1 && Function->GetReturnProperty())
		{
			uint8* Buffer = (uint8*)FMemory_Alloca(Function->ParmsSize);
			FMemory::Memzero(Buffer, Function->ParmsSize);

			Settings->ProcessEvent(Function, Buffer);

			FProperty* ReturnProp = Function->GetReturnProperty();
			if (ReturnProp)
			{
				void* ReturnValueAddress = Buffer + ReturnProp->GetOffset_ForInternal();
				if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(ReturnProp))
				{
					float Val = FloatProp->GetFloatingPointPropertyValue(ReturnValueAddress);
					CurrentValueStr = FString::SanitizeFloat(Val);
					bFound = true;
				}
				else if (const FIntProperty* IntProp = CastField<FIntProperty>(ReturnProp))
				{
					int32 Val = IntProp->GetSignedIntPropertyValue(ReturnValueAddress);
					CurrentValueStr = FString::FromInt(Val);
					bFound = true;
				}
				else if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(ReturnProp))
				{
					bool Val = BoolProp->GetPropertyValue(ReturnValueAddress);
					// Normalize bool string to lowercase for consistency if needed, assuming "true"/"false"
					CurrentValueStr = Val ? TEXT("true") : TEXT("false");
					bFound = true;
				}
				else if (const FByteProperty* ByteProp = CastField<FByteProperty>(ReturnProp))
				{
					uint8 Val = ByteProp->GetPropertyValue(ReturnValueAddress);
					CurrentValueStr = FString::FromInt(Val);
					bFound = true;
				}
				else if (const FEnumProperty* EnumProp = CastField<FEnumProperty>(ReturnProp))
				{
					void* EnumValueAddress = ReturnValueAddress;
					int64 Val = EnumProp->GetUnderlyingProperty()->GetSignedIntPropertyValue(EnumValueAddress);
					CurrentValueStr = FString::FromInt((int32)Val);
					bFound = true;
				}
			}
		}
	}
	
	// 2. Try to find a property
	if (!bFound)
	{
		if (FProperty* Prop = Settings->GetClass()->FindPropertyByName(SourceSettingName))
		{
			void* ValuePtr = Prop->ContainerPtrToValuePtr<void>(Settings);

			if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(Prop))
			{
				float Val = FloatProp->GetFloatingPointPropertyValue(ValuePtr);
				CurrentValueStr = FString::SanitizeFloat(Val);
				bFound = true;
			}
			else if (const FIntProperty* IntProp = CastField<FIntProperty>(Prop))
			{
				int32 Val = IntProp->GetSignedIntPropertyValue(ValuePtr);
				CurrentValueStr = FString::FromInt(Val);
				bFound = true;
			}
			else if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(Prop))
			{
				bool Val = BoolProp->GetPropertyValue(ValuePtr);
				CurrentValueStr = Val ? TEXT("true") : TEXT("false");
				bFound = true;
			}
			else if (const FByteProperty* ByteProp = CastField<FByteProperty>(Prop))
			{
				uint8 Val = ByteProp->GetPropertyValue(ValuePtr);
				CurrentValueStr = FString::FromInt(Val);
				bFound = true;
			}
			else if (const FEnumProperty* EnumProp = CastField<FEnumProperty>(Prop))
			{
				int64 Val = EnumProp->GetUnderlyingProperty()->GetSignedIntPropertyValue(ValuePtr);
				CurrentValueStr = FString::FromInt((int32)Val);
				bFound = true;
			}
		}
	}

	if (!bFound)
	{
		// If we can't find the source, we can't evaluate dependencies, so default to true or false?
		// Defaulting to true (editable) is safer to avoid locking users out.
		return true;
	}

	// Helper to handle loose string matching
	auto CheckMatch = [&](const FString& CheckValue) -> bool
	{
		if (CheckValue == CurrentValueStr) return true;
		
		// Handle bool "1" == "true"
		if (CheckValue == TEXT("1") && CurrentValueStr == TEXT("true")) return true;
		if (CheckValue == TEXT("0") && CurrentValueStr == TEXT("false")) return true;
		if (CheckValue == TEXT("true") && CurrentValueStr == TEXT("1")) return true;
		if (CheckValue == TEXT("false") && CurrentValueStr == TEXT("0")) return true;

		return false;
	};

	// Check Allowed
	if (AllowedValues.Num() > 0)
	{
		bool bMatch = false;
		for (const FString& Allowed : AllowedValues)
		{
			if (CheckMatch(Allowed))
			{
				bMatch = true;
				break;
			}
		}
		if (!bMatch) return false;
	}

	// Check Disallowed
	if (DisallowedValues.Num() > 0)
	{
		for (const FString& Disallowed : DisallowedValues)
		{
			if (CheckMatch(Disallowed))
			{
				return false;
			}
		}
	}

	return true;
}

bool UFrontendSettingCompositeCondition::CanEdit_Implementation(UGameUserSettings* Settings) const
{
	if (Conditions.Num() == 0)
	{
		return true; 
	}

	if (Operator == EFrontendConditionOperator::And)
	{
		for (const auto& Cond : Conditions)
		{
			if (Cond && !Cond->CanEdit(Settings))
			{
				return false;
			}
		}
		return true;
	}
	else // Or
	{
		for (const auto& Cond : Conditions)
		{
			if (Cond && Cond->CanEdit(Settings))
			{
				return true;
			}
		}
		// If none passed, return false
		return false;
	}
}
