// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_UpdateDistance.generated.h"

UCLASS()
class RINGRIDER_API UBTS_UpdateDistance : public UBTService
{
	GENERATED_BODY()
	

public:
	UBTS_UpdateDistance();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


public:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector In_TargetKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector Out_DistanceKey;
};
