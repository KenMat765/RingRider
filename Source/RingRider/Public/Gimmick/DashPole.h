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
	virtual void OnBanditSticked(UBanditBand* _OtherBanditBand, AActor* _OtherActor) override;
	virtual void OnBanditPulledEnter(UBanditBand* _OtherBanditBand, AActor* _OtherActor) override;
	virtual void OnBanditPulledStay(UBanditBand* _OtherBanditBand, AActor* _OtherActor, float _DeltaTime) override;
	virtual void OnBanditPulledExit(UBanditBand* _OtherBanditBand, AActor* _OtherActor) override;


private:
	class IMoveable* OtherMoveable;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Speed added at the start of Pull-Dash"))
	float AccelOnPullDashEnter = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Speed continuously added during Pull-Dash"))
	float AccelOnPullDashStay = 200.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Rotation speed at which the sticked actor turns toward this actor during Pull-Dash"))
	float TurnSpeedOnPullDashStay = 1.5f;

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
		ToolTip="Duration of collision ignoring after Pull-Dash"))
	float CollisionIgnoreSeconds = 1.f;

	bool bIsForceCut = false;
};
