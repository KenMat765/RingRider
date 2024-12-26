#include "Utility/VectorUtility.h"

namespace FVectorUtility
{
	float RadianBetweenVectors(FVector VectorFrom, FVector VectorTo)
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