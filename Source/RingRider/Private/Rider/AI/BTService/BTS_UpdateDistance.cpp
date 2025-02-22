// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTService/BTS_UpdateDistance.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_UpdateDistance::UBTS_UpdateDistance()
{
	NodeName = "Update Distance";

	In_DistanceFromKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateDistance, In_DistanceFromKey), AActor::StaticClass());
	In_DistanceFromKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateDistance, In_DistanceFromKey));

	In_DistanceToKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateDistance, In_DistanceToKey), AActor::StaticClass());
	In_DistanceToKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateDistance, In_DistanceToKey));

	Out_DistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateDistance, Out_DistanceKey));
}

void UBTS_UpdateDistance::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		// ‚±‚ê‚ðŒÄ‚Î‚È‚¢‚Æ FBlackboardKeySelector::GetSelectedKeyID() ‚Å³‚µ‚¢’l‚ªŽæ“¾‚Å‚«‚È‚¢
		In_DistanceFromKey.ResolveSelectedKey(*BBAsset);
		In_DistanceToKey.ResolveSelectedKey(*BBAsset);
		Out_DistanceKey.ResolveSelectedKey(*BBAsset);
	}
}

void UBTS_UpdateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	FVector FromLocation, ToLocation;
	if (Blackboard->GetLocationFromEntry(In_DistanceFromKey.GetSelectedKeyID(), FromLocation) &&
		Blackboard->GetLocationFromEntry(In_DistanceToKey.GetSelectedKeyID(), ToLocation))
	{
		float Distance = FVector::Dist(FromLocation, ToLocation);
		Blackboard->SetValueAsFloat(Out_DistanceKey.SelectedKeyName, Distance);
	}
	else
		Blackboard->ClearValue(Out_DistanceKey.SelectedKeyName);
}
