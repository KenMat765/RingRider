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
	// �G�l���M�[�l�̕ύX���\���ǂ��� (SetEnergy�Ńt�B���^�[�������邱��)
	virtual bool CanModifyEnergy() const = 0;
	virtual void SetEnergyModifiable(bool _bModifiable) = 0;

	virtual float GetEnergy() const = 0;
	virtual void SetEnergy(float _NewEnergy) = 0;
	virtual void AddEnergy(float _DeltaEnergy);

	// �G�l���M�[�ő�l�̗L���͔C�ӂȂ̂ŁA������virtual�ŗǂ�
	virtual float GetMaxEnergy() const;
	virtual void SetMaxEnergy(float _NewMaxEnergy);

	// _StealAmount�Ƀ}�C�i�X�̒l������ƁA����ɃG�l���M�[��^����
	virtual void StealEnergy(IEnergy* _OtherEnergy, float _StealAmount);

	// _GiveAmount�Ƀ}�C�i�X�̒l������ƁA���肩��G�l���M�[��D��
	virtual void GiveEnergy(IEnergy* _OtherEnergy, float _GiveAmount);
};
