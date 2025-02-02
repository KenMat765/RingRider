// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Rider.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Rider/Bandit/BanditBand.h"
#include "Rider/Bandit/BanditSnapArea.h"
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


	Tags.Add(TAG_RIDER);
	Tags.Add(TAG_BOUNCE);

	RootBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	RootComponent = RootBox;
	RootBox->SetBoxExtent(FVector(96.f, 54.f, 96.f));
	RootBox->SetSimulatePhysics(true);
	RootBox->BodyInstance.bLockXRotation = true;
	RootBox->BodyInstance.bLockYRotation = true;
	RootBox->SetNotifyRigidBodyCollision(true);
	if (UPhysicalMaterial* RiderPhysMaterial = LoadObject<UPhysicalMaterial>(nullptr, TEXT("/Game/Rider/Physics/PM_Rider")))
		RootBox->SetPhysMaterialOverride(RiderPhysMaterial);
	RootBox->SetCollisionProfileName(TEXT("Rider"));

	BikeBase = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Bike"));
	BikeBase->SetupAttachment(RootComponent);
	BikeBase->SetRelativeLocation(FVector(0.f, 0.f, -BIKE_RADIUS));
	BikeBase->TargetArmLength = 0.f;
	BikeBase->bEnableCameraRotationLag = true;
	BikeBase->CameraRotationLagSpeed = 15.f;

	Bike = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bike Mesh"));
	Bike->SetupAttachment(BikeBase);
	Bike->SetRelativeLocation(FVector(0.f, 0.f, BIKE_RADIUS));
	if (UStaticMesh* BikeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Rider/Mesh/Bike_Cockpit")))
		Bike->SetStaticMesh(BikeMesh);
	Bike->SetCollisionProfileName(TEXT("BanditStickableOverlap"));
	Bike->ComponentTags.Add(TAG_BIKE);

	Wheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel Mesh"));
	Wheel->SetupAttachment(Bike);
	if (UStaticMesh* BikeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Rider/Mesh/Bike_Wheel")))
		Wheel->SetStaticMesh(BikeMesh);
	Wheel->SetGenerateOverlapEvents(false);
	Wheel->SetCollisionProfileName(TEXT("NoCollision"));

	BanditBand = CreateDefaultSubobject<UBanditBand>(TEXT("Bandit Band"));
	BanditBand->SetupAttachment(BikeBase);
	BanditBand->SetRelativeLocation(FVector(0.f, 0.f, BIKE_RADIUS));

	BanditSnapArea = CreateDefaultSubobject<UBanditSnapArea>(TEXT("Bandit Snap Area"));
	BanditSnapArea->SetupAttachment(Bike);
	BanditSnapArea->SetSphereRadius(2000.f);

	DashHitArea = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DashHitArea"));
	DashHitArea->SetupAttachment(Bike);
	DashHitArea->SetGenerateOverlapEvents(true);
	DashHitArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DashHitArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DashHitArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


	// ===== Psm ===== //
	Psm = CreateDefaultSubobject<UPsmComponent>(TEXT("Parallel State Machine"));

	LeftDriftState = [this](const FPsmInfo& Info) { this->LeftDriftStateFunc(Info); };
	Psm->AddState(LeftDriftState);

	RightDriftState = [this](const FPsmInfo& Info) { this->RightDriftStateFunc(Info); };
	Psm->AddState(RightDriftState);

	StunState = [this](const FPsmInfo& Info) { this->StunStateFunc(Info); };
	Psm->AddState(StunState);


	// ===== VFX ===== //
	SparkComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Spark Effect"));
	SparkComp->SetupAttachment(RootComponent);
	SparkComp->SetRelativeLocation(FVector(0.f, 0.f, -BIKE_RADIUS));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SparkSystem(TEXT("/Game/Rider/VFX/NS_Spark"));
	if (SparkSystem.Succeeded())
		SparkComp->SetAsset(SparkSystem.Object);

	SpinComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Spin Effect"));
	SpinComp->SetupAttachment(Bike);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SpinSystem(TEXT("/Game/Rider/VFX/NS_Spin"));
	if (SpinSystem.Succeeded())
		SpinComp->SetAsset(SpinSystem.Object);

	ImageComp = CreateDefaultSubobject<UAfterImageComponent>(TEXT("After Image"));
	ImageComp->SetupAttachment(Bike);
}

void ARider::BeginPlay()
{
	Super::BeginPlay();

	GenericTeamId = FGenericTeamId(TeamId);
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

		// ===== Rotation ===== //
		float RotationSpeed = MaxRotationSpeed * TiltRatio;
		FRotator ActorRotation = GetActorRotation();
		ActorRotation.Yaw += RotationSpeed * DeltaTime;
		SetRotation(ActorRotation);

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

void ARider::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (!Other)
		return;

	if (Other->ActorHasTag(TAG_GROUND))
	{
		bIsGroundedBuffer = true;
	}

	if (Other->ActorHasTag(TAG_BOUNCE))
	{
		if (bCanBounce)
		{
			bCanBounce = false; // Do this in order not to bound twice.
			FVector ImpulseDirection = FVector(HitNormal.X, HitNormal.Y, 0.f).GetSafeNormal();
			FVector ImpulseVector = ImpulseDirection * CollisionImpulse;
			RootBox->AddImpulse(ImpulseVector);
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



// IBanditStickable Implementation ///////////////////////////////////////////////////////////
void ARider::OnBanditSticked(UBanditBand* _OtherBanditBand)
{
	if(!GetStickedBands().Contains(_OtherBanditBand))
		AddStickedBand(_OtherBanditBand);

	AActor* OtherActor = _OtherBanditBand->GetOwner();

	OtherMoveable = Cast<IMoveable>(OtherActor);
	if (!OtherMoveable)
		UE_LOG(LogTemp, Warning, TEXT("%s: Could not get IMoveable from %s"), *GetName(), *OtherActor->GetName());

	OtherRotatable = Cast<IRotatable>(OtherActor);
	if (!OtherRotatable)
		UE_LOG(LogTemp, Warning, TEXT("%s: Could not get IRotatable from %s"), *GetName(), *OtherActor->GetName());
}

void ARider::OnBanditPulledEnter(UBanditBand* _OtherBanditBand)
{
	if (!OtherMoveable || !OtherRotatable)
	{
		_OtherBanditBand->CutBand(); // 必要なインターフェースを実装していない場合、何もできないのでそのまま切る
		return;
	}
	OtherMoveable->AddSpeed(AccelOnPullDashEnter);
}

void ARider::OnBanditPulledStay(UBanditBand* _OtherBanditBand, float _DeltaTime)
{
	FVector StickPos = _OtherBanditBand->GetStickInfo().StickPos;

	if (auto OtherRider = Cast<ARider>(_OtherBanditBand->GetOwner()))
	{
		TArray<AActor*> OverlapActors;
		OtherRider->GetDashHitAreaOverlap(OverlapActors, ARider::StaticClass());
		for (AActor* OverlapActor : OverlapActors)
		{
			auto OverlapRider = Cast<ARider>(OverlapActor);
			if (!IsValid(OverlapRider))
				continue;

			// TODO:
			// 相手が引っ張りダッシュ中はスタンさせない

			// 引張タックル成功
			OverlapRider->Stun();
			OverlapRider->GiveEnergy(OtherRider, OverlapRider->GetEnergy() * OtherRider->GetEnergyStealRate());
		}
	}

	if (OtherMoveable && OtherRotatable)
	{
		OtherMoveable->AddSpeed(AccelOnPullDashStay * _DeltaTime);

		// ジャンプによるZ軸方向の長さを無視
		float DistToSticked_XY = FVector::DistXY(_OtherBanditBand->GetComponentLocation(), StickPos);
		float NextMoveAmount = OtherMoveable->GetSpeed()*_DeltaTime;
		FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(_OtherBanditBand->GetOwner(), StickPos, _DeltaTime, TurnSpeedOnPullDashStay);
		float YawDiffAbs = FMath::Abs(LookAtRotator.Yaw - _OtherBanditBand->GetOwner()->GetActorRotation().Yaw);

		// 引っ張っているアクターが近くにいる状態でヨーが急激に変化する場合、アクターが急に方向転換してしまうためBandを強制カット
		if (DistToSticked_XY < NextMoveAmount && YawDiffAbs > 90.f)
		{
			bIsForceCut = true;
			_OtherBanditBand->CutBand();
		}
		else
			OtherRotatable->SetRotation(LookAtRotator);
	}
}

void ARider::OnBanditPulledExit(UBanditBand* _OtherBanditBand)
{
	if (!OtherMoveable || !OtherRotatable)
		return;

	float BandLength = _OtherBanditBand->GetBandLength();
	if (bIsForceCut)
	{
		bIsForceCut = false;
		UE_LOG(LogTemp, Log, TEXT("Force: %f"), BandLength);
	}
	else if (BandLength <= PerfectCutLength)
	{
		UE_LOG(LogTemp, Log, TEXT("Perfect: %f"), BandLength);
	}
}



// Tilt & Rotation ///////////////////////////////////////////////////////////////////////////
void ARider::TiltBike(float _TiltRatio)
{
	float TiltRatio = FMath::Clamp(_TiltRatio, -1.f, 1.f);
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

inline bool ARider::IsDrifting()
{
	return Psm->IsStateOn(LeftDriftState) || Psm->IsStateOn(RightDriftState);
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

void ARider::Stun()
{
	Psm->TurnOnState(StunState);
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

void ARider::StunStateFunc(const FPsmInfo& Info)
{
	static float StunTimer = 0.f;
	static float BlinkTimer = 0.f;

	const float SPIN_DEG_PER_SEC = 360.f * StunSpin / StunDuration;

	switch (Info.Condition)
	{
	case EPsmCondition::ENTER: {
		StunTimer = 0.f;
		BlinkTimer = 0.f;

		FCollisionResponseContainer RespContainer;
		RespContainer.SetResponse(ECC_Pawn, ECR_Ignore);
		RespContainer.SetResponse(ECC_WorldDynamic, ECR_Ignore);
		RootBox->SetCollisionResponseToChannels(RespContainer);

		SetSpeed(GetMinSpeed());
		SetCanModifySpeed(false);

		// スタン中はBanditBandが付かないようにする
		SetStickable(false);
		BanditSnapArea->EnableSnap(false);

		// すでにくっついているBandは切る
		if (GetStickedBands().Num() > 0)
		{
			for (UBanditBand* StickedBand : GetStickedBands())
				StickedBand->CutBand();
		}
	} break;

	case EPsmCondition::STAY: {
		Bike->AddLocalRotation(FRotator(0.f, SPIN_DEG_PER_SEC * Info.DeltaTime, 0.f));

		BlinkTimer += Info.DeltaTime;
		if (BlinkTimer >= StunBlinkInterval)
		{
			Bike->ToggleVisibility(true);
			BlinkTimer = 0.f;
		}

		StunTimer += Info.DeltaTime;
		if (StunTimer >= StunDuration)
			Psm->TurnOffState(StunState);
	} break;

	case EPsmCondition::EXIT: {
		FCollisionResponseContainer RespContainer;
		RespContainer.SetResponse(ECC_Pawn, ECR_Block);
		RespContainer.SetResponse(ECC_WorldDynamic, ECR_Block);
		RootBox->SetCollisionResponseToChannels(RespContainer);

		SetCanModifySpeed(true);

		SetStickable(true);
		BanditSnapArea->EnableSnap(true);

		Bike->SetRelativeRotation(FRotator::ZeroRotator);
		Bike->SetVisibility(true, true);
	} break;
	}
}

