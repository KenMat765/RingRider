// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RiderJump.generated.h"


UCLASS()
class RINGRIDER_API UBTT_RiderJump : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_RiderJump();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
