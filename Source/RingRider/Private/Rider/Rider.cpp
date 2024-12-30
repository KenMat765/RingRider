// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Rider.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "GameInfo.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SearchLightComponent.h"
#include "Rider/Bandit/BanditBand.h"

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

	JumpState = [this](const FPsmInfo& Info) { this->JumpStateFunc(Info); };
	Psm->AddState(JumpState);

	BoostState = [this](const FPsmInfo& Info) { this->BoostStateFunc(Info); };
	Psm->AddState(BoostState);

	DriftState = [this](const FPsmInfo& Info) { this->DriftStateFunc(Info); };
	Psm->AddState(DriftState);



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
	BanditBand->SetupAttachment(Bike);	// Bikeが傾いてもBikeの中心から紐が出るように、Bikeの子にする
}



void ARider::BeginPlay()
{
	Super::BeginPlay();

	MaxSpeed = BoostSpeed;
	SetSpeed(DefaultSpeed);

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

	// ===== Tilt ===== //
	if (bCanTilt)
	{
		float TargetTilt = MaxTilt * StickValue;
		BikeBase->SetRelativeRotation(FRotator(0.f, 0.f, TargetTilt));
	}

	if (bIsGrounded)
	{
		float BikeTilt = Bike->GetComponentRotation().Roll;
		float TiltRatio = BikeTilt / MaxTilt;	// -1.0 ~ 1.0

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
	if (bCanMoveForward)
	{
		FVector DeltaPos = GetActorForwardVector() * Speed * DeltaTime;
		AddActorWorldOffset(DeltaPos);
	}

	// ===== Wheel Rotation ===== //
	float WheelRotSpeed = FMath::RadiansToDegrees(Speed / BIKE_RADIUS);
	FRotator DeltaWheelRot = FRotator(-WheelRotSpeed * DeltaTime, 0.f, 0.f);
	Wheel->AddLocalRotation(DeltaWheelRot);

	// ===== Lock On ===== //
	TargetActors = SearchTargetActor(LockOnRadius, LockOnAngle);
}



void ARider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("SwipeUp",		IE_Pressed,  this, &ARider::OnSwipeUp);
	PlayerInputComponent->BindAction("SwipeDown",	IE_Pressed,  this, &ARider::OnSwipeDown);
	PlayerInputComponent->BindAction("SwipeLeft",	IE_Pressed,  this, &ARider::OnSwipeLeft);
	PlayerInputComponent->BindAction("SwipeRight",	IE_Pressed,  this, &ARider::OnSwipeRight);

	PlayerInputComponent->BindAction("BoostButton", IE_Pressed,  this, &ARider::OnPressedBoost);
	PlayerInputComponent->BindAction("BoostButton", IE_Released, this, &ARider::OnReleasedBoost);

	PlayerInputComponent->BindAction("BanditButton", IE_Pressed,  this, &ARider::OnPressedBandit);
	PlayerInputComponent->BindAction("BanditButton", IE_Repeat,  this, &ARider::OnRepeatBandit);
	PlayerInputComponent->BindAction("BanditButton", IE_Released, this, &ARider::OnReleasedBandit);

	PlayerInputComponent->BindAxis("JoyStick", this, &ARider::OnJoyStick);
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
	}
}



// Speed ///////////////////////////////////////////////////////////////////////////////////////////
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



// Lock On /////////////////////////////////////////////////////////////////////////////////////////////
inline TArray<AActor*> ARider::SearchTargetActor(float Radius, float Angle)
{
	TArray<FName> _TargetTags = { FTagList::TAG_LOCKON };
	TArray<ECollisionChannel> _CollisionChannels = { ECC_WorldDynamic };
	TArray<AActor*> _TargetActors = SearchLightComp->SearchActors(Radius, Angle, _TargetTags, _CollisionChannels);
	return _TargetActors;
}

inline void ARider::LookAtActor(AActor* TargetActor, float RotationSpeed, float DeltaTime)
{
	FVector RelativeDirection = TargetActor->GetActorLocation() - GetActorLocation();
	FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(RelativeDirection);
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed);
	SetActorRotation(NewRotation);
}



// Input Events ////////////////////////////////////////////////////////////////////////////////////////
void ARider::OnSwipeUp()
{
	if (bIsGrounded)
	{
		Psm->TurnOnState(JumpState);
	}
}

void ARider::OnSwipeDown()
{
}

void ARider::OnSwipeLeft()
{
	float BikeTilt = Bike->GetComponentRotation().Roll;
	float TiltRatio = BikeTilt / MaxTilt;	// -1.0 ~ 1.0

	// Right (Opposite direction)
	if (TiltRatio >= 0.f)
	{
		Psm->TurnOnState(SlideState);
	}

	// Left (Same direction)
	else
	{
		if (Psm->IsStateOn(DriftState))
		{
			Psm->TurnOffState(DriftState);
		}
		else
		{
			Psm->TurnOnState(DriftState);
		}
	}
}

void ARider::OnSwipeRight()
{
	float BikeTilt = Bike->GetComponentRotation().Roll;
	float TiltRatio = BikeTilt / MaxTilt;	// -1.0 ~ 1.0

	// Right (Same direction)
	if (TiltRatio >= 0.f)
	{
		if (Psm->IsStateOn(DriftState))
		{
			Psm->TurnOffState(DriftState);
		}
		else
		{
			Psm->TurnOnState(DriftState);
		}
	}

	// Left (Opposite direction)
	else
	{
		Psm->TurnOnState(SlideState);
	}
}

void ARider::OnPressedBoost()
{
	if (Energy >= BoostEnterEnergy)
	{
		Psm->TurnOnState(BoostState);
	}
}

