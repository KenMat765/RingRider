// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ShootBandit.generated.h"


UCLASS()
class RINGRIDER_API UBTT_ShootBandit : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_ShootBandit();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector TargetActorKey;
};
