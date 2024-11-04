// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameInfo.h"
#include "Stone.generated.h"

class ARider;
class UDestructibleComponent;

/*:
* タッチしたRiderに対して以下を実行
* １．頭上を追尾
* ２．Riderが触れたタイルのチームを変更
*/
UCLASS()
class RINGRIDER_API AStone : public AActor
{
	GENERATED_BODY()
	
public:	
	AStone();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& Hit
	);



	// Destructible Mesh /////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere)
	UDestructibleComponent* StoneDestructComp;

	UPROPERTY(EditAnywhere, Category = "Stone Properties|Destruction")
	float DestructImpulse = 20000.0f;

	void DestructStone();

	UPROPERTY(EditAnywhere, Category = "Stone Properties|Destruction")
	float DestroyDelay = 3.0f;

	FTimerHandle TimerHandle;

	void DestroyStone();



	// Tile Change Control ///////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category = "Stone Properties|Tile Change")
	bool bCanChangeTile = false;

	UPROPERTY(EditAnywhere, Category = "Stone Properties|Tile Change")
	float RayDistance = 1000.f;

public:
	bool CanChangeTile() const { return bCanChangeTile; }
	void SetCanChangeTile(bool CanChangeTile) { bCanChangeTile = CanChangeTile; }



	// Team //////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category = "Stone Properties|Team")
	ETeam Team = ETeam::Team_None;

public:
	ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam NewTeam) { Team = NewTeam; }



	// Energy ////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category = "Stone Properties|Energy")
	float DecreaseEnergyPerSec = 20;



	// Owner Rider ///////////////////////////////////////////////////////////////////////////////////////////
private:
	ARider* OwnerRider;

	UPROPERTY(EditAnywhere, Category = "Stone Properties|Rider")
	float ZOffset = 260;

	UPROPERTY(EditAnywhere, Category = "Stone Properties|Rider")
	float ChaseRatio = 0.8f;

	void OnOwnedByRider(ARider* NewOwnerRider);



	// Z Offset Animation //////////////////////////////////////////////////////////////////////////////////////
private:
	bool Animating = false;
	float AnimTimer = 0.f;

	UPROPERTY(EditAnywhere, Category = "Stone Properties|Animation")
	UCurveFloat* AnimCurve;

	UPROPERTY(EditAnywhere, Category = "Stone Properties|Animation")
	float AnimDuration = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Stone Properties|Animation")
	float AnimMaxZOffset = 350.f;

	void StartZOffsetAnimation()
	{
		AnimTimer = 0.f;
		Animating = true;
	}

	void StopZOffsetAnimation()
	{
		Animating = false;
	}
};
