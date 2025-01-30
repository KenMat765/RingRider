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


protected:
	virtual void OnPossess(APawn* _Pawn) override;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerception;
};
