// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/BanditStickable.h"
#include "GroundPole.generated.h"


class UBanditBand;


UCLASS()
class RINGRIDER_API AGroundPole : public AActor, public IBanditStickable
{
	GENERATED_BODY()
	
public:	
	AGroundPole();


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
	float TurnSpeedOnPullDashStay = 2.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Band is forcibly cut when its length is below this value during Pull Dash"))
	float ForceCutLength = 200.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Obtains large bonus when band is cut below this length during Pull Dash"))
	float PerfectCutLength = 400.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Obtains bonus when band is cut below this length during Pull Dash"))
	float GreatCutLength = 600.f;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Duration of collision ignoring of Rider after Pull Dash"))
	float CollisionIgnoreSeconds = 1.f;
};
