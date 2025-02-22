// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/EQS/EnvQueryContext/EnvQueryContext_FinalDestination.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UEnvQueryContext_FinalDestination::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	auto QueryOwner = Cast<APawn>(QueryInstance.Owner.Get());
	if (!IsValid(QueryOwner))
	{
		UE_LOG(LogTemp, Error, TEXT("EnvQueryContext_FinalDestination: Could not cast Owner to APawn"));
		return;
	}

	auto AIController = Cast<AAIController>(QueryOwner->GetController());
	if (!IsValid(AIController))
	{
		UE_LOG(LogTemp, Error, TEXT("EnvQueryContext_FinalDestination: Could not cast Controller to AIController"));
		return;
	}

	auto Blackboard = AIController->GetBlackboardComponent();
	if (!IsValid(Blackboard))
	{
		UE_LOG(LogTemp, Error, TEXT("EnvQueryContext_FinalDestination: Could not get Blackboard from AIController"));
		return;
	}

	FName FinalDestinationKeyName = FName("FinalDestination");
	if (Blackboard->IsVectorValueSet(FinalDestinationKeyName))
	{
		FVector FinalDestination = Blackboard->GetValueAsVector(FinalDestinationKeyName);
		UEnvQueryItemType_Point::SetContextHelper(ContextData, FinalDestination);
	}
}
