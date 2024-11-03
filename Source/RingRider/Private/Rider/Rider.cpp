// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Rider.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "VFX/AfterImageComponent.h"
#include "GameInfo.h"
#include "TagList.h"


const float ARider::BIKE_RADIUS = 95.75f;
const FName ARider::SPARK_SPAWN_RATE = FName("SpawnRate");
const FName ARider::RIDER_TAG = FName("Rider");


// Sets default values
ARider::ARider():
	bIsGrounded(false),
	bCanCurve(true),
	bCanTilt(true),
	bCanBounce(true),
	bIsGroundedBuffer(false),
	bCanAccelOnCurve(true),
	bCanMoveForward(true)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	// ===== Actor Settings ===== //
	Tags.Add(FTagList::TAG_RIDER);
	Tags.Add(FTagList::TAG_BOUNCE);



	// カーブはエディタからセットできなかったのでここでする
	SlideCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/Rider/SlideCurve"));



	// ===== Root Box ===== //
	RootBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	RootComponent = RootBox;
	RootBox->SetBoxExtent(FVector(96.f, 54.f, 96.f));

	// Physics
	RootBox->SetSimulatePhysics(true);
	RootBox->BodyInstance.bLockXRotation = true;
	RootBox->BodyInstance.bLockYRotation = true;
	const TCHAR RiderPhysMaterialPath[] = TEXT("PhysicalMaterial'/Game/Rider/PM_Rider.PM_Rider'");
	UPhysicalMaterial* RiderPhysMaterial = LoadObject<UPhysicalMaterial>(nullptr, RiderPhysMaterialPath);
	RootBox->SetPhysMaterialOverride(RiderPhysMaterial);

	// Collision
	RootBox->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootBox->SetNotifyRigidBodyCollision(true);



	// ===== Bike Base (Parent component of Bike) ===== //
	BikeBase = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Bike"));
	BikeBase->SetupAttachment(RootComponent);
	BikeBase->SetRelativeLocation(FVector(0.f, 0.f, -BIKE_RADIUS));

	// Relative Transform
	BikeBase->TargetOffset = FVector(0, 0, 0);
	BikeBase->TargetArmLength = 0.f;
	BikeBase->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

	// Lag
	BikeBase->bEnableCameraLag = false;
	BikeBase->bEnableCameraRotationLag = true;
	BikeBase->CameraRotationLagSpeed = 5.0f;

	// Rotation Inheritance
	BikeBase->bInheritYaw = true;
	BikeBase->bInheritPitch = true;
	BikeBase->bInheritRoll = true;



	// ===== Bike ===== //
	Bike = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bike Mesh"));
	Bike->SetupAttachment(BikeBase);
	Bike->SetRelativeLocation(FVector(0.f, 0.f, BIKE_RADIUS));

	// Mesh
	const TCHAR BikeMeshPath[] = TEXT("/Game/Rider/Bike_Cockpit");
	UStaticMesh* BikeMesh = LoadObject<UStaticMesh>(nullptr, BikeMeshPath);
	Bike->SetStaticMesh(BikeMesh);

	// Physics
	Bike->SetSimulatePhysics(false);

	// Collision
	Bike->SetCollisionProfileName(TEXT("NoCollision"));
	Bike->SetNotifyRigidBodyCollision(false);
	Bike->SetGenerateOverlapEvents(false);



	// ===== Wheel ===== //
	Wheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel Mesh"));
	Wheel->SetupAttachment(Bike);

	// Mesh
	const TCHAR WheelMeshPath[] = TEXT("/Game/Rider/Bike_Wheel");
	UStaticMesh* WheelMesh = LoadObject<UStaticMesh>(nullptr, WheelMeshPath);
	Wheel->SetStaticMesh(WheelMesh);

	// Physics
	Wheel->SetSimulatePhysics(false);

	// Collision
	Wheel->SetCollisionProfileName(TEXT("NoCollision"));
	Wheel->SetNotifyRigidBodyCollision(false);
	Wheel->SetGenerateOverlapEvents(false);



	// ===== Spring Arm ===== //
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Camera"));
	SpringArm->SetupAttachment(RootComponent);

	// Relative Transform
	SpringArm->TargetOffset = FVector(0, 0, 10.0f);
	SpringArm->TargetArmLength = 1500.0f;
	SpringArm->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));

	// Lag
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 5.f;

	// Rotation Inheritance
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;



	// ===== Camera ===== //
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;



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
	ImageComp->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	ImageComp->AddMesh(BikeMesh);
	ImageComp->AddMesh(WheelMesh);
}



// Called when the game starts or when spawned //////////////////////////////////////////////////////////////////
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



// Called every frame ///////////////////////////////////////////////////////////////////////////////////////////
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
}



// Called to bind functionality to input ///////////////////////////////////////////////////////////////////////
void ARider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("SwipeUp", IE_Pressed, this, &ARider::OnSwipeUp);
	PlayerInputComponent->BindAction("SwipeDown", IE_Pressed, this, &ARider::OnSwipeDown);
	PlayerInputComponent->BindAction("SwipeLeft", IE_Pressed, this, &ARider::OnSwipeLeft);
	PlayerInputComponent->BindAction("SwipeRight", IE_Pressed, this, &ARider::OnSwipeRight);

	PlayerInputComponent->BindAction("BoostButton", IE_Pressed, this, &ARider::OnPressedBoost);
	PlayerInputComponent->BindAction("BoostButton", IE_Released, this, &ARider::OnReleasedBoost);

	PlayerInputComponent->BindAxis("JoyStick", this, &ARider::OnJoyStick);
}



// Called on collision /////////////////////////////////////////////////////////////////////////////////////////
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



// Curve Accel ///////////////////////////////////////////////////////////////////////////////
void ARider::AccelSpeed(float TargetSpeed, float Acceleration, float DeltaTime)
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

		// VFX
		ImageComp->PlayEffect();
	}
	break;

	case EPsmCondition::STAY:
	{
		// 別のステートでtrueにされるのを防ぐ
		bCanAccelOnCurve = false;

		// TickのTiltで常にSetRotationされ続けるので、Addし続ける
		BikeBase->AddLocalRotation(FRotator(BoostPitch, 0, 0));
	}
	break;

	case EPsmCondition::EXIT:
	{
		// ブースト無しで出せる最大スピードにする
		Speed = DefaultSpeed + MaxSpeedOffset;
		bCanAccelOnCurve = true;

		// VFX
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
	Psm->TurnOnState(BoostState);
}

void ARider::OnReleasedBoost()
{
	Psm->TurnOffState(BoostState);
}

void ARider::OnJoyStick(float AxisValue)
{
	StickValue = AxisValue;
}

