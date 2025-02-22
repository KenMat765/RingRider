// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RiderMoveTo.generated.h"


UCLASS()
class RINGRIDER_API UBTT_RiderMoveTo : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_RiderMoveTo();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector TargetLocationKey;

	UPROPERTY(EditAnywhere, Category = Node)
	float AcceptableRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = Node, meta = (
		ToolTip="Bike's tilt reaches its maximum when the relative angle to TargetLocation is this value."))
	float MaxTiltDeg = 90.f;

	UPROPERTY(EditAnywhere, Category = Node)
	float DriftStartDeg = 90.f;

	UPROPERTY(EditAnywhere, Category = Node)
	float DriftEndDeg = 75.f;
};
