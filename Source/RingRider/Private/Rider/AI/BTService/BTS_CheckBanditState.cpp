// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTService/BTS_CheckBanditState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/RiderAIController.h"
#include "Rider/Bandit/BanditBand.h"

UBTS_CheckBanditState::UBTS_CheckBanditState()
{
	NodeName = "Check Bandit State";
}

void UBTS_CheckBanditState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (auto AiController = Cast<ARiderAIController>(OwnerComp.GetAIOwner()))
	{
		if (UBanditBand* BanditBand = AiController->GetBanditBand())
		{
			EBanditState BanditState = BanditBand->GetBanditState();
			UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
			Blackboard->SetValueAsEnum(BanditStateKey.SelectedKeyName, static_cast<uint8>(BanditState));
		}
	}
}
