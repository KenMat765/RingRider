// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Rider.generated.h"

class UBoxComponent;
class UCameraComponent;
class USpringArmComponent;

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
		const FHitResult& Hit) override;



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



	// Properties ////////////////////////////////////////////////////////////////////////////////
public:
	UPROPERTY(EditInstanceOnly, Category="Runner Editable Properties")
	float DefaultSpeed;

	UPROPERTY(EditInstanceOnly, Category="Runner Editable Properties")
	float MaxRotationSpeed;

	UPROPERTY(EditInstanceOnly, Category="Runner Editable Properties")
	float MaxTilt;

	UPROPERTY(EditInstanceOnly, Category="Runner Editable Properties")
	float JumpImpulse;

	UPROPERTY(EditInstanceOnly, Category="Runner Editable Properties")
	float CurveAcceleration;

	UPROPERTY(EditInstanceOnly, Category="Runner Editable Properties")
	float CurveDeceleration;

	UPROPERTY(EditInstanceOnly, Category="Runner Editable Properties")
	float MaxSpeedOffset;

	UPROPERTY(EditInstanceOnly, Category="Runner Editable Properties")
	float CollisionImpulse;



	// Movements ////////////////////////////////////////////////////////////////////////////////
public:
	void Lean(float TiltRatio);
	void Jump(float Impulse);



	// Input Events ////////////////////////////////////////////////////////////////////////////
private:
	void OnCurveInput(float AxisValue);
	void OnJumpInput();



	// Private Properties ///////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleInstanceOnly, Category="Runner Visible Properties")
	bool bIsGrounded;

	UPROPERTY(VisibleInstanceOnly, Category="Runner Visible Properties")
	float Speed;

	UPROPERTY(VisibleInstanceOnly, Category="Runner Visible Properties")
	float SpeedOffset;	// This value varies with the amount of tilt.

	bool bCanBounce;	// Used to prevent getting multiple impulse on collision.

public:
	bool IsGrounded() const;
	float GetSpeed() const;
	float GetSpeedOffset() const;



	// Constants ////////////////////////////////////////////////////////////////////////////////
	const static float BIKE_RADIUS;
};
