#include "Utility/TransformUtility.h"
#include "Kismet/KismetMathLibrary.h"

namespace FVectorUtility
{
	float RadianBetweenVectors(const FVector& _VectorFrom, const FVector& _VectorTo)
	{
		FVector VectorFrom_Norm = _VectorFrom.GetSafeNormal();
		FVector VectorTo_Norm   = _VectorTo.GetSafeNormal();
		if (VectorFrom_Norm == FVector::ZeroVector || VectorTo_Norm == FVector::ZeroVector)
			return 0;

		float DotProduct = FVector::DotProduct(VectorFrom_Norm, VectorTo_Norm);
		float RadAngle = FMath::Acos(DotProduct);
		return RadAngle;
	}

	float RadianBetweenVectors(const FVector2D& _VectorFrom, const FVector2D& _VectorTo)
	{
		FVector2D VectorFrom_Norm = _VectorFrom.GetSafeNormal();
		FVector2D VectorTo_Norm   = _VectorTo.GetSafeNormal();
		if (VectorFrom_Norm == FVector2D::ZeroVector || VectorTo_Norm == FVector2D::ZeroVector)
			return 0;

		float DotProduct = FVector2D::DotProduct(VectorFrom_Norm, VectorTo_Norm);
		float RadAngle = FMath::Acos(DotProduct);
		return RadAngle;
	}

	void Cartesian2Polar(const FVector2D& _InputVector, float& _OutputLength, float& _OutputRadian)
	{
		_OutputLength = _InputVector.Size();
		_OutputRadian = RadianBetweenVectors(FVector2D(1.f, 0.f), _InputVector);
	}

	void Polar2Cartesian(float _InputLength, float _InputRadian, FVector2D& _OutputVector)
	{
		_OutputVector.X = _InputLength * FMath::Cos(_InputRadian);
		_OutputVector.Y = _InputLength * FMath::Sin(_InputRadian);
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