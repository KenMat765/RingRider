// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/RiderAIController.h"
#include "Rider/AIRider.h"


void ARiderAIController::OnPossess(APawn* _Pawn)
{
	AAIRider* AiRider = Cast<AAIRider>(_Pawn);
	if (AiRider && AiRider->BTAsset)
		RunBehaviorTree(AiRider->BTAsset);
	else
		UE_LOG(LogTemp, Error, TEXT("Could not load BTAsset from %s"), *_Pawn->GetName());
}
