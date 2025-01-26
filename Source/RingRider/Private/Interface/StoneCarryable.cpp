// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/StoneCarryable.h"

bool IStoneCarryable::IsCarryingStone() const
{
	return GetCarryingStone() != nullptr;
}
