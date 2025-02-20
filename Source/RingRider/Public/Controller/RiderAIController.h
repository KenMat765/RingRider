// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "RiderAIController.generated.h"

class AAIRider;
class UBanditBand;

UCLASS()
class RINGRIDER_API ARiderAIController : public AAIController
{
	GENERATED_BODY()


public:
	ARiderAIController();

	virtual void Tick(float DeltaTime) override;

	AAIRider* GetAiRider() const { return AiRider; }
	UBanditBand* GetBanditBand() const { return BanditBand; }

	UFUNCTION()
	void OnPerception(AActor* _PerceivedActor, FAIStimulus _Stimulus);

	class UAISenseConfig_Sight* SightSenseConfig;

	virtual FGenericTeamId GetGenericTeamId() const override { return GenericTeamId; }

	// 落下時にPawnをDestroyさせたくないので、デフォルトのKillZでなく、自前で用意する
	UPROPERTY(EditAnywhere, Category = "Controll")
	float KillZ = -1000.f;


protected:
	virtual void OnPossess(APawn* _Pawn) override;

	AAIRider* AiRider;
	UBanditBand* BanditBand;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerception;

	FGenericTeamId GenericTeamId;

	// OnPerceptionの前に呼ばれ、検知したアクターのチームを判定する (なぜか2回呼ばれる)
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& _OtherActor) const override;

	virtual void OnRiderFellOff();

	FVector StartLocation;
};
