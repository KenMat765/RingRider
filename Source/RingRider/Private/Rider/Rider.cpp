// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Rider.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SearchLightComponent.h"
#include "Rider/Bandit/BanditBand.h"
#include "Utility/TransformUtility.h"

// VFX Components
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "VFX/AfterImageComponent.h"


const float ARider::BIKE_RADIUS = 95.75f;
const FName ARider::SPARK_SPAWN_RATE = FName("SpawnRate");


ARider::ARider()
{
	PrimaryActorTick.bCanEverTick = true;


	RootBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	RootComponent = RootBox;

	BikeBase = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Bike"));
	BikeBase->SetupAttachment(RootComponent);

	Bike = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bike Mesh"));
	Bike->SetupAttachment(BikeBase);

	Wheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel Mesh"));
	Wheel->SetupAttachment(Bike);

	SearchLightComp = CreateDefaultSubobject<USearchLightComponent>(TEXT("Search Light"));
	SearchLightComp->SetupAttachment(RootComponent);



	// ===== Psm ===== //
	Psm = CreateDefaultSubobject<UPsmComponent>(TEXT("Parallel State Machine"));

	SlideState = [this](const FPsmInfo& Info) { this->SlideStateFunc(Info); };
	Psm->AddState(SlideState);

	BoostState = [this](const FPsmInfo& Info) { this->BoostStateFunc(Info); };
	Psm->AddState(BoostState);

	LeftDriftState = [this](const FPsmInfo& Info) { this->LeftDriftStateFunc(Info); };
	Psm->AddState(LeftDriftState);

	RightDriftState = [this](const FPsmInfo& Info) { this->RightDriftStateFunc(Info); };
	Psm->AddState(RightDriftState);



	// ===== VFX ===== //
	SparkComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Spark Effect"));
	SparkComp->SetupAttachment(RootComponent);
	SparkComp->SetRelativeLocation(FVector(0.f, 0.f, -BIKE_RADIUS));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SparkSystem(TEXT("/Game/Rider/NS_Spark"));
	if (SparkSystem.Succeeded())
	{
		SparkComp->SetAsset(SparkSystem.Object);
	}

	SpinComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Spin Effect"));
	SpinComp->SetupAttachment(Bike);
	SpinComp->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SpinSystem(TEXT("/Game/Rider/NS_Spin"));
	if (SpinSystem.Succeeded())
	{
		SpinComp->SetAsset(SpinSystem.Object);
	}

	ImageComp = CreateDefaultSubobject<UAfterImageComponent>(TEXT("After Image"));
	ImageComp->SetupAttachment(Bike);



	// ===== Bandit ===== //
	BanditBand = CreateDefaultSubobject<UBanditBand>(TEXT("Bandit Band"));
	BanditBand->SetupAttachment(BikeBase);	// Bike���X���Ă�Bike�̒��S����R���o��悤�ɁABikeBase�̎q�ɂ��� (Bike�̎q�ɂ���ƁA�\�����Ԃ�邱�Ƃ�����)
}

void ARider::BeginPlay()
{
	Super::BeginPlay();

	MaxSpeed = BoostSpeed;
	SetSpeed(DefaultSpeed);
	SetTiltOffsetAndRange(0.f, DefaultTiltRange);

	SparkComp->Deactivate();
	SpinComp->Deactivate();

	// �����Ŏc���̃p�����[�^���Z�b�g���Ȃ��ƁA�G�f�B�^�Őݒ肵���l�����f����Ȃ�
	ImageComp->SetMaterialParams(AfterImageColor, AfterImageMetallic, AfterImageRoughness, AfterImageOpacity);
	ImageComp->SetLifetime(AfterImageLifetime);
	ImageComp->SetInterval(AfterImageInterval);
}

void ARider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Log, TEXT("TipPos: %s"), *BanditBand->GetTipPos().ToString());
	UE_LOG(LogTemp, Log, TEXT("Length: %f"), BanditBand->GetBandLength());

	// Tick�̍ŏ��ɌĂԂ���
	bIsGrounded = bIsGroundedBuffer;
	bIsGroundedBuffer = false;
	bCanBounce = true;

	if (bIsGrounded)
	{
		float BikeTilt = Bike->GetComponentRotation().Roll;
		float TiltRatio = BikeTilt / DefaultTiltRange;

		// ===== Curve ===== //
		if (bCanCurve)
		{
			float RotationSpeed = MaxRotationSpeed * TiltRatio;
			FRotator ActorRotation = GetActorRotation();
			ActorRotation.Yaw += RotationSpeed * DeltaTime;
			SetActorRotation(ActorRotation);
		}

		// ===== Curve Accel ===== //
		if (bCanAccelOnCurve)
		{
			SpeedOffset = MaxSpeedOffset * pow(TiltRatio, 2);
			float TargetSpeed = DefaultSpeed + SpeedOffset;
			AccelSpeed(TargetSpeed, CurveAcceleration, DeltaTime);
		}
	}

	// ===== Move Forward ===== //
	MoveForward(DeltaTime);

	// ===== Wheel Rotation ===== //
	float WheelRotSpeed = FMath::RadiansToDegrees(Speed / BIKE_RADIUS);
	FRotator DeltaWheelRot = FRotator(-WheelRotSpeed * DeltaTime, 0.f, 0.f);
	Wheel->AddLocalRotation(DeltaWheelRot);

	// ===== Lock On ===== //
	TargetActors = SearchTargetActor(LockOnRadius, LockOnAngle);
}

