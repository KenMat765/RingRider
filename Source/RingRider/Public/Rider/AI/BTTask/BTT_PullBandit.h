// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PullBandit.generated.h"


UCLASS()
class RINGRIDER_API UBTT_PullBandit : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_PullBandit();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
