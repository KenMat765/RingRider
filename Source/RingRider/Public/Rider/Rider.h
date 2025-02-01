// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PsmComponent.h"
#include "GameInfo.h"
#include "GenericTeamAgentInterface.h"
#include "Interface/Moveable.h"
#include "Interface/PhysicsMoveable.h"
#include "Interface/Rotatable.h"
#include "Interface/Energy.h"
#include "Interface/StoneCarryable.h"
#include "Interface/BanditStickable.h"
#include "Rider.generated.h"


UCLASS()
class RINGRIDER_API ARider
	: public APawn, public IMoveable, public IPhysicsMoveable, public IRotatable, public IEnergy, public IStoneCarryable, public IBanditStickable
	, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ARider();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;


	// Constants ////////////////////////////////////////////////////////////////////////////////
public:
	const static float BIKE_RADIUS;

private:
	const static FName SPARK_SPAWN_RATE;


	// Components //////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* RootBox;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* BikeBase;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Bike;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Wheel;

	UPROPERTY(VisibleAnywhere)
	class UBanditBand* BanditBand;

	UPROPERTY(VisibleAnywhere)
	class UBanditSnapArea* BanditSnapArea;

	class UPsmComponent* Psm;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* SparkComp;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* SpinComp;

	UPROPERTY(VisibleAnywhere)
	class UAfterImageComponent* ImageComp;


	// Team ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Team")
	ETeam Team;

public:
	ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam NewTeam) { Team = NewTeam; }

	UPROPERTY(EditAnywhere, Category="Rider Properties|Team")
	uint8 TeamId = 0;

	virtual FGenericTeamId GetGenericTeamId() const override { return GenericTeamId; }

protected:
	FGenericTeamId GenericTeamId;


	// IMoveable Implementation /////////////////////////////////////////////////////////////
public:
	virtual bool CanMove() const override { return bCanMove; }
	virtual void SetCanMove(bool _CanMove) override { bCanMove = _CanMove; }

	virtual bool CanModifySpeed() const override { return bCanModifySpeed; }
	virtual void SetCanModifySpeed(bool _CanModifySpeed) override { bCanModifySpeed = _CanModifySpeed; }

	virtual float GetSpeed() const override { return Speed; }
	virtual void SetSpeed(float _NewSpeed) override
	{
		if (!CanModifySpeed())
			return;
		Speed = _NewSpeed;
		Speed = FMath::Clamp(Speed, MinSpeed, MaxSpeed);
		if(OnSpeedChanged.IsBound())
			OnSpeedChanged.Broadcast(Speed, DefaultSpeed, MinSpeed, MaxSpeed);
	}

	virtual FVector GetMoveDirection() const override { return GetActorForwardVector(); }
	virtual void SetMoveDirection(FVector _NewMoveDirection) override
	{
		UE_LOG(LogTemp, Warning, TEXT("Move direction of Rider is FIXED to actor's forward vector"));
	};

	virtual FVector GetLocation() const override { return GetActorLocation(); }
	virtual void SetLocation(FVector _NewLocation) override
	{
		if(CanMove())
			SetActorLocation(_NewLocation);
	}
	virtual void AddLocation(FVector _DeltaLocation) override
	{
		if(CanMove())
			AddActorWorldOffset(_DeltaLocation);
	}

	virtual float GetMaxSpeed() const override { return MaxSpeed; }
	virtual void SetMaxSpeed(float _NewMaxSpeed) override { MaxSpeed = _NewMaxSpeed; }
	virtual float GetMinSpeed() const override { return MinSpeed; }
	virtual void SetMinSpeed(float _NewMinSpeed) override { MinSpeed = _NewMinSpeed; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpeedChangedDelegate, float, NewSpeed, float, DefaultSpeed, float, MinSpeed, float, MaxSpeed);
	FSpeedChangedDelegate OnSpeedChanged;

