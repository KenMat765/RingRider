#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

namespace FVectorUtility
{
	// �����Ȃ��p�x
	float RadianBetweenVectors(const FVector& _VectorFrom, const FVector& _VectorTo);
	// ��������p�x
	float RadianBetweenVectors(const FVector2D& _VectorFrom, const FVector2D& _VectorTo);

	// �������W���ɍ��W�ɕϊ� (�����E����:0rad | �����v���:+ | ���v���:-)
	void Cartesian2Polar(const FVector2D& _InputVector, float& _OutputLength, float& _OutputRadian);
	void Polar2Cartesian(float _InputLength, float _InputRadian, FVector2D& _OutputVector);

	/// <summary>
	/// _StartVector��_EndVector�̕�����_RotateRatio������]������Vector��Ԃ�
	/// </summary>
	/// <param name="_StartVector">�����Ő��K�������</param>
	/// <param name="_EndVector">�����Ő��K�������</param>
	/// <param name="_RotateRatio">_StartVector����]�����銄��. 0.f[=_StartVector] ~ 1.f[=_EndVector].</param>
	/// <returns>��]���ꂽFVector [���K������Ă���]</returns>
	FVector InterpolateVectorRotation(const FVector& _StartVector, const FVector& _EndVector, float _RotateRatio);
}

namespace FRotatorUtility
{
	// �A�N�^�[���w�肵���X�s�[�h�Ŏw�肵���n�_�֌�������FRotator���v�Z����
	FRotator GetLookAtRotator(AActor* _Actor, const FVector& _LookPos, float _DeltaTime, float _RotateSpeed);
}
