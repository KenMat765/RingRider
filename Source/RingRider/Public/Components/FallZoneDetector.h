// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FallZoneDetector.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RINGRIDER_API UFallZoneDetector : public USceneComponent
{
	GENERATED_BODY()

public:	
	UFallZoneDetector();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;


public:
	bool DetectedFallZone() const { return bDetectedFallZone; }
	bool DetectedFallZone(FVector& _FallZonePos) const
	{
		_FallZonePos = bDetectedFallZone ? FallPoint : FVector::ZeroVector;
		return bDetectedFallZone;
	}
	// Returns zero vector when not detecting fall zone.
	FVector GetFallPoint() const
	{
		return bDetectedFallZone ? FallPoint : FVector::ZeroVector;
	}

	bool CheckFallZone() const ;
	bool CheckFallZone(FVector& _FallZonePos) const;

#if WITH_EDITOR
	UPROPERTY(EditAnywhere, Category = "Check Point", meta = (
		ToolTip = "FOR DEBUG: This is for visualizing line trace (Fall zone checking will run in Tick when enabled)"))
	bool bVisualizeDebugDraw = false;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "Check Point")
	uint8 CheckPointNum = 3;

	UPROPERTY(EditAnywhere, Category = "Check Point")
	float CheckPointRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Check Point")
	float CheckPointRayDistance = 100.f;

	UPROPERTY(EditAnywhere, Category = "Check Point")
	TEnumAsByte<ECollisionChannel> CheckObjectType;


private:
	TArray<FVector> CheckPoints;
	void UpdateCheckPointsLocation();

	bool bDetectedFallZone;
	FVector FallPoint;
};
