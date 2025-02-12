// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_EUW_CheckPlacedActor.h
 * @brief 配置Actorに対するチェック
 * @author SensyuGames
 * @date 2024/11/30
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CSDebug_EUW_Base.h"
#include "CSDebug_EUW_CheckPlacedActor.generated.h"

USTRUCT(BlueprintType)
struct CSDEBUGEDITOR_API FCSDebug_PlacedActorClassList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckPlacedActor")
	TArray<TSoftClassPtr<AActor>> mClassList;
};

UCLASS()
class CSDEBUGEDITOR_API UCSDebug_EUW_CheckPlacedActor : public UCSDebug_EUW_Base
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetupPlacedActorClassMap();
	UFUNCTION(BlueprintCallable)
	bool CheckError();

protected:
	bool CheckError_PossibleToPlaceActor();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckPlacedActor")
	TMap<FString, FCSDebug_PlacedActorClassList> mPlacedActorClassMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckPlacedActor")
	TMap<FString, FCSDebug_PlacedActorClassList> mPossibleToPlaceActorClassMap;
};