void ARider::NotifyHit(
	class UPrimitiveComponent* MyComp,
	class AActor* Other,
	class UPrimitiveComponent* OtherComp,
	bool bSelfMoved,
	FVector HitLocation,
	FVector HitNormal,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Other)
	{
		if (Other->ActorHasTag(FTagList::TAG_GROUND))
		{
			bIsGroundedBuffer = true;
		}

		if (Other->ActorHasTag(FTagList::TAG_BOUNCE))
		{
			if (bCanBounce)
			{
				// Do this in order not to bound twice.
				bCanBounce = false;
				FVector ImpulseVector = HitNormal * CollisionImpulse;
				RootBox->AddImpulse(ImpulseVector);

				// Debug
				// FString ActorName = this->GetFName().ToString();
				// FString ComponentName = MyComp->GetFName().ToString();
				// UE_LOG(LogTemp, Log, TEXT("Actor: %s, Component: %s"), *ActorName, *ComponentName);
			}
		}

		if (Other->ActorHasTag(FTagList::TAG_RIDER))
		{
			ARider* OpponentRider = Cast<ARider>(Other);
			ensureAlwaysMsgf(OpponentRider != nullptr, TEXT("Could not cast to ARider!!"));
			if (IsBoosting() && !OpponentRider->IsBoosting())
			{
				StealEnergy(OpponentRider);
			}
		}

		/*
		if (BanditBand->IsPullDashState())
		{
			AActor* BanditStickedActor = BanditBand->GetStickedActor();
			if (BanditStickedActor && BanditStickedActor == Other)
			{
				BanditBand->CutBand();
				ECollisionChannel OtherChannel = OtherComp->GetCollisionObjectType();
				RootBox->SetCollisionResponseToChannel(OtherChannel, ECR_Ignore);
				// TODO: ��莞�Ԍ�ɃR���W�������ĊJ����
			}
		}
		*/
	}
}



// IMoveable Implementation ////////////////////////////////////////////////////////////////////////
void ARider::MoveForward(float DeltaTime)
{
	if (bCanMove)
	{
		FVector DeltaPos = GetActorForwardVector() * Speed * DeltaTime;
		AddActorWorldOffset(DeltaPos);
	}
}

void ARider::MoveToward(const FVector& _TargetPos, float DeltaTime)
{
	if (bCanMove)
	{
		FVector MoveDirection = (_TargetPos - GetActorLocation()).GetSafeNormal();
		FVector DeltaPos = MoveDirection * Speed * DeltaTime;
		AddActorWorldOffset(DeltaPos);
	}
}

void ARider::TriggerOnSpeedChangeActions(float _NewSpeed, float _MaxSpeed) const
{
	if (OnSpeedChangeActions.IsBound())
	{
		OnSpeedChangeActions.Broadcast(_NewSpeed, _MaxSpeed);
	}
}

FDelegateHandle ARider::AddOnSpeedChangeAction(TFunction<void(float, float)> NewFunc)
{
	auto NewAction = FSpeedChangeDelegate::FDelegate::CreateLambda(NewFunc);
	return OnSpeedChangeActions.Add(NewAction);
}

void ARider::RemoveOnSpeedChangeAction(FDelegateHandle DelegateHandle)
{
	OnSpeedChangeActions.Remove(DelegateHandle);
}



// IPhysicsMoveable Implementation /////////////////////////////////////////////////////////////
inline UPrimitiveComponent* ARider::GetPrimitiveComp() const { return Cast<UPrimitiveComponent>(RootBox); }



// Energy ///////////////////////////////////////////////////////////////////////////////////////////
void ARider::TriggerOnEnergyChangeActions(float _NewEnergy, float _MaxEnergy) const
{
	if (OnEnergyChangeActions.IsBound())
	{
		OnEnergyChangeActions.Broadcast(_NewEnergy, _MaxEnergy);
	}
}

