// Copyright 2020 SensyuGames.
/**
 * @file CSDebugSelectManager.h
 * @brief CSDebugSelect�Ǘ��pManager
 * @author SensyuGames
 * @date 2020/05/27
 */
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebugSelectManager.generated.h"

class UCanvas;
class UCSDebugSelectComponent;
class ADebugCameraController;
struct FCSDebugMenuNodeGetter;
/**
 * 
 */
UCLASS()
class CSDEBUG_API UCSDebugSelectManager : public UObject
{
	GENERATED_BODY()

#if USE_CSDEBUG
public:
	void	Init();
	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(class UCanvas* InCanvas);
	void	EntryDebugSelectComponent(UCSDebugSelectComponent* InComponent);
	void	ExitDebugSelectComponent(UCSDebugSelectComponent* InComponent);

	bool	IsShowPathFollow() const { return mbShowSelectPathFollow; }
	bool	IsShowLastEQS() const { return mbShowSelectLastEQS; }

protected:
	ADebugCameraController* GetActiveDebugCameraController() const;
	void	CheckDebugCameraController();
	void	CheckSelectTarget();
	void	OnSelect(AActor* InActor);
	void	SetActiveTickActor(AActor* InActor, const bool bInActive);

	void	DrawInfo(UCanvas* InCanvas);
	void	DrawSelectMarker(UCanvas* InCanvas);
	void	DrawMarkAllSelectList(UCanvas* InCanvas);

	void	SetOnlyUpdateSelectActor(const bool bInOnlyUpdate);

private:
	TWeakObjectPtr<ADebugCameraController>	mDebugCameraController;
	TArray<TWeakObjectPtr<UCSDebugSelectComponent>>	mAllSelectList;
	TArray<TWeakObjectPtr<UCSDebugSelectComponent>>	mSelectList;
	bool	mbActive = false;
	bool	mbOnlyUpdateSelectActor = false;
	bool	mbShowInfo = false;
	bool	mbShowMark = false;
	bool	mbShowSelectAxis = false;
	bool	mbShowSelectBone = false;
	bool	mbShowSelectPathFollow = false;
	bool	mbShowSelectLastEQS = false;
	bool	mbShowSelectBehaviorTree = false;
	bool	mbShowSelectPerception = false;

#endif//USE_CSDEBUG
};