private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Movement")
	bool bCanMove = true;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Movement")
	bool bCanModifySpeed = true;
	
	UPROPERTY(EditAnywhere, Category="Rider Properties|Movement")
	float DefaultSpeed = 2000.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Movement")
	float MaxSpeed = 6000.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Movement")
	float MinSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Movement")
	float MaxDeceleration = 800.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Movement", meta = (ClampMin="0.1", ClampMax="10.0", UIMin="0.1", UIMax="10.0",
		ToolTip="Increasing this value causes greater deceleration at higher speeds."))
	float DecelerationSensitivity = 0.5f;

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Movement")
	float Speed;


	// IPhysicsMoveable Implementation /////////////////////////////////////////////////////////////
public:
	virtual UPrimitiveComponent* GetPrimitiveComp() const override;


	// IRotatable Implementation /////////////////////////////////////////////////////////////
public:
	virtual bool CanRotate() const override { return bCanRotate; }
	virtual void SetCanRotate(bool _CanRotate) override { bCanRotate = _CanRotate; }

	virtual FRotator GetRotation() const { return GetActorRotation(); }
	virtual void SetRotation(const FRotator& _NewRotator)
	{
		if (!CanRotate())
			return;
		FRotator NewRotator = _NewRotator;
		NewRotator.Pitch = 0;
		NewRotator.Roll = 0;
		SetActorRotation(NewRotator);
	}

	void TiltBike(float _TiltRatio);

private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Rotation")
	bool bCanRotate = true;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Rotation")
	float MaxRotationSpeed = 60.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Rotation")
	float DefaultTiltRange = 30.f;	// 通常走行時の最大の傾き

	// バイクをデフォルトで傾けた状態にする (ドリフト時に使用)
	float TiltOffset = 0.f;

	// バイクの最大傾き (通常走行時とドリフト時で変化)
	float TiltRange;

	void SetTiltOffsetAndRange(float _TiltOffset, float _TiltRange)
	{
		TiltOffset = _TiltOffset;
		TiltRange = _TiltRange;
	}


	// IEnergy Implementation /////////////////////////////////////////////////////////////
public:
	virtual bool CanModifyEnergy() const { return bCanModifyEnergy; }
	virtual void SetEnergyModifiable(bool _bModifiable) { bCanModifyEnergy = _bModifiable; }

	virtual float GetEnergy() const override { return Energy; }
	virtual void SetEnergy(float _NewEnergy) override
	{
		if (!CanModifyEnergy())
			return;
		Energy = _NewEnergy;
		Energy = FMath::Clamp(Energy, 0.f, GetMaxEnergy());
		if(OnEnergyChanged.IsBound())
			OnEnergyChanged.Broadcast(Energy, GetMaxEnergy());
	}

	virtual float GetMaxEnergy() const { return MaxEnergy; }
	virtual void SetMaxEnergy(float _NewMaxEnergy) { MaxEnergy = _NewMaxEnergy; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEnergyChangedDelegate, float, NewEnergy, float, MaxEnergy);
	FEnergyChangedDelegate OnEnergyChanged;

private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Energy")
	bool bCanModifyEnergy = true;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Energy")
	float Energy;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Energy")
	float MaxEnergy = 1000.f;


	// IStoneCarryable Implementation /////////////////////////////////////////////////////////////
public:
	virtual AActor* GetActor() override { return this; }
	virtual AStone* GetCarryingStone() const override { return CarryingStone; }
	virtual void CarryStone(AStone* _Stone) override;
	virtual void ReleaseStone() override;

private:
	AStone* CarryingStone;


	// IBanditStickable Implementation ///////////////////////////////////////////////////////////
public:
	virtual bool IsStickable() const override { return bIsStickable; }
	virtual void SetStickable(bool _bStickable) override { bIsStickable = _bStickable; }
	virtual TArray<UBanditBand*> GetStickedBands() const override { return StickedBands; }

	virtual void AddStickedBand(UBanditBand* _StickedBand) override
	{
		if (!StickedBands.Contains(_StickedBand))
			StickedBands.Add(_StickedBand);
	}

	virtual void RemoveStickedBand(UBanditBand* _StickedBand) override
	{
		if (StickedBands.Contains(_StickedBand))
			StickedBands.Remove(_StickedBand);
	}

	virtual void OnBanditSticked(UBanditBand* _OtherBanditBand) override;
	virtual void OnBanditPulledEnter(UBanditBand* _OtherBanditBand) override;
	virtual void OnBanditPulledStay(UBanditBand* _OtherBanditBand, float _DeltaTime) override;
	virtual void OnBanditPulledExit(UBanditBand* _OtherBanditBand) override;

