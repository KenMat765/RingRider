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
void ALevelMaker::AlignTiles(bool bIsQuater)
{
	if (AlignStartRadius > AlignEndRadius)
	{
		UE_LOG(LogLevelMaker, Error, TEXT("AlignStartRadius must be equal or smaller than AlignEndRadius. Aborted."));
		return;
	}

	if (FolderName.IsNone())
	{
		UE_LOG(LogLevelMaker, Warning, TEXT("FolderName was not specified. Aborted."));
		return;
	}

	// フォルダーが空でなければ空にする
	TArray<AActor*> TilesInFolder = FActorUtility::GetActorsInFolder(GetWorld(), FolderName);
	if (TilesInFolder.Num() > 0)
	{
		for (auto TileInFolder : TilesInFolder)
		{
			TileInFolder->Destroy();
		}
	}

	// 中心のタイルを生成
	if (AlignStartRadius == 1)
	{
		auto CenterTile = GetWorld()->SpawnActor<AHexTile>(FVector::ZeroVector, FRotator::ZeroRotator);
		CenterTile->SetFolderPath(FolderName);
	}

	// 周囲のタイルを生成していく
	for (int r = AlignStartRadius-1; r < AlignEndRadius; r++)
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

				// 誤差でタイルの位置がX軸上にあるべきなのに若干ズレる場合がある (ミラーする場合に問題が生じる) ので、Yが十分小さかったら0にする
				if (FMath::Abs(SpawnLoc.Y) < 0.01f)
					SpawnLoc.Y = 0;

				if (bIsQuater && (SpawnLoc.X < 0 || SpawnLoc.Y < 0))
					continue;
				auto SpawnedTile = GetWorld()->SpawnActor<AHexTile>(SpawnLoc, FRotator::ZeroRotator);
				SpawnedTile->SetFolderPath(FolderName);
			}

			// 次の頂点の位置へ移動
			VertLoc += Dir * TileWidth() * r;
		}
	}
}



// Mirror ////////////////////////////////////////////////////////////////////////////////////////////
void ALevelMaker::MirrorTiles(uint8 MirrorAxis)
{
	if (FolderName.IsNone())
	{
		UE_LOG(LogLevelMaker, Warning, TEXT("FolderName was not specified. Aborted."));
		return;
	}

	// 引数に応じてミラー軸を決定する
	FVector MirrorVector;
	switch (MirrorAxis)
	{
	case 0: MirrorVector = FVector(-1, 1, 1); break;
	case 1: MirrorVector = FVector(1, -1, 1); break;
	default: UE_LOG(LogLevelMaker, Warning, TEXT("MirrorTiles: Argument out of range. Aborted.")) return;
	}

	// 位置をミラーしたタイルを生成
	TArray<AActor*> TilesInFolder = FActorUtility::GetActorsInFolder(GetWorld(), FolderName);
	for (auto TileInFolder : TilesInFolder)
	{
		FVector TileLocation = TileInFolder->GetActorLocation();
		FVector MirroredLocation = TileLocation * MirrorVector;
		if (TileLocation == MirroredLocation)
			continue;
		auto MirroredTile = GetWorld()->SpawnActor<AHexTile>(MirroredLocation, FRotator::ZeroRotator);
		MirroredTile->SetFolderPath(FolderName);
	}
}



// Create LevelInstance //////////////////////////////////////////////////////////////////////////////
void ALevelMaker::CreateLevelInstance()
{
	if (FolderName.IsNone())
	{
		UE_LOG(LogLevelMaker, Warning, TEXT("FolderName was not specified. Aborted."));
		return;
	}

	TArray<AActor*> TilesInFolder = FActorUtility::GetActorsInFolder(GetWorld(), FolderName);
	if (TilesInFolder.Num() == 0)
	{
		UE_LOG(LogLevelMaker, Warning, TEXT("Folder: %s does not exist or empty. Aborted."), *FolderName.ToString());
		return;
	}

	AActor* LevelInstanceActor = GetWorld()->SpawnActor<ALevelInstance>(FVector::ZeroVector, FRotator::ZeroRotator);
	ALevelInstance* LevelInstance = Cast<ALevelInstance>(LevelInstanceActor);
	for (auto TileInFolder : TilesInFolder)
	{
		FTransform TileTrans = TileInFolder->GetActorTransform();
		LevelInstance->AddTile(TileTrans);
	}
}

