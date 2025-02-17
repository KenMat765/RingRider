// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


namespace FComponentUtility
{
	template<typename T>
	typename TEnableIf<TIsDerivedFrom<T, UActorComponent>::Value, T*>::Type
	GetComponentByName(const AActor* Actor, FString Name)
	{
		TArray<T*> Components;
		Actor->GetComponents<T>(Components);
		for (T* Component : Components)
		{
			if (Component && Component->GetName() == Name)
			{
				return Component;
			}
		}
		return nullptr;
	}

	FTimerHandle IgnoreCollisionTemporary(
		FTimerManager& _TimerManager,
		UPrimitiveComponent* _Primitive,
		ECollisionChannel _IgnoreChannel,
		float _IgnoreSeconds);
}
