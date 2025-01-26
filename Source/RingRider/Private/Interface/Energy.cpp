// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/Energy.h"

void IEnergy::AddEnergy(float _DeltaEnergy)
{
	SetEnergy(GetEnergy() + _DeltaEnergy);
}

float IEnergy::GetMaxEnergy() const
{
	UE_LOG(LogTemp, Warning, TEXT("Does not have max energy: returning INFINITY"));
	return INFINITY;
}

void IEnergy::SetMaxEnergy(float _NewMaxEnergy)
{
	UE_LOG(LogTemp, Warning, TEXT("Does not have max energy"));
}

void IEnergy::StealEnergy(IEnergy* _OtherEnergy, float _StealAmount)
{
	_OtherEnergy->AddEnergy(-_StealAmount);
	AddEnergy(_StealAmount);
}

void IEnergy::GiveEnergy(IEnergy* _OtherEnergy, float _GiveAmount)
{
	AddEnergy(-_GiveAmount);
	_OtherEnergy->AddEnergy(_GiveAmount);
}
