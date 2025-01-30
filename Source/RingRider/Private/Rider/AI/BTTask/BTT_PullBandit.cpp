// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTTask/BTT_PullBandit.h"
#include "Controller/RiderAIController.h"
#include "Rider/Bandit/BanditBand.h"

UBTT_PullBandit::UBTT_PullBandit()
{
	NodeName = "Pull Bandit";
}

EBTNodeResult::Type UBTT_PullBandit::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto AiController = Cast<ARiderAIController>(OwnerComp.GetAIOwner());
	if (UBanditBand* BanditBand = AiController->GetBanditBand())
		BanditBand->PullBand();

	return EBTNodeResult::Type();
}