void ARider::OnReleasedBoost()
{
	Psm->TurnOffState(BoostState);
}

void ARider::OnPressedBandit()
{
	FVector AimTarget = GetActorLocation() + GetActorForwardVector() * BanditBand->MaxLength;
	BanditBand->StartAim(AimTarget);
}

void ARider::OnRepeatBandit()
{
	FVector AimTarget = GetActorLocation() + GetActorForwardVector() * BanditBand->MaxLength;
	BanditBand->SetAimTarget(AimTarget);
}

void ARider::OnReleasedBandit()
{
	BanditBand->EndAim();
	if (BanditBand->bCanShoot)
	{
		BanditBand->ShootBand();
	}
}

void ARider::OnJoyStick(float AxisValue)
{
	StickValue = AxisValue;
}



// States ///////////////////////////////////////////////////////////////////////////////////////////
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
		// 別のステートでtrueにされるのを防ぐ
		bCanTilt = false;
		bCanCurve = false;
		bCanMoveForward = false;

		// バイクを大きく傾ける (他のステートで変更されないようSTAYで呼ぶ)
		float TargetTilt = SlideTilt * SlideDirection * -1;
		BikeBase->SetRelativeRotation(FRotator(0.f, 0.f, TargetTilt));

		// 左右方向移動
		float TimeRatio = SlideTimer / SlideDuration;
		float SlideSpeed = SlideCurve->GetFloatValue(TimeRatio) * SlideMaxSpeed;
		FVector DeltaPos = GetActorRightVector() * SlideDirection * SlideSpeed * Info.DeltaTime;
		AddActorWorldOffset(DeltaPos);

		// 時間経過でスライド終了
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
		bCanTilt = true;
		bCanCurve = true;
		bCanMoveForward = true;
		
		// VFX
		ImageComp->StopEffect();
	}
	break;
	}
}

void ARider::JumpStateFunc(const FPsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
		const FVector ImpulseVector = FVector(0.f, 0.f, JumpImpulse);
		RootBox->AddImpulse(ImpulseVector);

		// VFX
		SpinComp->Activate(true);
	}
	break;

	case EPsmCondition::STAY:
	{
		Psm->TurnOffState(JumpState);
	}
	break;

	case EPsmCondition::EXIT:
	{
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
		// 別のステートでtrueにされるのを防ぐ
		bCanAccelOnCurve = false;
		// TickのTiltで常にSetRotationされ続けるので、Addし続ける
		BikeBase->AddLocalRotation(FRotator(BoostPitch, 0, 0));

		if (TargetActors.Num() > 0)
		{
			LookAtActor(TargetActors[0], LockOnAssistStrength, Info.DeltaTime);
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
		// ブースト無しで出せる最大スピードにする
		Speed = DefaultSpeed + MaxSpeedOffset;
		bCanAccelOnCurve = true;
		ImageComp->StopEffect();
	}
	break;
	}
}

void ARider::DriftStateFunc(const FPsmInfo& Info)
{
	// Direction of drift (1:right, -1:left)
	static int Direction;

	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
		// Jump
		if (bIsGrounded)
		{
			const FVector ImpulseVector = FVector(0.f, 0.f, DriftImpulse);
			RootBox->AddImpulse(ImpulseVector);
		}

		// Determine drift direction.
		float BikeTilt = Bike->GetComponentRotation().Roll;
		Direction = BikeTilt > 0 ? 1 : -1;
			
		// VFX
		SparkComp->SetRelativeRotation(FRotator(0.f, 0.f, SparkTilt * -Direction));
	}
	break;

	case EPsmCondition::STAY:
	{
		// 別のステートでtrueにされるのを防ぐ
		bCanTilt = false;
		bCanAccelOnCurve = false;

		// Inertia
		if (bIsGrounded)
		{
			float DeltaAmount = DriftInertiaSpeed * Info.DeltaTime;
			FVector DeltaPos = GetActorRightVector() * -Direction * DeltaAmount;
			AddActorWorldOffset(DeltaPos);
		}

		if (!Psm->IsStateOn(SlideState))
		{
			// Tilt (Slide状態の時はここでは傾きを制御しない)
			float TargetTilt = DriftMidTilt + DriftTiltRange * StickValue * Direction;
			TargetTilt *= Direction;
			BikeBase->SetRelativeRotation(FRotator(0.f, 0.f, TargetTilt));

			// VFX
			if (bIsGrounded)
			{
				if (!SparkComp->IsActive())
				{
					SparkComp->Activate(true);
				}
			}
			else
			{
				if (SparkComp->IsActive())
				{
					SparkComp->Deactivate();
				}
			}
			float AbsBikeTilt = Bike->GetComponentRotation().Roll * Direction;
			float MinTilt = DriftMidTilt - DriftTiltRange;
			float TiltRatio = (AbsBikeTilt - MinTilt) / (2 * DriftTiltRange);
			float SpawnRate = MinSparkRate + (MaxSparkRate - MinSparkRate) * TiltRatio;
			SparkComp->SetVariableFloat(SPARK_SPAWN_RATE, SpawnRate);
		}
	}
	break;

	case EPsmCondition::EXIT:
	{
		bCanTilt = true;
		bCanAccelOnCurve = true;

		// Jump
		if (bIsGrounded)
		{
			const FVector ImpulseVector = FVector(0.f, 0.f, DriftImpulse);
			RootBox->AddImpulse(ImpulseVector);
		}

		// VFX
		if (SparkComp->IsActive())
		{
			SparkComp->Deactivate();
		}
	}
	break;
	}
}

