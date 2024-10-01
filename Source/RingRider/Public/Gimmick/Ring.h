// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ring.generated.h"


class UBoxComponent;
class ARider;
class UNiagaraComponent;


UCLASS()
class RINGRIDER_API ARing : public AActor
{
	GENERATED_BODY()
	
public:	
	ARing();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;



private:
	static constexpr int VertNum = 6;

	// Passed Rider
	ARider* PassedRider;

	UPROPERTY(VisibleAnywhere, Category="Ring Properties")
	bool bIsPassed;

	// Animation
	float AnimTimer;

	UPROPERTY(EditInstanceOnly, Category="Ring Properties|Animation")
	float AnimDuration;

	UPROPERTY(EditInstanceOnly, Category="Ring Properties|Animation")
	class UCurveFloat* AnimCurve;

	float StartScale;

	UPROPERTY(EditInstanceOnly, Category="Ring Properties|Animation")
	float RotateSpeed;



	// Components //////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* ColComps[VertNum];

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PassCheckComp;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* ObtainComp;



	// Overlap Events //////////////////////////////////////////////////////////////////////////////
private:
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherbodyIndex,
		bool bFromSweep,
		const FHitResult& Hit
	);



	// Rider Pass Events /////////////////////////////////////////////////////////////////////////
private:
	void OnRiderPassed(ARider* PassedRider);



	// VFX /////////////////////////////////////////////////////////////////////////////////////////
	static const FString STATIC_MESH_FROM;
	static const FString TARGET_POSITION;
};
