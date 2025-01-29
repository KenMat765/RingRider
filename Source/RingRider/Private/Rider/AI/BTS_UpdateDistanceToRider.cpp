// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTS_UpdateDistanceToRider.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTS_UpdateDistanceToRider::UBTS_UpdateDistanceToRider()
{
	NodeName = "Update Distance to Rider";
}

void UBTS_UpdateDistanceToRider::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AActor* SelfActor = Cast<AActor>(Blackboard->GetValueAsObject(SelfActorKey.SelectedKeyName));
	AActor* RiderActor = Cast<AActor>(Blackboard->GetValueAsObject(RiderKey.SelectedKeyName));
	if (SelfActor && RiderActor)
	{
		float Distance = FVector::Dist(SelfActor->GetActorLocation(), RiderActor->GetActorLocation());
		Blackboard->SetValueAsFloat(DistanceToRiderKey.SelectedKeyName, Distance);
	}
}
