// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Bandit/BanditBand.h"
#include "NiagaraComponent.h"


const FString UBanditBand::BANDIT_BEAM_END	 = TEXT("BeamEnd");
const FString UBanditBand::BANDIT_BEAM_WIDTH = TEXT("BeamWidth");
const FString UBanditBand::BANDIT_COLOR		 = TEXT("Color");
const FString UBanditBand::BANDIT_INTENSITY	 = TEXT("Intensity");


UBanditBand::UBanditBand()
{
	PrimaryComponentTick.bCanEverTick = true;


	BanditVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Bandit Band"));

	Psm = CreateDefaultSubobject<UPsmComponent>(TEXT("Bandit PSM"));

	StandbyState = [this](const FPsmInfo& Info) { this->StandbyStateFunc(Info); };
	Psm->AddState(StandbyState);

	ExpandState = [this](const FPsmInfo& Info) { this->ExpandStateFunc(Info); };
	Psm->AddState(ExpandState);

	StickState = [this](const FPsmInfo& Info) { this->StickStateFunc(Info); };
	Psm->AddState(StickState);
}


void UBanditBand::BeginPlay()
{
	Super::BeginPlay();
}


void UBanditBand::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsAiming)
	{
		FVector SnapPos;
		if (CheckSnap(AimTarget, SnapPos))
		{
			AimTarget = SnapPos;
		}

		if (OnAimingActions.IsBound())
			OnAimingActions.Broadcast(AimTarget);
	}
}



// Aiming ////////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::StartAim(const FVector& _AimTarget)
{
	UE_LOG(LogTemp, Log, TEXT("Start Aim"));

	AimTarget = _AimTarget;
	FVector SnapPos;
	if (CheckSnap(AimTarget, SnapPos))
	{
		AimTarget = SnapPos;
	}

	bIsAiming = true;
	if (OnStartAimActions.IsBound())
		OnStartAimActions.Broadcast(AimTarget);
}

void UBanditBand::EndAim()
{
	UE_LOG(LogTemp, Log, TEXT("End Aim"));

	bIsAiming = false;
	if (OnEndAimActions.IsBound())
		OnEndAimActions.Broadcast();
}

FDelegateHandle UBanditBand::AddOnStartAimAction(TFunction<void(const FVector& AimPos)> NewFunc)
{
	auto NewAction = FStartAimDelegate::FDelegate::CreateLambda(NewFunc);
	return OnStartAimActions.Add(NewAction);
}

void UBanditBand::RemoveOnStartAimAction(FDelegateHandle DelegateHandle)
{
	OnStartAimActions.Remove(DelegateHandle);
}

FDelegateHandle UBanditBand::AddOnAimingAction(TFunction<void(const FVector&)> NewFunc)
{
	auto NewAction = FAimingDelegate::FDelegate::CreateLambda(NewFunc);
	return OnAimingActions.Add(NewAction);
}

void UBanditBand::RemoveOnAimingAction(FDelegateHandle DelegateHandle)
{
	OnAimingActions.Remove(DelegateHandle);
}

FDelegateHandle UBanditBand::AddOnEndAimAction(TFunction<void()> NewFunc)
{
	auto NewAction = FEndAimDelegate::FDelegate::CreateLambda(NewFunc);
	return OnEndAimActions.Add(NewAction);
}

void UBanditBand::RemoveOnEndAimAction(FDelegateHandle DelegateHandle)
{
	OnEndAimActions.Remove(DelegateHandle);
}

bool UBanditBand::CheckSnap(const FVector& _AimTarget, FVector& SnapPos)
{
	FCollisionObjectQueryParams ObjQueryParam;
	ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel2);

	FCollisionQueryParams QueryParam;
	QueryParam.AddIgnoredActor(this->GetOwner());

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByObjectType(
		Hit,
		GetComponentLocation(),
		_AimTarget,
		ObjQueryParam,
		QueryParam
	);

	if (bHit)
	{
		FVector HitActorWorldLoc = Hit.Actor->GetActorLocation();
		FVector HitCompLocalLoc = Hit.Component->GetComponentLocation();
		SnapPos = HitActorWorldLoc + HitCompLocalLoc;
	}

	return bHit;
}



// Shoot Out ////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::ShootBand()
{
}

void UBanditBand::StandbyStateFunc(const FPsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
	}
	break;

	case EPsmCondition::STAY:
	{
	}
	break;

	case EPsmCondition::EXIT:
	{
	}
	break;
	}
}

void UBanditBand::ExpandStateFunc(const FPsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
		BanditVFX->Activate();
	}
	break;

	case EPsmCondition::STAY:
	{
		// BanditComp->SetNiagaraVariableVec3(BANDIT_BEAM_END, FVector(, 0, 0));
	}
	break;

	case EPsmCondition::EXIT:
	{
	}
	break;
	}
}

void UBanditBand::StickStateFunc(const FPsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
	}
	break;

	case EPsmCondition::STAY:
	{
	}
	break;

	case EPsmCondition::EXIT:
	{
	}
	break;
	}
}
