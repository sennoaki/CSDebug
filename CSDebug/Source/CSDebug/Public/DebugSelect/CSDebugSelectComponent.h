// Copyright 2020 SensyuGames.
/**
 * @file CSDebugSelectComponent.h
 * @brief CSDebugSelect�ΏۗpComponent
 * @author SensyuGames
 * @date 2020/05/27
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DebugInfoWindow/CSDebugInfoWindowText.h"
#include "CSDebugSelectComponent.generated.h"

class UCSDebugSelectManager;
class UCanvas;
class AAIController;

DECLARE_DYNAMIC_DELEGATE(FDebugSelectPreDrawDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSDEBUG_API UCSDebugSelectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCSDebugSelectComponent();

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void    AddTextBP(const FString& InString)
	{
#if USE_CSDEBUG
		mDebugInfoWindow.AddText(InString);
#endif//USE_CSDEBUG
	}
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void    AddPreDrawDelegateBP(FDebugSelectPreDrawDelegate InDelegate)
	{
#if USE_CSDEBUG
		mPreDrawDelegate = InDelegate;
		mbUsePreDrawDelegate = true;
#endif//USE_CSDEBUG
	}

protected:
	UPROPERTY(EditAnywhere)
	FLinearColor	mMyColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);


#if USE_CSDEBUG
public:	

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	void    AddText(const FString& InString)
	{
		mDebugInfoWindow.AddText(InString);
	}
    void    AddPreDrawDelegate(FDebugSelectPreDrawDelegate InDelegate)
    {
        mPreDrawDelegate = InDelegate;
        mbUsePreDrawDelegate = true;
    }

	void	DebugDraw(UCanvas* InCanvas);
	void	DrawMark(class UCanvas* InCanvas) const;

	void	SetSelect(bool bInSelect);
	bool	IsSelect() const { return !!mbSelect; }
	const FLinearColor&	GetMyColor() const { return mMyColor; }

private:
	FCSDebugInfoWindowText    mDebugInfoWindow;
	FDebugSelectPreDrawDelegate mPreDrawDelegate;
	TWeakObjectPtr<UCSDebugSelectManager>	mManager;
	uint8	mbSelect : 1;//�I���ς�
	uint8   mbUsePreDrawDelegate : 1;
	uint8 : 6;

#endif//USE_CSDEBUG
};
