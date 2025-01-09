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
	// �A�N�^�[���w�肵���X�s�[�h�Ŏw�肵���n�_�֌�������FRotator���v�Z����
	FRotator GetLookAtRotator(AActor* _Actor, const FVector& _LookPos, float _DeltaTime, float _RotateSpeed);
}
