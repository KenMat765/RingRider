#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

namespace FVectorUtility
{
	// 符号なし角度
	float RadianBetweenVectors(const FVector& _VectorFrom, const FVector& _VectorTo);
	// 符号あり角度
	float RadianBetweenVectors(const FVector2D& _VectorFrom, const FVector2D& _VectorTo);

	// 直交座標を極座標に変換 (水平右方向:0rad | 反時計回り:+ | 時計回り:-)
	void Cartesian2Polar(const FVector2D& _InputVector, float& _OutputLength, float& _OutputRadian);
	void Polar2Cartesian(float _InputLength, float _InputRadian, FVector2D& _OutputVector);
}

namespace FRotatorUtility
{
	// アクターを指定したスピードで指定した地点へ向かせるFRotatorを計算する
	FRotator GetLookAtRotator(AActor* _Actor, const FVector& _LookPos, float _DeltaTime, float _RotateSpeed);
}
