// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/Moveable.h"

float IMoveable::GetMaxSpeed() const { return 0; }
void IMoveable::SetMaxSpeed(float _NewSpeed) {}

void IMoveable::MoveForward(float DeltaTime) {}
void IMoveable::MoveToward(const FVector& _TargetPos, float DeltaTime) {}
