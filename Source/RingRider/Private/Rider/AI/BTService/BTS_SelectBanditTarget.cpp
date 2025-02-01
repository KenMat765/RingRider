// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTService/BTS_SelectBanditTarget.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_SelectBanditTarget::UBTS_SelectBanditTarget()
{
	NodeName = "Select Bandit Target";
}

void UBTS_SelectBanditTarget::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	UBlackboardComponent* Blackboard = SearchData.OwnerComp.GetBlackboardComponent();

	// TODO:
	// 状況に応じてBanditBandをくっつけるべきアクターを選択する

	if (auto HostileRider = Cast<AActor>(Blackboard->GetValueAsObject(HostileRiderKey.SelectedKeyName)))
		Blackboard->SetValueAsObject(BanditTargetKey.SelectedKeyName, HostileRider);

	else if (auto FriendlyRider = Cast<AActor>(Blackboard->GetValueAsObject(FriendlyRiderKey.SelectedKeyName)))
		Blackboard->SetValueAsObject(BanditTargetKey.SelectedKeyName, FriendlyRider);

	else if (auto DashPole = Cast<AActor>(Blackboard->GetValueAsObject(DashPoleKey.SelectedKeyName)))
		Blackboard->SetValueAsObject(BanditTargetKey.SelectedKeyName, DashPole);
}
