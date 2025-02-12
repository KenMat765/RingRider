// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_Approached.generated.h"

UCLASS()
class RINGRIDER_API UBTD_Approached : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_Approached();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category=Condition, meta = (ClampMin = "0.0"))
	float AcceptableRadius;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector ApproachOriginKey;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector ApproachTargetKey;

private:
	bool CalcConditionImpl(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;
};
