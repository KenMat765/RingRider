// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTService/BTS_UpdateDistance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_UpdateDistance::UBTS_UpdateDistance()
{
	NodeName = "Update Distance";

	In_TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateDistance, In_TargetKey), AActor::StaticClass());
	In_TargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateDistance, In_TargetKey));

	Out_DistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateDistance, Out_DistanceKey));
}

void UBTS_UpdateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController))
	{
		UE_LOG(LogTemp, Error, TEXT("UBTS_UpdateDistance: Could not get AI Controller"));
		return;
	}

	APawn* SelfActor = AIController->GetPawn();
	if (!IsValid(SelfActor))
	{
		UE_LOG(LogTemp, Error, TEXT("UBTS_UpdateDistance: Could not get AI Pawn"));
		return;
	}

	if (auto TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(In_TargetKey.SelectedKeyName)))
	{
		FVector TargetLocation = TargetActor->GetActorLocation();
		float Distance = FVector::Dist(SelfActor->GetActorLocation(), TargetLocation);
		Blackboard->SetValueAsFloat(Out_DistanceKey.SelectedKeyName, Distance);
	}

	else if (Blackboard->IsVectorValueSet(In_TargetKey.GetSelectedKeyID()))
	{
		FVector TargetLocation = Blackboard->GetValueAsVector(In_TargetKey.SelectedKeyName);
		float Distance = FVector::Dist(SelfActor->GetActorLocation(), TargetLocation);
		Blackboard->SetValueAsFloat(Out_DistanceKey.SelectedKeyName, Distance);
	}

	else
	{
		Blackboard->ClearValue(Out_DistanceKey.SelectedKeyName);
	}
}
