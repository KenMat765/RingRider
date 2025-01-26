// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Gimmick/Stone.h"
#include "StoneCarryable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStoneCarryable : public UInterface
{
	GENERATED_BODY()
};


class RINGRIDER_API IStoneCarryable
{
	GENERATED_BODY()

public:
	virtual bool IsCarryingStone() const = 0;
	virtual AStone* GetCarryingStone() const = 0;
	virtual void CarryStone(AStone* _Stone) = 0;
	virtual void ReleaseStone() = 0;
};
