// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTService/BTS_SetActorStickedTo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/RiderAIController.h"
#include "Rider/Bandit/BanditBand.h"

UBTS_SetActorStickedTo::UBTS_SetActorStickedTo()
{
	NodeName = "Set Actor Sticked To";
}

void UBTS_SetActorStickedTo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (auto AiController = Cast<ARiderAIController>(OwnerComp.GetAIOwner()))
	{
		AActor* ActorStickedTo = AiController->GetBanditBand()->GetStickInfo().StickActor;
		Blackboard->SetValueAsObject(ActorStickedToKey.SelectedKeyName, ActorStickedTo);
	}
}
