#pragma once

#include "CoreMinimal.h"
#include "GameInfo.generated.h"


UENUM(BlueprintType)
enum class ETeam : uint8
{
	Team_None UMETA(DisplayName = "Team None"),
	Team_1 UMETA(DisplayName = "Team 1"),
	Team_2 UMETA(DisplayName = "Team 2")
};


struct FTagList
{
	static const FName TAG_RIDER;
	static const FName TAG_GROUND;
	static const FName TAG_HEXTILE;
	static const FName TAG_BOUNCE;
	static const FName TAG_LOCKON;
};
