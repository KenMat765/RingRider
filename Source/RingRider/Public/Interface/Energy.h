// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Energy.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnergy : public UInterface
{
	GENERATED_BODY()
};


class RINGRIDER_API IEnergy
{
	GENERATED_BODY()

public:
	// エネルギー値の変更が可能かどうか (SetEnergyでフィルターをかけること)
	virtual bool CanModifyEnergy() const = 0;
	virtual void SetEnergyModifiable(bool _bModifiable) = 0;

	virtual float GetEnergy() const = 0;
	virtual void SetEnergy(float _NewEnergy) = 0;
	virtual void AddEnergy(float _DeltaEnergy);

	// エネルギー最大値の有無は任意なので、ただのvirtualで良い
	virtual float GetMaxEnergy() const;
	virtual void SetMaxEnergy(float _NewMaxEnergy);

	// _StealAmountにマイナスの値を入れると、相手にエネルギーを与える
	virtual void StealEnergy(IEnergy* _OtherEnergy, float _StealAmount);

	// _GiveAmountにマイナスの値を入れると、相手からエネルギーを奪う
	virtual void GiveEnergy(IEnergy* _OtherEnergy, float _GiveAmount);
};
