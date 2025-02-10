// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTTask/BTT_CutBandit.h"
#include "Controller/RiderAIController.h"
#include "Rider/Bandit/BanditBand.h"

UBTT_CutBandit::UBTT_CutBandit()
{
	NodeName = "Cut Bandit";
}

EBTNodeResult::Type UBTT_CutBandit::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto AiController = Cast<ARiderAIController>(OwnerComp.GetAIOwner()))
		if (UBanditBand* BanditBand = AiController->GetBanditBand())
			BanditBand->CutBand();

	return EBTNodeResult::Succeeded;
}
