// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

namespace FObjectUtility
{
	template<typename T>
	typename TEnableIf<TIsDerivedFrom<T, UInterface>::Value, typename T::IType*>::Type
	GetInterface(const UObject* Object)
	{
		UClass* InterfaceClass = T::StaticClass();
		if (Object && Object->GetClass()->ImplementsInterface(InterfaceClass))
		{
			return (typename T::IType*)Object->GetInterfaceAddress(InterfaceClass);
		}
		return nullptr;
	}
}