FDelegateHandle ARider::AddOnEnergyChangeAction(TFunction<void(float, float)> NewFunc)
{
	auto NewAction = FEnergyChangeDelegate::FDelegate::CreateLambda(NewFunc);
	return OnEnergyChangeActions.Add(NewAction);
}

void ARider::RemoveOnEnergyChangeAction(FDelegateHandle DelegateHandle)
{
	OnEnergyChangeActions.Remove(DelegateHandle);
}

void ARider::StealEnergy(ARider* _RiderToStealFrom)
{
	float _StealEnergy = _RiderToStealFrom->GetEnergy() * EnergyStealRate;
	_RiderToStealFrom->AddEnergy(-_StealEnergy);
	AddEnergy(_StealEnergy);
}



// Tilt & Rotation ///////////////////////////////////////////////////////////////////////////
void ARider::TiltBike(float TiltRatio) const
{
	float TargetTilt = TiltOffset + TiltRange * TiltRatio;
	BikeBase->SetRelativeRotation(FRotator(0.f, 0.f, TargetTilt));
}



// Curve Accel ///////////////////////////////////////////////////////////////////////////////
inline void ARider::AccelSpeed(float TargetSpeed, float Acceleration, float DeltaTime)
{
	float DiffSpeed = TargetSpeed - Speed;
	if (DiffSpeed > 0)
	{
		float DeltaSpeed = FMath::Min(DiffSpeed, Acceleration);
		AddSpeed(DeltaSpeed * DeltaTime);
	}
	else
	{
		float DeltaSpeed = FMath::Max(DiffSpeed, -Acceleration);
		AddSpeed(DeltaSpeed * DeltaTime);
	}
}



// Actions ////////////////////////////////////////////////////////////////////////////////
void ARider::StartDrift(EDriftDirection _DriftDirection)
{
	switch (_DriftDirection)
	{
	case EDriftDirection::LEFT:	 Psm->TurnOnState(LeftDriftState);	break;
	case EDriftDirection::RIGHT: Psm->TurnOnState(RightDriftState);	break;
	}
}

void ARider::StopDrift()
{
	if(Psm->IsStateOn(LeftDriftState))
		Psm->TurnOffState(LeftDriftState);
	if(Psm->IsStateOn(RightDriftState))
		Psm->TurnOffState(RightDriftState);
}

inline bool ARider::IsDrifting(EDriftDirection& _OutDriftDirection)
{
	if (Psm->IsStateOn(LeftDriftState))
	{
		_OutDriftDirection = EDriftDirection::LEFT;
		return true;
	}
	else if (Psm->IsStateOn(RightDriftState))
	{
		_OutDriftDirection = EDriftDirection::RIGHT;
		return true;
	}
	else
		return false;
}

void ARider::OnEnterDrift(EDriftDirection _DriftDirection)
{
	bCanAccelOnCurve = false;
	int Direction = static_cast<int>(_DriftDirection);
	SetTiltOffsetAndRange(DriftMidTilt * Direction, DriftTiltRange);
	if (bIsGrounded)
		RootBox->AddImpulse(FVector(0.f, 0.f, DriftImpulse));
	SparkComp->SetRelativeRotation(FRotator(0.f, 0.f, -SparkTilt * Direction));
}

void ARider::OnDrifting(EDriftDirection _DriftDirection, float _DeltaTime)
{
	// �ʂ̃X�e�[�g��true�ɂ����̂�h��
	bCanAccelOnCurve = false;

	// Inertia
	if (bIsGrounded)
	{
		// �Ƃ肠�������͊����l�͈��
		int Direction = static_cast<int>(_DriftDirection);
		float DeltaAmount = DriftInertiaSpeed * _DeltaTime;
		FVector DeltaPos = GetActorRightVector() * DeltaAmount * -Direction;
		AddActorWorldOffset(DeltaPos);
	}

	if (!Psm->IsStateOn(SlideState))
	{
		// VFX
		if (bIsGrounded)
		{
			if (!SparkComp->IsActive())
				SparkComp->Activate(true);
		}
		else
		{
			if (SparkComp->IsActive())
				SparkComp->Deactivate();
		}
		float AbsBikeTilt = FMath::Abs(Bike->GetComponentRotation().Roll);
		float MinTilt = DriftMidTilt - DriftTiltRange;
		float TiltRatio = (AbsBikeTilt - MinTilt) / (2 * DriftTiltRange);
		float SpawnRate = MinSparkRate + (MaxSparkRate - MinSparkRate) * TiltRatio;
		SparkComp->SetVariableFloat(SPARK_SPAWN_RATE, SpawnRate);
	}
}

