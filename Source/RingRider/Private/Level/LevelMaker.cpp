// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LevelMaker.h"
#include "Level/LevelInstance.h"
#include "Level/HexTile.h"
#include "Utility/ActorUtility.h"


DEFINE_LOG_CATEGORY(LogLevelMaker);


ALevelMaker::ALevelMaker()
{
	PrimaryActorTick.bCanEverTick = false;
}



// Align Tiles ///////////////////////////////////////////////////////////////////////////////////
void ALevelMaker::AlignTiles()
{
	if (FolderName.IsNone())
	{
		UE_LOG(LogLevelMaker, Warning, TEXT("FolderName was not specified. Aborted."));
		return;
	}

	UE_LOG(LogLevelMaker, Log, TEXT("Started aligning tiles..."));

	// フォルダーが空でなければ空にする
	TArray<AActor*> TilesInFolder = FActorUtility::GetActorsInFolder(GetWorld(), FolderName);
	if (TilesInFolder.Num() > 0)
	{
		UE_LOG(LogLevelMaker, Log, TEXT("Emptying Folder: %s"), *FolderName.ToString());
		for (auto TileInFolder : TilesInFolder)
		{
			TileInFolder->Destroy();
		}
	}

	// 中心のタイルを生成 (r == 0)
	auto CenterTile = GetWorld()->SpawnActor<AHexTile>(FVector::ZeroVector, FRotator::ZeroRotator);
	CenterTile->SetFolderPath(FolderName);

	// 周囲のタイルを生成していく
	for (int r = 1; r < AlignRadius; r++)
	{
		// 最初の頂点の位置 (一番右)
		FVector VertLoc(0, TileWidth() * r, 0);
		for (int v = 1; v <= 6; v++)
		{
			// 辺の方向を計算
			float Cos, Sin;
			float Angle = FMath::DegreesToRadians(60 * (v + 1));	// 120, 180, 240, 300, 360, 60
			FMath::SinCos(&Sin, &Cos, Angle);
			FVector Dir(Sin, Cos, 0);

			// 辺の方向にタイルを生成していく
			for (int e = 0; e < r; e++)
			{
				FVector SpawnLoc = VertLoc + Dir * TileWidth() * e;
				auto SpawnedTile = GetWorld()->SpawnActor<AHexTile>(SpawnLoc, FRotator::ZeroRotator);
				SpawnedTile->SetFolderPath(FolderName);
			}

			// 次の頂点の位置へ移動
			VertLoc += Dir * TileWidth() * r;
		}
	}

	UE_LOG(LogLevelMaker, Log, TEXT("Finished aligning tiles!!"));
}



// Create LevelInstance //////////////////////////////////////////////////////////////////////////////
void ALevelMaker::CreateLevelInstance()
{
	if (FolderName.IsNone())
	{
		UE_LOG(LogLevelMaker, Warning, TEXT("FolderName was not specified. Aborted."));
		return;
	}

	UE_LOG(LogLevelMaker, Log, TEXT("Started creating LevelInstance..."));

	TArray<AActor*> TilesInFolder = FActorUtility::GetActorsInFolder(GetWorld(), FolderName);
	if (TilesInFolder.Num() == 0)
	{
		UE_LOG(LogLevelMaker, Warning, TEXT("Folder: %s does not exist. Aborted."), *FolderName.ToString());
		return;
	}

	AActor* LevelInstanceActor = GetWorld()->SpawnActor<ALevelInstance>(FVector::ZeroVector, FRotator::ZeroRotator);
	ALevelInstance* LevelInstance = Cast<ALevelInstance>(LevelInstanceActor);
	for (auto TileInFolder : TilesInFolder)
	{
		FTransform TileTrans = TileInFolder->GetActorTransform();
		LevelInstance->AddTile(TileTrans);
	}

	UE_LOG(LogLevelMaker, Log, TEXT("Finished creating LevelInstance!!"));
}

