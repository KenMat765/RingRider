// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PsmComponent.h"
#include "GameInfo.h"
#include "Interface/Moveable.h"
#include "Interface/PhysicsMoveable.h"
#include "Rider.generated.h"


class UBoxComponent;
class USpringArmComponent;
class UNiagaraComponent;
class UAfterImageComponent;
class USearchLightComponent;
class UBanditBand;


DECLARE_MULTICAST_DELEGATE_TwoParams(FSpeedChangeDelegate, float, float)
DECLARE_MULTICAST_DELEGATE_TwoParams(FEnergyChangeDelegate, float, float)


UCLASS()
class RINGRIDER_API ARider : public APawn, public IMoveable, public IPhysicsMoveable
{
	GENERATED_BODY()

public:
	ARider();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyHit(
		class UPrimitiveComponent* MyComp,
		class AActor* Other,
		class UPrimitiveComponent* OtherComp,
		bool bSelfMoved,
		FVector HitLocation,
		FVector HitNormal,
		FVector NormalImpulse,
		const FHitResult& Hit
	) override;



	// Components //////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* RootBox;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* BikeBase;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Bike;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Wheel;

	UPsmComponent* Psm;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SparkComp;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SpinComp;

	UPROPERTY(VisibleAnywhere)
	UAfterImageComponent* ImageComp;

	UPROPERTY(VisibleAnywhere)
	USearchLightComponent* SearchLightComp;

	UPROPERTY(VisibleAnywhere)
	UBanditBand* BanditBand;


	// Team ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Team")
	ETeam Team;

public:
	ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam NewTeam) { Team = NewTeam; }



	// IMoveable Implementation /////////////////////////////////////////////////////////////