void ARider::OnExitDrift(EDriftDirection _DriftDirection)
{
	bCanAccelOnCurve = true;
	SetTiltOffsetAndRange(0.f, DefaultTiltRange);
	if (bIsGrounded)
		RootBox->AddImpulse(FVector(0.f, 0.f, DriftImpulse));
	if (SparkComp->IsActive())
		SparkComp->Deactivate();
}

void ARider::Jump()
{
	RootBox->AddImpulse(FVector(0.f, 0.f, JumpImpulse));
	SpinComp->Activate(true);
}

void ARider::SlideStateFunc(const FPsmInfo& Info)
{
	static int SlideDirection = 0;
	static float SlideTimer = 0.f;

	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
		// Reset Timer.
		SlideTimer = 0.f;

		// Determine slide direction.
		float BikeTilt = Bike->GetComponentRotation().Roll;
		SlideDirection = BikeTilt >= 0.f ? -1 : 1;
		
		// VFX
		ImageComp->PlayEffect();
	}
	break;

	case EPsmCondition::STAY:
	{
		// �ʂ̃X�e�[�g��true�ɂ����̂�h��
		bCanCurve = false;
		bCanMove = false;

		// �o�C�N��傫���X���� (���̃X�e�[�g�ŕύX����Ȃ��悤STAY�ŌĂ�)
		float TargetTilt = SlideTilt * SlideDirection * -1;
		BikeBase->SetRelativeRotation(FRotator(0.f, 0.f, TargetTilt));

		// ���E�����ړ�
		float TimeRatio = SlideTimer / SlideDuration;
		float SlideSpeed = SlideCurve->GetFloatValue(TimeRatio) * SlideMaxSpeed;
		FVector DeltaPos = GetActorRightVector() * SlideDirection * SlideSpeed * Info.DeltaTime;
		AddActorWorldOffset(DeltaPos);

		// ���Ԍo�߂ŃX���C�h�I��
		SlideTimer += Info.DeltaTime;
		if (SlideTimer >= SlideDuration)
		{
			Psm->TurnOffState(SlideState);
			return;
		}
	}
	break;

	case EPsmCondition::EXIT:
	{
		bCanCurve = true;
		bCanMove = true;
		
		// VFX
		ImageComp->StopEffect();
	}
	break;
	}
}

void ARider::BoostStateFunc(const FPsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
		SetSpeed(BoostSpeed);
		AddEnergy(-BoostEnterEnergy);
		ImageComp->PlayEffect();
	}
	break;

	case EPsmCondition::STAY:
	{
		// �ʂ̃X�e�[�g��true�ɂ����̂�h��
		bCanAccelOnCurve = false;
		// Tick��Tilt�ŏ��SetRotation���ꑱ����̂ŁAAdd��������
		BikeBase->AddLocalRotation(FRotator(BoostPitch, 0, 0));

		if (TargetActors.Num() > 0)
		{
			FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(this, TargetActors[0]->GetActorLocation(), Info.DeltaTime, LockOnAssistStrength);
			SetActorRotation(LookAtRotator);
		}

		AddEnergy(-BoostStayEnergyPerSec * Info.DeltaTime);
		if (Energy <= 0)
		{
			Psm->TurnOffState(BoostState);
		}
	}
	break;

	case EPsmCondition::EXIT:
	{
		// �u�[�X�g�����ŏo����ő�X�s�[�h�ɂ���
		Speed = DefaultSpeed + MaxSpeedOffset;
		bCanAccelOnCurve = true;
		ImageComp->StopEffect();
	}
	break;
	}
}

void ARider::LeftDriftStateFunc(const FPsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EPsmCondition::ENTER: OnEnterDrift(EDriftDirection::LEFT);				  break;
	case EPsmCondition::STAY:  OnDrifting(EDriftDirection::LEFT, Info.DeltaTime); break;
	case EPsmCondition::EXIT:  OnExitDrift(EDriftDirection::LEFT);				  break;
	}
}

void ARider::RightDriftStateFunc(const FPsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EPsmCondition::ENTER: OnEnterDrift(EDriftDirection::RIGHT);			   break;
	case EPsmCondition::STAY:  OnDrifting(EDriftDirection::RIGHT, Info.DeltaTime); break;
	case EPsmCondition::EXIT:  OnExitDrift(EDriftDirection::RIGHT);				   break;
	}
}



// Lock On /////////////////////////////////////////////////////////////////////////////////////////////
inline TArray<AActor*> ARider::SearchTargetActor(float Radius, float Angle)
{
	TArray<FName> _TargetTags = { FTagList::TAG_LOCKON };
	TArray<ECollisionChannel> _CollisionChannels = { ECC_WorldDynamic };
	TArray<AActor*> _TargetActors = SearchLightComp->SearchActors(Radius, Angle, _TargetTags, _CollisionChannels);
	return _TargetActors;
}

