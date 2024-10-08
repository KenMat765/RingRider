// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameInfo.h"
#include "Stone.generated.h"

class ARider;

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



	// Static Mesh //////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StoneMeshComp;

	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	float ZOffset = 288;



	// Tile Change Control ///////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere)
	bool bCanChangeTile = false;

	UPROPERTY(EditAnywhere)
	float RayDistance = 1000.f;

public:
	bool CanChangeTile() const { return bCanChangeTile; }
	void SetCanChangeTile(bool CanChangeTile) { bCanChangeTile = CanChangeTile; }



	// Team //////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	ETeam Team = ETeam::Team_None;

public:
	ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam NewTeam) { Team = NewTeam; }



	// Energy ////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category = "Stone Properties")
	float DecreaseEnergyPerSec = 20;



	// Owner Rider ///////////////////////////////////////////////////////////////////////////////////////////
private:
	ARider* OwnerRider;
};