public:
	virtual bool CanMove() const override { return bCanMove; }
	virtual void SetCanMove(bool _CanMove) override { bCanMove = _CanMove; }

	virtual float GetSpeed() const override { return Speed; }
	virtual void SetSpeed(float _NewSpeed) override
	{
		Speed = _NewSpeed;
		Speed = FMath::Clamp(Speed, 0.f, MaxSpeed);
		TriggerOnSpeedChangeActions(Speed, MaxSpeed);
	}
	virtual void AddSpeed(float _DeltaSpeed) override
	{
		Speed += _DeltaSpeed;
		TriggerOnSpeedChangeActions(Speed, MaxSpeed);
	}

	virtual float GetMaxSpeed() const override { return MaxSpeed; }
	virtual void SetMaxSpeed(float _NewMaxSpeed) override { MaxSpeed = _NewMaxSpeed; }

	virtual void MoveForward(float DeltaTime) override;
	virtual void MoveToward(const FVector& _TargetPos, float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Movement")
	bool bCanMove;
	
	UPROPERTY(EditAnywhere, Category="Rider Properties|Movement")
	float DefaultSpeed;

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Movement")
	float Speed;

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Movement")
	float MaxSpeed;	

private:
	FSpeedChangeDelegate OnSpeedChangeActions;
	void TriggerOnSpeedChangeActions(float _NewSpeed, float _MaxSpeed) const;

public:
	FDelegateHandle AddOnSpeedChangeAction(TFunction<void(float, float)> NewFunc);
	void RemoveOnSpeedChangeAction(FDelegateHandle DelegateHandle);



	// IPhysicsMoveable Implementation /////////////////////////////////////////////////////////////
public:
	virtual UPrimitiveComponent* GetPrimitiveComp() const override;



	// Energy ///////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Energy")
	float Energy;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Energy")
	float MaxEnergy;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Energy")
	float EnergyStealRate;

public:
	float GetEnergy() const { return Energy; }
	void SetEnergy(float NewEnergy) { Energy = NewEnergy; TriggerOnEnergyChangeActions(Energy, MaxEnergy); }
	void AddEnergy(float DeltaEnergy) { Energy += DeltaEnergy; TriggerOnEnergyChangeActions(Energy, MaxEnergy); }

	float GetMaxEnergy() const { return MaxEnergy; }
	void SetMaxEnergy(float NewMaxEnergy) { MaxEnergy = NewMaxEnergy; }

	float GetEnergyStealRate() const { return EnergyStealRate; }
	void SetEnergyStealRate(float NewStealRate) { EnergyStealRate = NewStealRate; }

private:
	FEnergyChangeDelegate OnEnergyChangeActions;
	void TriggerOnEnergyChangeActions(float _NewEnergy, float _MaxEnergy) const;

public:
	FDelegateHandle AddOnEnergyChangeAction(TFunction<void(float, float)> NewFunc);
	void RemoveOnEnergyChangeAction(FDelegateHandle DelegateHandle);

private:
	void StealEnergy(ARider* RiderToStealFrom);



	// Tilt & Rotation ///////////////////////////////////////////////////////////////////////////
public:
	void TiltBike(float TiltRatio) const;

private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Rotation")
	float MaxRotationSpeed;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Rotation")
	float DefaultTiltRange;	// 通常走行時の最大の傾き

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Rotation")
	bool bCanCurve = true;

	// バイクをデフォルトで傾けた状態にする (ドリフト時に使用)
	float TiltOffset = 0.f;

	// バイクの最大傾き (通常走行時とドリフト時で変化)
	float TiltRange;

	void SetTiltOffsetAndRange(float _TiltOffset, float _TiltRange)
	{
		TiltOffset = _TiltOffset;
		TiltRange = _TiltRange;
	}



	// Curve Accel ///////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Curve Accel")
	float CurveAcceleration;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Curve Accel")
	float CurveDeceleration;

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
	float CollisionImpulse;

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Collision")
	bool bCanBounce = true;	// Used to prevent getting multiple impulse on collision.



	// Properties ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|Spark")
	float SparkTilt;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|Spark")
	int MaxSparkRate;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|Spark")
	int MinSparkRate;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	FLinearColor AfterImageColor;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageMetallic;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageRoughness;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageOpacity;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageLifetime;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageInterval;



	// Actions ////////////////////////////////////////////////////////////////////////////////
public:
	enum class EDriftDirection : int8 {LEFT=-1, RIGHT=1};
	void StartDrift(EDriftDirection _DriftDirection);
	void StopDrift();
	bool IsDrifting(EDriftDirection& _OutDriftDirection);
	void Jump();
	bool IsBoosting() { return Psm->IsStateOn(BoostState); }

private:
	UPsmComponent::TPsmStateFunc SlideState;
	void SlideStateFunc(const FPsmInfo& Info);

	UPsmComponent::TPsmStateFunc BoostState;
	void BoostStateFunc(const FPsmInfo& Info);

	UPsmComponent::TPsmStateFunc LeftDriftState;
	void LeftDriftStateFunc(const FPsmInfo& Info);

	UPsmComponent::TPsmStateFunc RightDriftState;
	void RightDriftStateFunc(const FPsmInfo& Info);

	void OnEnterDrift(EDriftDirection _DriftDirection);
	void OnDrifting(EDriftDirection _DriftDirection, float _DeltaTime);
	void OnExitDrift(EDriftDirection _DriftDirection);

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Drift")
	float DriftImpulse;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Drift")
	float DriftMidTilt;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Drift")
	float DriftTiltRange;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Drift")
	float DriftInertiaSpeed;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Jump")
	float JumpImpulse;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Slide")
	float SlideDuration;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Slide")
	float SlideMaxSpeed;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Slide")
	class UCurveFloat* SlideCurve;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Slide")
	float SlideTilt;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Boost")
	float BoostSpeed;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Boost")
	float BoostPitch;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Boost")
	float BoostEnterEnergy;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Boost")
	float BoostStayEnergyPerSec;



	// Lock On ////////////////////////////////////////////////////////////////////////////////
private:
	TArray<AActor*> TargetActors;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Lock On")
	float LockOnRadius;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Lock On")
	float LockOnAngle;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Lock On")
	float LockOnAssistStrength;

private:
	TArray<AActor*> SearchTargetActor(float Radius, float Angle);



	// Constants ////////////////////////////////////////////////////////////////////////////////
public:
	const static float BIKE_RADIUS;

private:
	const static FName SPARK_SPAWN_RATE;
};
