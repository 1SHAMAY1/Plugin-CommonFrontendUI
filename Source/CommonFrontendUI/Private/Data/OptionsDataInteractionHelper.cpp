#include "Data/OptionsDataInteractionHelper.h"

#include "Utility/CommonFrontendLibrary.h"
#include "Utility/CommonFrontendGameUserSettings.h"

FOptionsDataInteractionHelper::FOptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath)
	: CachedFunctionOrPropertyPath(InSetterOrGetterFuncPath) 
{
	CachedWeakGameUserSettings = UCommonFrontendLibrary::GetCommonFrontendGameUserSettings();
}

FOptionsDataInteractionHelper::FOptionsDataInteractionHelper(FGetterFunction InGetter, FSetterFunction InSetter)
	: CachedGetterFunc(InGetter)
	, CachedSetterFunc(InSetter)
{
}

FString FOptionsDataInteractionHelper::GetValueAsString() const
{
	if (CachedGetterFunc)
	{
		return CachedGetterFunc();
	}

	UCommonFrontendGameUserSettings* Settings = CachedWeakGameUserSettings.Get();
	if (!Settings)
	{
		return FString();
	}

	// First, try to find and call a getter function
	if (UFunction* Function = Settings->FindFunction(FName(*CachedFunctionOrPropertyPath)))
	{
		if (Function->NumParms >= 1 && Function->GetReturnProperty())
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
					return FString::SanitizeFloat(FloatProp->GetFloatingPointPropertyValue(ReturnValueAddress));
				}
				else if (const FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(ReturnProp))
				{
					return FString::SanitizeFloat(DoubleProp->GetFloatingPointPropertyValue(ReturnValueAddress));
				}
				else if (const FIntProperty* IntProp = CastField<FIntProperty>(ReturnProp))
				{
					return FString::FromInt(IntProp->GetSignedIntPropertyValue(ReturnValueAddress));
				}
				else if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(ReturnProp))
				{
					return BoolProp->GetPropertyValue(ReturnValueAddress) ? TEXT("true") : TEXT("false");
				}
				else if (const FByteProperty* ByteProp = CastField<FByteProperty>(ReturnProp))
				{
					return FString::FromInt(ByteProp->GetPropertyValue(ReturnValueAddress));
				}
				else if (const FEnumProperty* EnumProp = CastField<FEnumProperty>(ReturnProp))
				{
					int64 Val = EnumProp->GetUnderlyingProperty()->GetSignedIntPropertyValue(ReturnValueAddress);
					return FString::FromInt((int32)Val);
				}
				else if (const FStrProperty* StrProp = CastField<FStrProperty>(ReturnProp))
				{
					return StrProp->GetPropertyValue(ReturnValueAddress);
				}
			}
		}
	}

	// Fallback: try property path
	FString OutStringValue;
	FCachedPropertyPath PropertyPath(CachedFunctionOrPropertyPath);
	PropertyPathHelpers::GetPropertyValueAsString(Settings, PropertyPath, OutStringValue);
	return OutStringValue;
}

void FOptionsDataInteractionHelper::SetValueFromString(const FString& InStringValue)
{
	if (CachedSetterFunc)
	{
		CachedSetterFunc(InStringValue);
		return;
	}

	UCommonFrontendGameUserSettings* Settings = CachedWeakGameUserSettings.Get();
	if (!Settings)
	{
		return;
	}

	// First, try to find and call a setter function
	if (UFunction* Function = Settings->FindFunction(FName(*CachedFunctionOrPropertyPath)))
	{
		// Setter functions typically have 1 parameter (the value) and no return
		if (Function->NumParms >= 1)
		{
			uint8* Buffer = (uint8*)FMemory_Alloca(Function->ParmsSize);
			FMemory::Memzero(Buffer, Function->ParmsSize);

			// Find the first parameter (excluding return property)
			for (TFieldIterator<FProperty> It(Function); It && (It->PropertyFlags & CPF_Parm); ++It)
			{
				FProperty* Param = *It;
				if (Param->HasAnyPropertyFlags(CPF_ReturnParm))
				{
					continue;
				}

				void* ParamAddress = Buffer + Param->GetOffset_ForInternal();

				if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(Param))
				{
					FloatProp->SetFloatingPointPropertyValue(ParamAddress, FCString::Atof(*InStringValue));
				}
				else if (const FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Param))
				{
					DoubleProp->SetFloatingPointPropertyValue(ParamAddress, FCString::Atod(*InStringValue));
				}
				else if (const FIntProperty* IntProp = CastField<FIntProperty>(Param))
				{
					IntProp->SetPropertyValue(ParamAddress, FCString::Atoi(*InStringValue));
				}
				else if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(Param))
				{
					bool bValue = InStringValue.Equals(TEXT("true"), ESearchCase::IgnoreCase) || InStringValue == TEXT("1");
					BoolProp->SetPropertyValue(ParamAddress, bValue);
				}
				else if (const FByteProperty* ByteProp = CastField<FByteProperty>(Param))
				{
					ByteProp->SetPropertyValue(ParamAddress, (uint8)FCString::Atoi(*InStringValue));
				}
				else if (const FEnumProperty* EnumProp = CastField<FEnumProperty>(Param))
				{
					EnumProp->GetUnderlyingProperty()->SetIntPropertyValue(ParamAddress, (int64)FCString::Atoi(*InStringValue));
				}
				else if (const FStrProperty* StrProp = CastField<FStrProperty>(Param))
				{
					StrProp->SetPropertyValue(ParamAddress, InStringValue);
				}

				break; // Only set the first parameter
			}

			Settings->ProcessEvent(Function, Buffer);
			return;
		}
	}

	// Fallback: try property path
	FCachedPropertyPath PropertyPath(CachedFunctionOrPropertyPath);
	PropertyPathHelpers::SetPropertyValueFromString(Settings, PropertyPath, InStringValue);
}
