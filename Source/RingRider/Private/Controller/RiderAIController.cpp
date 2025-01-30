// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/RiderAIController.h"
#include "Rider/AIRider.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"


ARiderAIController::ARiderAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));
	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	AIPerception->ConfigureSense(*SightSenseConfig);
	AIPerception->SetDominantSense(SightSenseConfig->GetSenseImplementation());
}

ETeamAttitude::Type ARiderAIController::GetTeamAttitudeTowards(const AActor& _OtherActor) const
{
	UE_LOG(LogTemp, Log, TEXT("GetTeamAttitudeTowards %s"), *_OtherActor.GetName());

	// _OtherActorのチーム判定を行い、判定したチームを返す
	if (auto OtherTeamInterface = Cast<IGenericTeamAgentInterface>(&_OtherActor))
	{
		FGenericTeamId OtherGenericTeamId = OtherTeamInterface->GetGenericTeamId();
		UE_LOG(LogTemp, Log, TEXT("MyTeamID: %d, OtherTeamID: %d"), GenericTeamId.GetId(), OtherGenericTeamId.GetId());
		if (OtherGenericTeamId == FGenericTeamId::NoTeam)
			return ETeamAttitude::Neutral;
		else if(OtherGenericTeamId == GenericTeamId)
			return ETeamAttitude::Friendly;
		else
			return ETeamAttitude::Hostile;
	}

	return ETeamAttitude::Neutral;
}

void ARiderAIController::OnPerception(AActor* _PerceivedActor, FAIStimulus _Stimulus)
{
	if (_PerceivedActor && _Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Log, TEXT("Found %s"), *_PerceivedActor->GetName());
		SetFocus(_PerceivedActor);
	}
}

void ARiderAIController::OnPossess(APawn* _Pawn)
{
	Super::OnPossess(_Pawn);

	AAIRider* AiRider = Cast<AAIRider>(_Pawn);
	if (!AiRider)
	{
		UE_LOG(LogTemp, Error, TEXT("RiderAIController: Failed to possess AIRider!!"));
		return;
	}

	// Behavior Tree
	if (AiRider->BTAsset)
	{
		if (RunBehaviorTree(AiRider->BTAsset))
		{
			// === For Test === //
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
				ARider* StickedRider = Cast<ARider>(GetWorld()->GetFirstPlayerController()->GetPawn());
				GetBlackboardComponent()->SetValueAsObject("StickedRider", StickedRider);
				});
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Failed to run BehaviorTree"));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Could not load BTAsset from %s"), *_Pawn->GetName());

	// AI Perception
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ARiderAIController::OnPerception);
	GenericTeamId = FGenericTeamId(AiRider->TeamId);
}
