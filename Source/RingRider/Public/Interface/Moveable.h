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
 * �ʒu�̈ړ��Ɋւ��鏈���ƁA�X�s�[�h�ւ̃A�N�Z�X�݂̂��
 * �I�u�W�F�N�g�̉�]�͈���Ȃ�
 */
class RINGRIDER_API IMoveable
{
	GENERATED_BODY()

public:
	virtual bool CanMove() const = 0;
	virtual void SetCanMove(bool _CanMove) = 0;

	virtual bool CanModifySpeed() const = 0;
	virtual void SetCanModifySpeed(bool _CanModifySpeed) = 0;

	virtual float GetSpeed() const = 0;
	virtual void SetSpeed(float _NewSpeed) = 0;
	virtual void AddSpeed(float _DeltaSpeed) { SetSpeed(GetSpeed() + _DeltaSpeed); }

	virtual FVector GetMoveDirection() const = 0;
	virtual void SetMoveDirection(FVector _NewMoveDirection) = 0;

	virtual FVector GetLocation() const = 0;
	virtual void SetLocation(FVector _NewLocation) = 0;
	virtual void AddLocation(FVector _DeltaLocation) { SetLocation(GetLocation() + _DeltaLocation); }

	// ����:GetMoveDirection()�֑��x:GetSpeed()�ňړ�������
	virtual void Move(float _DeltaTime);
	// �C�ӂ̕����֔C�ӂ̑��x�ňړ�������
	virtual void MoveToward(const FVector& _TargetPos, float _Speed, float _DeltaTime);

	// �ō�(��)���x�̗L���͔C�ӂȂ̂ŁA������virtual�ŗǂ�
	virtual float GetMaxSpeed() const;
	virtual void SetMaxSpeed(float _NewMaxSpeed);
	virtual float GetMinSpeed() const;
	virtual void SetMinSpeed(float _NewMinSpeed);
};
