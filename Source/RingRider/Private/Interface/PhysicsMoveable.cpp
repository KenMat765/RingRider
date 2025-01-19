// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/PhysicsMoveable.h"

void IPhysicsMoveable::AddImpulse(FVector _ImpulseVector)
{
	GetPrimitiveComp()->AddImpulse(_ImpulseVector);
}
