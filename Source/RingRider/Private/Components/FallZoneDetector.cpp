// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FallZoneDetector.h"
#include "DrawDebugHelpers.h"


UFallZoneDetector::UFallZoneDetector()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFallZoneDetector::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UpdateCheckPointsLocation();
	bDetectedFallZone = CheckFallZone(FallPoint);
}

void UFallZoneDetector::BeginPlay()
{
	Super::BeginPlay();

	CheckPoints.Empty();
	for (uint8 k = 0; k < CheckPointNum; k++)
		CheckPoints.Add(FVector::ZeroVector);
	UpdateCheckPointsLocation();
}


inline bool UFallZoneDetector::CheckFallZone() const
{
	FVector TempVec;
	return CheckFallZone(TempVec);
}

bool UFallZoneDetector::CheckFallZone(FVector& _FallZonePos) const
{
	const FVector COMP_DOWN = GetUpVector() * -1;

	// ���m�Ώ�(��ɒn��)�̐ݒ�
	FCollisionObjectQueryParams ObjQueryParam;
	ObjQueryParam.AddObjectTypesToQuery(CheckObjectType);

	// LineTrace�Œn�ʂ����m���A���m�ł��Ȃ���Η����n�_�Ƃ��ēo�^
	TArray<FVector> DetectedFallPoints;
	for (FVector CheckPoint : CheckPoints)
	{
		FVector RayStart = CheckPoint;
		FVector RayEnd = CheckPoint + COMP_DOWN * CheckPointRayDistance;
		FHitResult Hit;
		bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, RayStart, RayEnd, ObjQueryParam);
		if (!bHit)
			DetectedFallPoints.Add(CheckPoint);

#if WITH_EDITOR
		if (bVisualizeDebugDraw)
		{
			FColor LineColor = bHit ? FColor::Green : FColor::Red;
			float TickInterval = GetComponentTickInterval();
			DrawDebugSphere(GetWorld(), RayStart, 10.f, 8, LineColor, false, TickInterval);
			DrawDebugLine(GetWorld(), RayStart, RayEnd, LineColor, false, TickInterval);
		}
#endif
	}

	// �����n�_���L��΁A���̕��ς𗎉��n�_�Ƃ���_FallZonePos�ɃZ�b�g
	if (DetectedFallPoints.Num() > 0)
	{
		FVector FallPointSum;
		for (FVector DetectedFallPoint : DetectedFallPoints)
			FallPointSum += DetectedFallPoint;
		_FallZonePos = FallPointSum / DetectedFallPoints.Num();
		return true;
	}
	return false;
}

void UFallZoneDetector::UpdateCheckPointsLocation()
{
	// �`�F�b�N�|�C���g�� -90�x �` 90�x (�O��180�x) �͈̔͂ɔz�u
	const float DEG_BETWEEN_POINTS = 180.f / (CheckPointNum - 1);
	FVector CompForward = GetForwardVector();
	FVector CompUp = GetUpVector();
	for (uint8 k = 0; k < CheckPointNum; k++)
	{
		float CheckPointDeg = -90.f + DEG_BETWEEN_POINTS * k;
		FQuat RotationQuat = FQuat(CompUp, FMath::DegreesToRadians(CheckPointDeg));
		CheckPoints[k] = GetComponentLocation() + RotationQuat.RotateVector(CompForward) * CheckPointRadius;
	}
}

