#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

namespace FVectorUtility
{
	// Returns 0 when input vector was zero.
	float RadianBetweenVectors(const FVector& VectorFrom, const FVector& VectorTo);
}

namespace FRotatorUtility
{
	// アクターを指定したスピードで指定した地点へ向かせるFRotatorを計算する
	FRotator GetLookAtRotator(AActor* _Actor, const FVector& _LookPos, float _DeltaTime, float _RotateSpeed);
}
