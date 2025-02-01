// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTService/BTS_SetTargetLocation.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_SetTargetLocation::UBTS_SetTargetLocation()
{
	NodeName = "Set Target Location";
}

void UBTS_SetTargetLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	Blackboard->SetValueAsVector(TargetLocationKey.SelectedKeyName, FVector::ZeroVector);
}
