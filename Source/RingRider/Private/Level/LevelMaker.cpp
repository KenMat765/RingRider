// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LevelMaker.h"
#include "Level/HexTile.h"
#include "UtilityActors/EmptyActor.h"


DEFINE_LOG_CATEGORY(LogLevelMaker);


ALevelMaker::ALevelMaker()
{
	PrimaryActorTick.bCanEverTick = false;
}



// Align Tiles ///////////////////////////////////////////////////////////////////////////////////
void ALevelMaker::AlignTiles()
{
	if (Radius <= 0)
	{
		UE_LOG(LogLevelMaker, Warning, TEXT("Radius must be larger than 0!!"));
		UE_LOG(LogLevelMaker, Warning, TEXT("Aborting"));
		return;
	}

	UE_LOG(LogLevelMaker, Log, TEXT("Started aligning tiles..."));

	// ��������^�C���B�̐e�A�N�^�[�𐶐�
	AActor* ParentActor = GetWorld()->SpawnActor<AEmptyActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	USceneComponent* SceneComp = NewObject<USceneComponent>(this);
	SceneComp->RegisterComponent();

	// ���S�̃^�C���𐶐� (r == 0)
	AActor* CenterTile = GetWorld()->SpawnActor<AHexTile>(FVector::ZeroVector, FRotator::ZeroRotator);
	CenterTile->AttachToActor(ParentActor, FAttachmentTransformRules::KeepRelativeTransform);

	// ���͂̃^�C���𐶐����Ă���
	for (int r = 1; r < Radius; r++)
	{
		// �ŏ��̒��_�̈ʒu (��ԉE)
		FVector VertLoc(0, TileWidth() * r, 0);
		for (int v = 1; v <= 6; v++)
		{
			// �ӂ̕������v�Z
			float Cos, Sin;
			float Angle = FMath::DegreesToRadians(60 * (v + 1));	// 120, 180, 240, 300, 360, 60
			FMath::SinCos(&Sin, &Cos, Angle);
			FVector Dir(Sin, Cos, 0);

			// �ӂ̕����Ƀ^�C���𐶐����Ă���
			for (int e = 0; e < r; e++)
			{
				FVector SpawnLoc = VertLoc + Dir * TileWidth() * e;
				AActor* SpawnedTile = GetWorld()->SpawnActor<AHexTile>(SpawnLoc, FRotator::ZeroRotator);
				SpawnedTile->AttachToActor(ParentActor, FAttachmentTransformRules::KeepRelativeTransform);
			}

			// ���̒��_�̈ʒu�ֈړ�
			VertLoc += Dir * TileWidth() * r;
		}
	}

	UE_LOG(LogLevelMaker, Log, TEXT("Finished aligning tiles!!"));
}

