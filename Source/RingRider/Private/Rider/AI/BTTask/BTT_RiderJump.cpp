// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTTask/BTT_RiderJump.h"
#include "AIController.h"
#include "Rider/Rider.h"

UBTT_RiderJump::UBTT_RiderJump()
{
	NodeName = "Rider Jump";
}

EBTNodeResult::Type UBTT_RiderJump::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AiController = OwnerComp.GetAIOwner();
	if (ARider* AiRider = Cast<ARider>(AiController->GetPawn()))
		AiRider->Jump();
	return EBTNodeResult::Type();
}
