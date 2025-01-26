// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/BanditStickable.h"
#include "DashPole.generated.h"


UCLASS()
class RINGRIDER_API ADashPole : public AActor, public IBanditStickable
{
	GENERATED_BODY()
	
public:	
	ADashPole();


public:
	virtual bool IsStickable() const { return bIsStickable; }
	virtual void SetStickable(bool _bStickable) { bIsStickable = _bStickable; }

	virtual void OnBanditSticked(UBanditBand* _OtherBanditBand) override;
	virtual void OnBanditPulledEnter(UBanditBand* _OtherBanditBand) override;
	virtual void OnBanditPulledStay(UBanditBand* _OtherBanditBand, float _DeltaTime) override;
	virtual void OnBanditPulledExit(UBanditBand* _OtherBanditBand) override;


private:
	bool bIsStickable = true;

	class IMoveable* OtherMoveable;
	class IRotatable* OtherRotatable;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Speed added at the start of Pull-Dash"))
	float AccelOnPullDashEnter = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Speed continuously added during Pull-Dash"))
	float AccelOnPullDashStay = 500.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Max rotation speed at which the sticked actor turns toward this actor during Pull-Dash"))
	float MaxTurnSpeed = 8.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Min rotation speed at which the sticked actor turns toward this actor during Pull-Dash"))
	float MinTurnSpeed = 1.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Increasing this value reduces the rotation speed when the Band is long"))
	float TurnSpeedPower = 5.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Band is forcibly cut when its length is below this value during Pull-Dash"))
	float ForceCutLength = 300.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Obtains large bonus when band is cut below this length during Pull-Dash"))
	float PerfectCutLength = 600.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Obtains bonus when band is cut below this length during Pull-Dash"))
	float GreatCutLength = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Speed added on perfect cut"))
	float AccelOnPerfectCut = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Speed added on great cut"))
	float AccelOnGreatCut = 1500.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Duration of collision ignoring after Pull-Dash"))
	float CollisionIgnoreSeconds = 1.f;

	bool bIsForceCut = false;
};
