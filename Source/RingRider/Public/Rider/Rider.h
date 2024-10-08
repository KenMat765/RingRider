// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PsmComponent.h"
#include "GameInfo.h"
#include "Rider.generated.h"


class UBoxComponent;
class UCameraComponent;
class USpringArmComponent;
class UNiagaraComponent;
class UAfterImageComponent;


UCLASS()
class RINGRIDER_API ARider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called on collision
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

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	UPsmComponent* Psm;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SparkComp;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SpinComp;

	UPROPERTY(VisibleAnywhere)
	UAfterImageComponent* ImageComp;



	// Team ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditInstanceOnly, Category="Rider Properties")
	ETeam Team;

public:
	ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam NewTeam) { Team = NewTeam; }



	// Energy ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditInstanceOnly, Category="Rider Properties")
	float Energy;

public:
	float GetEnergy() const { return Energy; }
	void SetEnergy(float NewEnergy) { Energy = NewEnergy; }
	void AddEnergy(float DeltaEnergy) { Energy += DeltaEnergy; }



	// Properties ////////////////////////////////////////////////////////////////////////////////
public:
	UPROPERTY(EditInstanceOnly, Category="Rider Properties")
	float DefaultSpeed;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties")
	float MaxRotationSpeed;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties")
	float MaxTilt;	// 通常走行時の最大の傾き

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Curve Accel")
	float CurveAcceleration;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Curve Accel")
	float CurveDeceleration;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Curve Accel")
	float MaxSpeedOffset;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Collision")
	float CollisionImpulse;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Jump")
	float JumpImpulse;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Slide")
	float SlideDuration;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Slide")
	float SlideMaxSpeed;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Slide")
	class UCurveFloat* SlideCurve;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Slide")
	float SlideTilt;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Boost")
	float BoostDuration;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Boost")
	float BoostMaxDeltaSpeed;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Boost")
	class UCurveFloat* BoostCurve;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Boost")
	float BoostMaxPitch;	// ブースト時の最大ピッチ角

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Drift")
	float DriftImpulse;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Drift")
	float DriftMidTilt;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Drift")
	float DriftTiltRange;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Drift")
	float DriftInertiaSpeed;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|Spark")
	float SparkTilt;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|Spark")
	int MaxSparkRate;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|Spark")
	int MinSparkRate;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
	FLinearColor AfterImageColor;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
	float AfterImageMetallic;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
	float AfterImageRoughness;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
	float AfterImageOpacity;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
	float AfterImageLifetime;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
	float AfterImageInterval;



	// States ////////////////////////////////////////////////////////////////////////////////
private:
	UPsmComponent::TStateFunc SlideState;
	void SlideStateFunc(const FPsmInfo& Info);

	UPsmComponent::TStateFunc JumpState;
	void JumpStateFunc(const FPsmInfo& Info);

	UPsmComponent::TStateFunc BoostState;
	void BoostStateFunc(const FPsmInfo& Info);

	UPsmComponent::TStateFunc DriftState;
	void DriftStateFunc(const FPsmInfo& Info);



	// Input Events ///////////////////////////////////////////////////////////////////////////
private:
	void OnSwipeUp();
	void OnSwipeDown();
	void OnSwipeLeft();
	void OnSwipeRight();

	void OnJoyStick(float AxisValue);

	float StickValue;



	// Private Properties /////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleInstanceOnly, Category="Readonly Properties")
	float Speed;
	
	// DefaultSpeed + MaxSpeedOffset (カーブによる加速分) + BoostMaxDeltaSpeed (ブーストによる加速分)
	UPROPERTY(VisibleInstanceOnly, Category="Readonly Properties")
	float MaxSpeed;	

	UPROPERTY(VisibleInstanceOnly, Category="Readonly Properties")
	float SpeedOffset;	// This value varies with the amount of tilt.

	UPROPERTY(VisibleInstanceOnly, Category="Readonly Properties")
	bool bIsGrounded;

	UPROPERTY(VisibleInstanceOnly, Category="Readonly Properties")
	bool bCanCurve;

	UPROPERTY(VisibleInstanceOnly, Category="Readonly Properties")
	bool bCanTilt;

	UPROPERTY(VisibleInstanceOnly, Category="Readonly Properties")
	bool bCanBounce;	// Used to prevent getting multiple impulse on collision.

	UPROPERTY(VisibleInstanceOnly, Category="Readonly Properties")
	bool bCanAccelOnCurve;

	UPROPERTY(VisibleInstanceOnly, Category="Readonly Properties")
	bool bCanMoveForward;

	// NotifyHitで値が変化するbool変数用
	// OnCollisionExitが無いため、boolをfalseにするタイミングはフレームの最後になる
	// 毎フレームの最後に常にfalseにならないようにするためのバッファ変数が必要
	bool bIsGroundedBuffer;

public:
	bool IsGrounded() const;
	float GetSpeed() const;
	float GetSpeedOffset() const;
	UStaticMesh* GetStaticMesh() const;



	// Constants ////////////////////////////////////////////////////////////////////////////////
public:
	const static float BIKE_RADIUS;
	const static FName RIDER_TAG;

private:
	const static FName SPARK_SPAWN_RATE;
};
