// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_CutBandit.generated.h"


UCLASS()
class RINGRIDER_API UBTT_CutBandit : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_CutBandit();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
