// Copyright 2020 SensyuGames.
/**
 * @file CSDebugEUW_ViewportHelper.h
 * @brief UObject��Property�l�\��
 * @author SensyuGames
 * @date 2022/08/07
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "CSDebugEUW_ViewportHelper.generated.h"

/**
 * 
 */
UCLASS()
class CSDEBUGEDITOR_API UCSDebugEUW_ViewportHelper : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void	LookAt(const FString& InString);
	
	UFUNCTION(BlueprintCallable)
	void	RequestDraw(bool bInDraw);

protected:
	virtual void NativeDestruct() override;

	void	Draw(UCanvas* InCanvas, APlayerController* InPlayerController);

private:
	FVector		mLastLookAtLocation = FVector::ZeroVector;
	FDelegateHandle	mDebugDrawHandle;
};
