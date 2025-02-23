// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Rider.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Rider/Bandit/BanditBand.h"
#include "Rider/Bandit/BanditSnapArea.h"
#include "Utility/TransformUtility.h"

// VFX Components
#include "NiagaraComponent.h"
#include "VFX/AfterImageComponent.h"


const float ARider::BIKE_RADIUS = 95.75f;
const FName ARider::SPARK_SPAWN_RATE = FName("SpawnRate");


ARider::ARider()
{
	PrimaryActorTick.bCanEverTick = true;


	Tags.Add(TAG_RIDER);

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
	BikeBase->bDoCollisionTest = false;

	Bike = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bike Mesh"));
	Bike->SetupAttachment(BikeBase);
	Bike->SetRelativeLocation(FVector(0.f, 0.f, BIKE_RADIUS));
	if (UStaticMesh* BikeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Rider/Mesh/Bike_Cockpit")))
		Bike->SetStaticMesh(BikeMesh);
	Bike->SetCollisionProfileName(TEXT("BanditStickableOverlap"));
	Bike->ComponentTags.Add(TAG_BIKE);
	Bike->ComponentTags.Add(TAG_BOUNCE);

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
	DashHitArea->SetCollisionProfileName(TEXT("OverlapAllDynamic"));


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
	SparkComp->SetAutoActivate(false);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SparkSystem(TEXT("/Game/Rider/VFX/NS_Spark"));
	if (SparkSystem.Succeeded())
		SparkComp->SetAsset(SparkSystem.Object);

	SpinComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Spin Effect"));
	SpinComp->SetupAttachment(Bike);
	SpinComp->SetAutoActivate(false);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SpinSystem(TEXT("/Game/Rider/VFX/NS_Spin"));
	if (SpinSystem.Succeeded())
		SpinComp->SetAsset(SpinSystem.Object);

	ImageComp = CreateDefaultSubobject<UAfterImageComponent>(TEXT("After Image"));
	ImageComp->SetupAttachment(Bike);
}

void ARider::BeginPlay()
{
	Super::BeginPlay();

	Bike->OnComponentBeginOverlap.AddDynamic(this, &ARider::OnBikeOverlapBegin);
	DashHitArea->OnComponentBeginOverlap.AddDynamic(this, &ARider::OnDashHitAreaOverlapBegin);

	GenericTeamId = FGenericTeamId(TeamId);
	SetSpeed(DefaultSpeed);
	SetTiltOffsetAndRange(0.f, DefaultTiltRange);

	// �����Ŏc���̃p�����[�^���Z�b�g���Ȃ��ƁA�G�f�B�^�Őݒ肵���l�����f����Ȃ�
	ImageComp->SetMaterialParams(AfterImageColor, AfterImageMetallic, AfterImageRoughness, AfterImageOpacity);
	ImageComp->SetLifetime(AfterImageLifetime);
	ImageComp->SetInterval(AfterImageInterval);
}

void ARider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Tick�̍ŏ��ɌĂԂ���
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

		// ===== Deceleration (�X�s�[�h���f�t�H���g�l�܂ŏ��X�ɖ߂�) ===== //
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

// RootBox�̃R���W����
// �n�ʂƂ̓����蔻��݂̂����o���A�ڒn���Ă��邩�ǂ����𔻒肷��
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
}

void ARider::OnBikeOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (OtherActor == this)
		return;

	// ��������_�b�V������DashHitArea���g��
	if (BanditBand->IsPullState())
		return;

	if (OtherComp->ComponentHasTag(TAG_BOUNCE))
	{
		if (bCanBounce)
		{
			bCanBounce = false; // Do this in order not to bound twice.

			// �I�[�o�[���b�v�����Ώۂ�����������֌��͂�������
			FVector OtherToSelf = OverlappedComp->GetComponentLocation() - OtherComp->GetComponentLocation();
			FVector ImpulseDirection = FVector(OtherToSelf.X, OtherToSelf.Y, 0.f).GetSafeNormal();
			FVector ImpulseVector = ImpulseDirection * CollisionImpulse;
			RootBox->AddImpulse(ImpulseVector);
		}
	}
}

