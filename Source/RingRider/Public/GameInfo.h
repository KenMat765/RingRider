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


#define TAG_RIDER TEXT("Rider")
#define TAG_GROUND TEXT("Ground")
#define TAG_HEXTILE TEXT("HexTile")
#define TAG_BOUNCE TEXT("Bounce")
#define TAG_BIKE TEXT("Bike")
