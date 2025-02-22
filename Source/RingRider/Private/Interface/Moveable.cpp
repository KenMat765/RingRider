// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/Moveable.h"

float IMoveable::GetMaxSpeed() const
{
	UE_LOG(LogTemp, Warning, TEXT("Does not have max speed: returning INFINITY"));
	return INFINITY;
}

void IMoveable::SetMaxSpeed(float _NewMaxSpeed)
{
	UE_LOG(LogTemp, Warning, TEXT("Does not have max speed"));
}

float IMoveable::GetMinSpeed() const
{
	UE_LOG(LogTemp, Warning, TEXT("Does not have min speed: returning 0"));
	return 0;
}

void IMoveable::SetMinSpeed(float _NewMinSpeed)
{
	UE_LOG(LogTemp, Warning, TEXT("Does not have min speed"));
}

void IMoveable::Move(float _DeltaTime)
{
	FVector DeltaPos = GetMoveDirection() * GetSpeed() * _DeltaTime;
	AddLocation(DeltaPos);
}

void IMoveable::MoveToward(const FVector& _TargetPos, float _Speed, float _DeltaTime)
{
	FVector MoveDirection = (_TargetPos - GetLocation()).GetSafeNormal();
	FVector DeltaPos = MoveDirection * _Speed * _DeltaTime;
	AddLocation(DeltaPos);
}
