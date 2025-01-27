// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/JumpPole.h"
#include "Interface/PhysicsMoveable.h"


AJumpPole::AJumpPole()
{
	PrimaryActorTick.bCanEverTick = false;
}


void AJumpPole::OnBanditSticked(UBanditBand* _OtherBanditBand)
{
	if(!GetStickedBands().Contains(_OtherBanditBand))
		AddStickedBand(_OtherBanditBand);

	AActor* OtherActor = _OtherBanditBand->GetOwner();

	OtherPhysicsMoveable = Cast<IPhysicsMoveable>(OtherActor);
	if (!OtherPhysicsMoveable)
		UE_LOG(LogTemp, Warning, TEXT("%s: Could not get IPhysicsMoveable from %s"), *GetName(), *OtherActor->GetName());
}

void AJumpPole::OnBanditPulledEnter(UBanditBand* _OtherBanditBand)
{
	if (OtherPhysicsMoveable)
	{
		FVector ImpulseVector = _OtherBanditBand->GetBandDirection() * ImpulseOnPullJump;
		OtherPhysicsMoveable->AddImpulse(ImpulseVector);
	}
	_OtherBanditBand->CutBand();
}