private:
	IMoveable* OtherMoveable;
	IRotatable* OtherRotatable;

	bool bIsStickable = true;
	bool bIsForceCut = false;
	TArray<UBanditBand*> StickedBands;

	UPROPERTY(EditAnywhere, Category = "Rider Properties|BanditBand")
	float AccelOnPullDashEnter = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Rider Properties|BanditBand")
	float AccelOnPullDashStay = 500.f;

	UPROPERTY(EditAnywhere, Category = "Rider Properties|BanditBand")
	float TurnSpeedOnPullDashStay = 30.f;

	UPROPERTY(EditAnywhere, Category = "Rider Properties|BanditBand")
	float PerfectCutLength = 500.f;

	UPROPERTY(EditAnywhere, Category = "Rider Properties|BanditBand")
	float EnergyStealRate = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Rider Properties|BanditBand")
	float EnergyStealRateOnPerfectCut = 0.75f;


	// Curve Accel ///////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Curve Accel")
	float CurveAcceleration;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Curve Accel")
	float MaxSpeedOffset;

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Curve Accel")
	float SpeedOffset;	// This value varies with the amount of tilt.

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Curve Accel")
	bool bCanAccelOnCurve = true;

private:
	void AccelSpeed(float TargetSpeed, float Acceleration, float DeltaTime);


	// Grounded //////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Grounded")
	bool bIsGrounded = false;

	// NotifyHitで値が変化するbool変数用
	// OnCollisionExitが無いため、boolをfalseにするタイミングはフレームの最後になる
	// 毎フレームの最後に常にfalseにならないようにするためのバッファ変数が必要
	bool bIsGroundedBuffer = false;

public:
	bool IsGrounded() const { return bIsGrounded; };


	// Collision ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Collision")
	float CollisionImpulse = 5000000.f;

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Collision")
	bool bCanBounce = true;	// Used to prevent getting multiple impulse on collision.


	// VFX //////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|Spark")
	float SparkTilt = 55.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|Spark")
	int MaxSparkRate = 500;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|Spark")
	int MinSparkRate = 100;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	FLinearColor AfterImageColor;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageMetallic = 0.5f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageRoughness = 0.2f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageOpacity = 0.3f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageLifetime = 0.2f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageInterval = 0.01f;


	// Actions ////////////////////////////////////////////////////////////////////////////////
public:
	enum class EDriftDirection : int8 {LEFT=-1, RIGHT=1};
	void StartDrift(EDriftDirection _DriftDirection);
	void StopDrift();
	bool IsDrifting();
	bool IsDrifting(EDriftDirection& _OutDriftDirection);
	void Jump();
	void Stun();

private:
	UPsmComponent::TPsmStateFunc LeftDriftState;
	void LeftDriftStateFunc(const FPsmInfo& Info);

	UPsmComponent::TPsmStateFunc RightDriftState;
	void RightDriftStateFunc(const FPsmInfo& Info);

	UPsmComponent::TPsmStateFunc StunState;
	void StunStateFunc(const FPsmInfo& Info);

	void OnEnterDrift(EDriftDirection _DriftDirection);
	void OnDrifting(EDriftDirection _DriftDirection, float _DeltaTime);
	void OnExitDrift(EDriftDirection _DriftDirection);

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Drift")
	float DriftImpulse = 500000.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Drift")
	float DriftMidTilt = 25.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Drift")
	float DriftTiltRange = 15.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Drift")
	float MaxDriftInertiaSpeed = 3000.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Jump")
	float JumpImpulse = 1500000.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Stun")
	float StunDuration = 2.f;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Stun")
	uint8 StunSpin = 6;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Stun")
	float StunBlinkInterval = 0.15f;
};
