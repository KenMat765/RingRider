// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameInfo.h"
#include "Interface/BanditStickable.h"
#include "Stone.generated.h"


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

	bool IsStoneCarried() const { return CarrierActor != nullptr; }
	AActor* GetStoneCarrier() const { return CarrierActor; }
	void SetStoneCarrier(AActor* _NewCarrierActor);

	// Stone�̔j�󉉏o��������A��莞�Ԍ�ɃI�u�W�F�N�g��j������
	void DestructStone();

	virtual bool IsStickable() const override { return bIsStickable; }
	virtual void SetStickable(bool _bStickable) override { bIsStickable = _bStickable; }
	virtual void OnBanditPulledEnter(UBanditBand* _OtherBanditBand) override;


private:
	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	ETeam Team = ETeam::Team_None;

	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	bool bCanChangeTile = false;

	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	float RayDistance = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	float DecreaseEnergyPerSec = 20;

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

	AActor* CarrierActor;

	bool bAnimating = false;
	float AnimTimer = 0.f;
};
