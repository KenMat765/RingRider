#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

namespace FVectorUtility
{
	// Returns 0 when input vector was zero.
	float RadianBetweenVectors(const FVector& _VectorFrom, const FVector& _VectorTo);
	float RadianBetweenVectors(const FVector2D& _VectorFrom, const FVector2D& _VectorTo);

	// �������W���ɍ��W�ɕϊ� (�����E����:0rad | �����v���:+ | ���v���:-)
	void Cartesian2Polar(const FVector2D& _InputVector, float& _OutputLength, float& _OutputRadian);
	void Polar2Cartesian(float _InputLength, float _InputRadian, FVector2D& _OutputVector);
}

namespace FRotatorUtility
{
	// �A�N�^�[���w�肵���X�s�[�h�Ŏw�肵���n�_�֌�������FRotator���v�Z����
	FRotator GetLookAtRotator(AActor* _Actor, const FVector& _LookPos, float _DeltaTime, float _RotateSpeed);
}