void ARider::OnDashHitAreaOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (OtherActor == this)
		return;

	// ��������_�b�V�����̂�DashHitArea�͋@�\����
	if (!BanditBand->IsPullState())
		return;

	// Rider�ɓ���������X�^��������
	if (auto OtherRider = Cast<ARider>(OtherActor))
	{
		OtherRider->Stun();
		OtherRider->StealEnergy(OtherRider, OtherRider->GetEnergy() * EnergyStealRate);
	}

	// ���̑��̏�Q���ɓ��������ꍇ
	else if (OtherComp->ComponentHasTag(TAG_BOUNCE) && bCanBounce)
	{
		bCanBounce = false; // Do this in order not to bound twice.
		FVector ImpulseDirection = FVector(Hit.Normal.X, Hit.Normal.Y, 0.f).GetSafeNormal();
		FVector ImpulseVector = ImpulseDirection * CollisionImpulse;
		RootBox->AddImpulse(ImpulseVector);
		BanditBand->CutBand();
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
		_OtherBanditBand->CutBand(); // �K�v�ȃC���^�[�t�F�[�X���������Ă��Ȃ��ꍇ�A�����ł��Ȃ��̂ł��̂܂ܐ؂�
		return;
	}
	OtherMoveable->AddSpeed(AccelOnPullDashEnter);
}

void ARider::OnBanditPulledStay(UBanditBand* _OtherBanditBand, float _DeltaTime)
{
	if (!OtherMoveable || !OtherRotatable)
		return;

	OtherMoveable->AddSpeed(AccelOnPullDashStay * _DeltaTime);

	FVector StickPos = _OtherBanditBand->GetStickInfo().StickPos;
	// �W�����v�ɂ��Z�������̒����𖳎�
	float DistToSticked_XY = FVector::DistXY(_OtherBanditBand->GetComponentLocation(), StickPos);
	float NextMoveAmount = OtherMoveable->GetSpeed()*_DeltaTime;
	FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(_OtherBanditBand->GetOwner(), StickPos, _DeltaTime, TurnSpeedOnPullDashStay);
	float YawDiffAbs = FMath::Abs(LookAtRotator.Yaw - _OtherBanditBand->GetOwner()->GetActorRotation().Yaw);

	// ���������Ă���A�N�^�[���߂��ɂ����ԂŃ��[���}���ɕω�����ꍇ�A�A�N�^�[���}�ɕ����]�����Ă��܂�����Band�������J�b�g
	if (DistToSticked_XY < NextMoveAmount && YawDiffAbs > 90.f)
	{
		bIsForceCut = true;
		_OtherBanditBand->CutBand();
	}
	else
		OtherRotatable->SetRotation(LookAtRotator);
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
	// �ʂ̃X�e�[�g��true�ɂ����̂�h��
	bCanAccelOnCurve = false;

	// Inertia
	int Direction = static_cast<int>(_DriftDirection);
	float SpeedRate = (Speed - MinSpeed) / (MaxSpeed - MinSpeed); // 0.f ~ 1.f
	float DeltaAmount = MaxDriftInertiaSpeed * SpeedRate * _DeltaTime;
	DriftDeltaPos = GetActorRightVector() * DeltaAmount * -Direction;

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

		SetSpeed(GetMinSpeed());
		SetCanModifySpeed(false);

		// �X�^������BanditBand���t���Ȃ��悤�ɂ���
		SetStickable(false);
		BanditSnapArea->EnableSnap(false);

		// ���łɂ������Ă���Band�͐؂�
		if (GetStickedBands().Num() > 0)
		{
			for (UBanditBand* StickedBand : GetStickedBands())
				StickedBand->CutBand();
		}

		// �h���t�g���������璆�f
		if (IsDrifting())
			StopDrift();
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
		SetCanModifySpeed(true);

		SetStickable(true);
		BanditSnapArea->EnableSnap(true);

		Bike->SetRelativeRotation(FRotator::ZeroRotator);
		Bike->SetVisibility(true, true);
	} break;
	}
}

