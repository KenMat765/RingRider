// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PhysicsMoveable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPhysicsMoveable : public UInterface
{
	GENERATED_BODY()
};

/**
 * •¨—‰‰Z‚É‚æ‚éˆÚ“®‚ğÀ‘•
 */
class RINGRIDER_API IPhysicsMoveable
{
	GENERATED_BODY()

public:
	virtual UPrimitiveComponent* GetPrimitiveComp() const = 0;
	virtual void AddImpulse(FVector _ImpulseVector);
};
