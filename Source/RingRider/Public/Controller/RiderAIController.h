// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "RiderAIController.generated.h"


UCLASS()
class RINGRIDER_API ARiderAIController : public AAIController
{
	GENERATED_BODY()


public:
	ARiderAIController();

	UFUNCTION()
	void OnPerception(AActor* _PerceivedActor, FAIStimulus _Stimulus);

	class UAISenseConfig_Sight* SightSenseConfig;

	virtual FGenericTeamId GetGenericTeamId() const override { return GenericTeamId; }


protected:
	virtual void OnPossess(APawn* _Pawn) override;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerception;

	FGenericTeamId GenericTeamId;

	// OnPerceptionの前に呼ばれ、検知したアクターのチームを判定する (なぜか2回呼ばれる)
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& _OtherActor) const override;
};
