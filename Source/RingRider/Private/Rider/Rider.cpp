// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Rider.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

const float ARider::BIKE_RADIUS = 95.75f;

// Sets default values
ARider::ARider():
	bIsGrounded(false),
	bCanBounce(true)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	// ===== Actor Settings ===== //
	Tags.Add(FName("Bounce"));



	// ===== Property Defaults ===== //
	DefaultSpeed = 2000.f;
	MaxRotationSpeed = 80.f;
	MaxTilt = 30.f;
	JumpImpulse = 1500000.f;
	CurveAcceleration = 200.f;
	CurveDeceleration = 100.f;
	MaxSpeedOffset = 2000.f;
	CollisionImpulse = 5000000.f;



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
	SpringArm->TargetArmLength = 1000.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	// Lag
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 2.f;

	// Rotation Inheritance
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;



	// ===== Camera ===== //
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;
}

// Called when the game starts or when spawned
void ARider::BeginPlay()
{
	Super::BeginPlay();

	Speed = DefaultSpeed;
}

// Called every frame
void ARider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsGrounded)
	{
		float BikeTilt = Bike->GetComponentRotation().Roll;
		float TiltRatio = BikeTilt / MaxTilt;	// -1.0 ~ 1.0

		// ===== Curve ===== //
		float RotationSpeed = MaxRotationSpeed * TiltRatio;
		FRotator ActorRotation = GetActorRotation();
		ActorRotation.Yaw += RotationSpeed * DeltaTime;
		SetActorRotation(ActorRotation);

		// Ac[De]celerate speed according to current bike tilt.
		SpeedOffset = MaxSpeedOffset * pow(TiltRatio, 2);
		float TargetSpeed = DefaultSpeed + SpeedOffset;
		if (Speed < TargetSpeed)
		{
			Speed += CurveAcceleration * DeltaTime;
		}
		else
		{
			Speed -= CurveDeceleration * DeltaTime;
		}
	}

	// ===== Move Forward ===== //
	FVector DeltaPos = GetActorForwardVector() * Speed * DeltaTime;
	AddActorWorldOffset(DeltaPos);

	// ===== Wheel Rotation ===== //
	float WheelRotSpeed = FMath::RadiansToDegrees(Speed / BIKE_RADIUS);
	FRotator DeltaWheelRot = FRotator(-WheelRotSpeed * DeltaTime, 0.f, 0.f);
	Wheel->AddLocalRotation(DeltaWheelRot);

	// Do these, because we cannot call OnCollisionExit.
	bIsGrounded = false;
	bCanBounce = true;
}

// Called to bind functionality to input
void ARider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARider::OnJumpInput);
	PlayerInputComponent->BindAxis("Curve", this, &ARider::OnCurveInput);
}

// Called on collision
void ARider::NotifyHit(
	class UPrimitiveComponent* MyComp,
	class AActor* Other,
	class UPrimitiveComponent* OtherComp,
	bool bSelfMoved,
	FVector HitLocation,
	FVector HitNormal,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	bIsGrounded = true;

	if (Other)
	{
		if (bCanBounce && Other->ActorHasTag(FName("Bounce")))
		{
			// Do this in order not to bound twice.
			bCanBounce = false;
			FVector ImpulseVector = HitNormal * CollisionImpulse;
			RootBox->AddImpulse(ImpulseVector);

			FString ActorName = this->GetFName().ToString();
			FString ComponentName = MyComp->GetFName().ToString();
			UE_LOG(LogTemp, Log, TEXT("Actor: %s, Component: %s"), *ActorName, *ComponentName);
		}
	}
}



// Movements ///////////////////////////////////////////////////////////////////////////////////////////
void ARider::Lean(float TiltRatio)
{
	float Tilt = MaxTilt * TiltRatio;
	BikeBase->SetRelativeRotation(FRotator(0.f, 0.f, Tilt));
}

void ARider::Jump(float Impulse)
{
	bIsGrounded = false;
	const FVector ImpulseVector = FVector(0.f, 0.f, Impulse);
	RootBox->AddImpulse(ImpulseVector);
}



// Input Events ////////////////////////////////////////////////////////////////////////////////////////
void ARider::OnCurveInput(float AxisValue)
{
	Lean(AxisValue);
}

void ARider::OnJumpInput()
{
	if (bIsGrounded)
	{
		Jump(JumpImpulse);
	}
}



// Private Properties ///////////////////////////////////////////////////////////////////////////////////
bool ARider::IsGrounded() const { return bIsGrounded; }
float ARider::GetSpeed() const { return Speed; }
float ARider::GetSpeedOffset() const { return SpeedOffset; }

