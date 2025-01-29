// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTTask/BTT_ShootBandit.h"
#include "AIController.h"
#include "Rider/Bandit/BanditBand.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_ShootBandit::UBTT_ShootBandit()
{
	NodeName = "Shoot Bandit";
}

EBTNodeResult::Type UBTT_ShootBandit::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));

	AAIController* AiController = OwnerComp.GetAIOwner();
	UBanditBand* BanditBand = AiController->GetPawn()->FindComponentByClass<UBanditBand>();

	if (TargetActor && BanditBand)
		BanditBand->ShootBand(TargetActor->GetActorLocation());

	return EBTNodeResult::Type();
}
