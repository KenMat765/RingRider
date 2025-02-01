// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTService/BTS_UpdateStickedRiderInfo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Rider/Rider.h"
#include "Rider/Bandit/BanditBand.h"

UBTS_UpdateStickedRiderInfo::UBTS_UpdateStickedRiderInfo()
{
	NodeName = "Update Sticked Rider Info";
}

void UBTS_UpdateStickedRiderInfo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	auto SelfRider = Cast<ARider>(Blackboard->GetValueAsObject(SelfActorKey.SelectedKeyName));
	if (!IsValid(SelfRider))
	{
		UE_LOG(LogTemp, Error, TEXT("BTS_UpdateStickedRiderInfo: SelfActor was not Rider"));
		return;
	}

	auto StickedBands = SelfRider->GetStickedBands();
	if (StickedBands.Num() > 0)
	{
		auto StickedRider = Cast<ARider>(StickedBands[0]->GetOwner());
		if (!IsValid(StickedRider))
		{
			UE_LOG(LogTemp, Error, TEXT("BTS_UpdateStickedRiderInfo: Sticked Actor was not Rider"));
			return;
		}

		float Distance = FVector::Dist(SelfRider->GetActorLocation(), StickedRider->GetActorLocation());
		auto TeamAttitude = FGenericTeamId::GetAttitude(SelfRider, StickedRider);
		Blackboard->SetValueAsObject(StickedRiderKey.SelectedKeyName, StickedRider);
		Blackboard->SetValueAsFloat(DistanceToRiderKey.SelectedKeyName, Distance);
		Blackboard->SetValueAsEnum(TeamAttitudeOfRiderKey.SelectedKeyName, TeamAttitude);
	}
	else
	{
		Blackboard->ClearValue(StickedRiderKey.SelectedKeyName);
		Blackboard->ClearValue(DistanceToRiderKey.SelectedKeyName);
		Blackboard->ClearValue(TeamAttitudeOfRiderKey.SelectedKeyName);
	}
}
