// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameInfo.h"
#include "Interface/BanditStickable.h"
#include "Stone.generated.h"


class IStoneCarryable;


UCLASS()
class RINGRIDER_API AStone : public AActor, public IBanditStickable
{
	GENERATED_BODY()
	
public:	
	AStone();

public:
	virtual void Tick(float DeltaTime) override;


public:
	ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam NewTeam) { Team = NewTeam; }

	bool CanChangeTile() const { return bCanChangeTile; }
	void SetCanChangeTile(bool CanChangeTile) { bCanChangeTile = CanChangeTile; }

	bool IsStoneCarried() const { return StoneCarrier != nullptr; }
	IStoneCarryable* GetStoneCarrier() const { return StoneCarrier; }
	void SetStoneCarrier(IStoneCarryable* _NewStoneCarrier);

	float GetEnergyConsumePerSec() const { return EnergyConsumePerSec; }

	// Stoneの破壊演出をした後、一定時間後にオブジェクトを破棄する
	void DestructStone();

	virtual bool IsStickable() const override { return bIsStickable; }
	virtual void SetStickable(bool _bStickable) override { bIsStickable = _bStickable; }
	virtual TArray<UBanditBand*> GetStickedBands() const override { return StickedBands; }

	virtual void AddStickedBand(UBanditBand* _StickedBand) override
	{
		if (!StickedBands.Contains(_StickedBand))
			StickedBands.Add(_StickedBand);
	}

	virtual void RemoveStickedBand(UBanditBand* _StickedBand) override
	{
		if (StickedBands.Contains(_StickedBand))
			StickedBands.Remove(_StickedBand);
	}

	virtual void OnBanditPulledEnter(UBanditBand* _OtherBanditBand) override;


private:
	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	ETeam Team = ETeam::Team_None;

	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	bool bCanChangeTile = false;

	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	float RayDistance = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	float EnergyConsumePerSec = 20;

	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	float ZOffset = 260;

	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	float ChaseRatio = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	float DestructImpulse = 20000.0f;

	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	float DestroyDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Stone Properties|Animation")
	UCurveFloat* AnimCurve;

	UPROPERTY(EditAnywhere, Category = "Stone Properties|Animation")
	float AnimDuration = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Stone Properties|Animation")
	float AnimMaxZOffset = 350.f;

	UPROPERTY(EditAnywhere, Category = "Stone Properties|BanditBand")
	bool bIsStickable = true;

	UPROPERTY(VisibleAnywhere)
	class UDestructibleComponent* StoneDestructComp;

	UPROPERTY(VisibleAnywhere)
	class UBanditSnapArea* BanditSnapArea;

	class IStoneCarryable* StoneCarrier;

	bool bAnimating = false;
	float AnimTimer = 0.f;
	TArray<UBanditBand*> StickedBands;
};
