// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Moveable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMoveable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 位置の移動に関する処理と、スピードへのアクセスのみを提供
 * オブジェクトの回転は扱わない
 */
class RINGRIDER_API IMoveable
{
	GENERATED_BODY()

public:
	virtual bool CanMove() const = 0;
	virtual void SetCanMove(bool _CanMove) = 0;

	virtual float GetSpeed() const = 0;
	virtual void SetSpeed(float _NewSpeed) = 0;
	virtual void AddSpeed(float _DeltaSpeed) = 0;

	virtual float GetMaxSpeed() const;
	virtual void SetMaxSpeed(float _NewSpeed);

	virtual void MoveForward(float DeltaTime);
	virtual void MoveToward(const FVector& _TargetPos, float DeltaTime);
};
