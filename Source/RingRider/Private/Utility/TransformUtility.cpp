#include "Utility/TransformUtility.h"
#include "Kismet/KismetMathLibrary.h"

namespace FVectorUtility
{
	float RadianBetweenVectors(const FVector& VectorFrom, const FVector& VectorTo)
	{
		FVector VectorFrom_Norm = VectorFrom.GetSafeNormal();
		FVector VectorTo_Norm   = VectorTo.GetSafeNormal();
		if (VectorFrom_Norm == FVector::ZeroVector || VectorTo_Norm == FVector::ZeroVector)
			return 0;

		float DotProduct = FVector::DotProduct(VectorFrom_Norm, VectorTo_Norm);
		float RadAngle = FMath::Acos(DotProduct);
		return RadAngle;
	}
}

namespace FRotatorUtility
{
	FRotator GetLookAtRotator(AActor* _Actor, const FVector& _LookPos, float _DeltaTime, float _RotateSpeed)
	{
		if (!_Actor)
			return FRotator::ZeroRotator;

		FVector RelativeDirection = _LookPos - _Actor->GetActorLocation();
		FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(RelativeDirection);
		FRotator LookAtRotation = FMath::RInterpTo(_Actor->GetActorRotation(), TargetRotation, _DeltaTime, _RotateSpeed);
		return LookAtRotation;
	}
}