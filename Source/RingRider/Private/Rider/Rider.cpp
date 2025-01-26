// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Rider.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
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



	// ===== Psm ===== //
	Psm = CreateDefaultSubobject<UPsmComponent>(TEXT("Parallel State Machine"));

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
}

void ARider::BeginPlay()
{
	Super::BeginPlay();

	SetSpeed(DefaultSpeed);
	SetTiltOffsetAndRange(0.f, DefaultTiltRange);

	SparkComp->Deactivate();
	SpinComp->Deactivate();

	// ここで残像のパラメータをセットしないと、エディタで設定した値が反映されない
	ImageComp->SetMaterialParams(AfterImageColor, AfterImageMetallic, AfterImageRoughness, AfterImageOpacity);
	ImageComp->SetLifetime(AfterImageLifetime);
	ImageComp->SetInterval(AfterImageInterval);
}

void ARider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Tickの最初に呼ぶこと
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
			SetRotation(ActorRotation);
		}

		// ===== Curve Accel ===== //
		if (bCanAccelOnCurve)
		{
			SpeedOffset = MaxSpeedOffset * pow(TiltRatio, 2);
			float TargetSpeed = DefaultSpeed + SpeedOffset;
			AccelSpeed(TargetSpeed, CurveAcceleration, DeltaTime);
		}

		// ===== Deceleration (スピードをデフォルト値まで徐々に戻す) ===== //
		float SpeedRate = (Speed - DefaultSpeed) / (MaxSpeed - DefaultSpeed);
		float DecelerationRate = FMath::Sign(SpeedRate) * FMath::Pow(FMath::Abs(SpeedRate), 1.f/DecelerationSensitivity);
		float DecelerationAmount = MaxDeceleration * DecelerationRate * DeltaTime;
		AddSpeed(-DecelerationAmount);
	}

	// ===== Move Forward ===== //
	Move(DeltaTime);

	// ===== Wheel Rotation ===== //
	float WheelRotSpeed = FMath::RadiansToDegrees(Speed / BIKE_RADIUS);
	FRotator DeltaWheelRot = FRotator(-WheelRotSpeed * DeltaTime, 0.f, 0.f);
	Wheel->AddLocalRotation(DeltaWheelRot);

	// ===== Stone Carry ===== //
	if (IsCarryingStone())
	{
		float DeltaEnergy = GetCarryingStone()->GetEnergyConsumePerSec() * DeltaTime;
		AddEnergy(-DeltaEnergy);
		if (GetEnergy() <= 0.f)
			ReleaseStone();
	}
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
				FVector ImpulseDirection = FVector(HitNormal.X, HitNormal.Y, 0.f).GetSafeNormal();
				FVector ImpulseVector = ImpulseDirection * CollisionImpulse;
				RootBox->AddImpulse(ImpulseVector);
			}
		}
	}
}



// IPhysicsMoveable Implementation /////////////////////////////////////////////////////////////
inline UPrimitiveComponent* ARider::GetPrimitiveComp() const { return Cast<UPrimitiveComponent>(RootBox); }



// IStoneCarryable Implementation /////////////////////////////////////////////////////////////
inline void ARider::CarryStone(AStone* _Stone)
{
	if (GetEnergy() <= 0.f)
		return;
	_Stone->SetTeam(GetTeam());
	_Stone->SetCanChangeTile(true);
	_Stone->SetStoneCarrier(this);
	CarryingStone = _Stone;
}

inline void ARider::ReleaseStone()
{
	if (!IsCarryingStone())
		return;
	CarryingStone->SetTeam(ETeam::Team_None);
	CarryingStone->SetCanChangeTile(false);
	CarryingStone->SetStoneCarrier(nullptr);
	CarryingStone = nullptr;
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
	// 別のステートでtrueにされるのを防ぐ
	bCanAccelOnCurve = false;

	// Inertia
	int Direction = static_cast<int>(_DriftDirection);
	float SpeedRate = (Speed - MinSpeed) / (MaxSpeed - MinSpeed); // 0.f ~ 1.f
	float DeltaAmount = MaxDriftInertiaSpeed * SpeedRate * _DeltaTime;
	FVector DeltaPos = GetActorRightVector() * DeltaAmount * -Direction;
	AddActorWorldOffset(DeltaPos);

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

