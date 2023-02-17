// Copyright 2020 SensyuGames.
/**
 * @file CSDebugDraw.cpp
 * @brief Debug表示用のまとめクラス
 * @author SensyuGames
 * @date 2022/5/12
 */


#include "CSDebugDraw.h"
#include "CSDebugMath.h"
#include "DebugInfoWindow/CSDebugInfoWindowText.h"

#include "Components/LineBatchComponent.h"
#include "AIModule/Classes/AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "NavLinkCustomComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "EnvironmentQuery/EnvQueryDebugHelpers.h"

#if USE_CSDEBUG
/**
 * @brief	8面体風矢印表示
 */
void UCSDebugDraw::OctahedronArrow::Draw(UWorld* InWorld, const FColor& InColor, const uint8 InDepthPriority, const float InThickness) const
{
	if (GEngine->GetNetMode(InWorld) == NM_DedicatedServer)
	{
		return;
	}

	ULineBatchComponent* const LineBatcher = InWorld->LineBatcher;
	if (LineBatcher == NULL)
	{
		return;
	}

	const float TargetLen = FVector::Distance(mBasePos, mTargetPos);
	const float Extent = mRadius;
	const FVector TargetV = mTargetPos - mBasePos;
	const FRotator TargetRotator = TargetV.Rotation();
	const uint32 QuadPosNum = 4;
	const FVector QuadPosList[QuadPosNum] =
	{
		mBasePos + TargetRotator.RotateVector(FVector(TargetLen * mQadCenterRatio,Extent,Extent)),
		mBasePos + TargetRotator.RotateVector(FVector(TargetLen * mQadCenterRatio,-Extent,Extent)),
		mBasePos + TargetRotator.RotateVector(FVector(TargetLen * mQadCenterRatio,-Extent,-Extent)),
		mBasePos + TargetRotator.RotateVector(FVector(TargetLen * mQadCenterRatio,Extent,-Extent)),
	};

	for (int32 i = 0; i < QuadPosNum; ++i)
	{
		LineBatcher->DrawLine(mTargetPos, QuadPosList[i], InColor, InDepthPriority, InThickness);
	}
	for (int32 i = 0; i < QuadPosNum; ++i)
	{
		LineBatcher->DrawLine(mBasePos, QuadPosList[i], InColor, InDepthPriority, InThickness);
	}
	for (int32 i = 0; i < QuadPosNum; ++i)
	{
		LineBatcher->DrawLine(QuadPosList[i], QuadPosList[(i + 1) % QuadPosNum], InColor, InDepthPriority, InThickness);
	}
}

/**
 * @brief	扇形表示
 */
void	UCSDebugDraw::FanShape::Draw(UWorld* InWorld, const FColor& InColor, const uint8 InDepthPriority, const float InThickness) const
{
	if (mEdgePointNum == 0
		|| GEngine->GetNetMode(InWorld) == NM_DedicatedServer)
	{
		return;
	}

	ULineBatchComponent* const LineBatcher = InWorld->LineBatcher;
	if (LineBatcher == NULL)
	{
		return;
	}

	const uint32 AllPointNum = mEdgePointNum + 1;//起点分加算
	TArray<FVector>	PointList;
	PointList.Reserve(AllPointNum);
	PointList.Add(mPos);

	const float HalfAngle = mAngle * 0.5f;

	const float AngleInterval = mAngle / (float)mEdgePointNum;
	float PointAngle = -HalfAngle;
	for (uint32 i = 0; i < mEdgePointNum; ++i)
	{
		if (i + 1 == mEdgePointNum)
		{
			PointAngle = HalfAngle;
		}
		const FRotator LocalLeftRotator(0.f, PointAngle, 0.f);
		const FVector LocalLeftEdgeV = LocalLeftRotator.RotateVector(FVector(mRadius, 0.f, 0.f));
		PointList.Add(mPos + mRot.RotateVector(LocalLeftEdgeV));
		PointAngle += AngleInterval;
	}

	for (uint32 i = 0; i < AllPointNum; ++i)
	{
		const uint32 NextPointIndex = (i + 1) % AllPointNum;
		LineBatcher->DrawLine(PointList[i], PointList[NextPointIndex], InColor, InDepthPriority, InThickness);
	}
}

/**
 * @brief	鋭角部分を削った扇形表示
 */
void	UCSDebugDraw::FanShapeClipTip::Draw(UWorld* InWorld, const FColor& InColor, const uint8 InDepthPriority, const float InThickness) const
{
	ULineBatchComponent* const LineBatcher = InWorld->LineBatcher;
	if (mEdgePointNum == 0
		|| GEngine->GetNetMode(InWorld) == NM_DedicatedServer
		|| LineBatcher == NULL
		|| mNearClipRadius >= mRadius
		)
	{
		return;
	}

	if (mNearClipRadius <= 0.f)
	{
		FanShape::Draw(InWorld, InColor, InDepthPriority, InThickness);
		return;
	}

	const uint32 AllPointNum = mEdgePointNum * 2;//内側と外側
	TArray<FVector>	PointList;
	PointList.Reserve(AllPointNum);

	const float HalfAngle = mAngle * 0.5f;

	const float AngleInterval = mAngle / (float)mEdgePointNum;
	for (int32 ArcIndex = 0; ArcIndex < 2; ++ArcIndex)
	{
		const bool bNearArc = (ArcIndex == 1);
		float ArcRadius = mRadius;
		if (bNearArc)
		{
			ArcRadius = mNearClipRadius;
		}

		float PointAngle = -HalfAngle;
		if (bNearArc)
		{
			PointAngle = HalfAngle;
		}
		for (uint32 i = 0; i < mEdgePointNum; ++i)
		{
			if (i + 1 == mEdgePointNum)
			{
				PointAngle = HalfAngle;
				if (bNearArc)
				{
					PointAngle = -HalfAngle;
				}
			}
			const FRotator LocalLeftRotator(0.f, PointAngle, 0.f);
			const FVector LocalLeftEdgeV = LocalLeftRotator.RotateVector(FVector(ArcRadius, 0.f, 0.f));
			PointList.Add(mPos + mRot.RotateVector(LocalLeftEdgeV));
			if (bNearArc)
			{
				PointAngle -= AngleInterval;
			}
			else
			{
				PointAngle += AngleInterval;
			}
		}
	}

	for (uint32 i = 0; i < AllPointNum; ++i)
	{
		const uint32 NextPointIndex = (i + 1) % AllPointNum;
		LineBatcher->DrawLine(PointList[i], PointList[NextPointIndex], InColor, InDepthPriority, InThickness);
	}
}

/**
 * @brief	PathFollowComponentの移動ルート表示
 */
void	UCSDebugDraw::DrawPathFollowRoute(UWorld* InWorld, UCanvas* InCanvas, const AAIController* InAIController, const bool bInShowDetail)
{
	if (InAIController->GetMoveStatus() != EPathFollowingStatus::Moving)
	{
		return;
	}
	UWorld* World = InWorld;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	const UPathFollowingComponent* PathFollowingComponent = InAIController->GetPathFollowingComponent();
	if (NavSys == nullptr
		|| PathFollowingComponent == nullptr)
	{
		return;
	}

	const FNavigationPath* PathInstance = PathFollowingComponent->GetPath().Get();
	if (PathInstance == nullptr)
	{
		return;
	}

	const FColor PassagePathColor(20, 20, 255);
	const FColor PlanPathColor(255, 165, 20);
	const int32 PathPointNum = PathInstance->GetPathPoints().Num();
	const int32 NeedPointNum = 2;
	const int32 CurrentPathIndex = (int32)PathFollowingComponent->GetCurrentPathIndex();
	const int32 NextPathIndex = (int32)PathFollowingComponent->GetNextPathIndex();
	if (PathPointNum < NeedPointNum
		|| CurrentPathIndex >= PathPointNum
		|| NextPathIndex >= PathPointNum
		)
	{
		return;
	}


	FVector BeginLocation = PathInstance->GetPathPoints()[0].Location;
	DrawDebugLine(World, BeginLocation, BeginLocation + FVector(0.f, 0.f, 100.f), PassagePathColor);

	const float OctahedronRadius = 10.f;
	const FVector OctahedronBasePosOffset(0.f, 0.f, 80.f);
	{
		UCSDebugDraw::OctahedronArrow CurrentPathArrow;
		CurrentPathArrow.mBasePos = PathInstance->GetPathPoints()[CurrentPathIndex].Location + OctahedronBasePosOffset;
		CurrentPathArrow.mTargetPos = PathInstance->GetPathPoints()[CurrentPathIndex].Location;
		CurrentPathArrow.mRadius = OctahedronRadius;
		CurrentPathArrow.Draw(World, PassagePathColor);
	}
	{
		UCSDebugDraw::OctahedronArrow NextPathArrow;
		NextPathArrow.mBasePos = PathInstance->GetPathPoints()[NextPathIndex].Location + OctahedronBasePosOffset;
		NextPathArrow.mTargetPos = PathInstance->GetPathPoints()[NextPathIndex].Location;
		NextPathArrow.mRadius = OctahedronRadius;
		NextPathArrow.Draw(World, PlanPathColor);
	}

	if (bInShowDetail)
	{
		FCSDebugInfoWindowText DebugInfoWindow;
		DebugInfoWindow.AddText(FString::Printf(TEXT("Index : %d"), 0));
		DebugInfoWindow.AddText(FString::Printf(TEXT("pos(%s)"), *BeginLocation.ToString()));
		DebugInfoWindow.SetWindowFrameColor(FColor(150, 200, 200));
		DebugInfoWindow.Draw(InCanvas, BeginLocation);
	}

	for (int32 i = 0; i + 1 < PathPointNum; ++i)
	{
		const FNavPathPoint& PathPoint = PathInstance->GetPathPoints()[i];
		const FNavPathPoint& NextPathPoint = PathInstance->GetPathPoints()[i + 1];
		const FVector EndLocation = NextPathPoint.Location;
		FColor LineColor = PlanPathColor;
		if (i < CurrentPathIndex)
		{
			LineColor = PassagePathColor;
		}

#if WITH_EDITOR
		if (i == CurrentPathIndex)
		{
			TArray<FVector> NavMeshEdgeVerts;
			//RecastNavMesh->DebugGetNavNodeVertex(NavMeshEdgeVerts, NextPathPoint.NodeRef);
			for (int32 Index = 0; Index < NavMeshEdgeVerts.Num(); Index += 2)
			{
				const FVector BeginPos = NavMeshEdgeVerts[Index];
				const FVector EndPos = NavMeshEdgeVerts[Index + 1];
				DrawDebugLine(World, BeginPos, EndPos, FColor::Red, false, -1.f, 255, 5.f);
			}
		}
#endif

		if (PathPoint.CustomLinkId > 0)
		{
			const INavLinkCustomInterface* CustomNavLink = NavSys->GetCustomLink(PathPoint.CustomLinkId);
			if (CustomNavLink)
			{
				const UNavLinkCustomComponent* CustomComponent = Cast<UNavLinkCustomComponent>(CustomNavLink);
				if (CustomComponent)
				{
					FVector LeftPos;
					FVector RightPos;
					ENavLinkDirection::Type Direction;
					CustomComponent->GetLinkData(LeftPos, RightPos, Direction);
					DrawDebugLine(World, LeftPos, RightPos, FColor::White);
				}
			}
		}
		else
		{
			DrawDebugLine(World, BeginLocation, EndLocation, LineColor);
		}
		DrawDebugLine(World, EndLocation, EndLocation + FVector(0.f, 0.f, 100.f), LineColor);

		if (bInShowDetail)
		{
			FCSDebugInfoWindowText DebugInfoWindow;
			DebugInfoWindow.AddText(FString::Printf(TEXT("Index : %d"), i + 1));
			DebugInfoWindow.AddText(FString::Printf(TEXT("pos(%s)"), *EndLocation.ToString()));
			DebugInfoWindow.SetWindowFrameColor(FColor(150, 200, 200));
			DebugInfoWindow.Draw(InCanvas, EndLocation);
		}

		BeginLocation = EndLocation;
	}
}

/**
 * @brief	最終EQS情報表示
 */
void	UCSDebugDraw::DrawLastEQS(UWorld* InWorld, UCanvas* InCanvas, const AAIController* InAIController, const float InShowDetailDistance)
{
#if USE_EQS_DEBUGGER
	float TimeStamp = 0.f;
	FEnvQueryInstance* QueryInstance = UCSDebugMath::FindLastEnvQueryInstance(TimeStamp, InAIController->GetPawn());
	if (QueryInstance == nullptr)
	{
		return;
	}
	const FColor LineColor_Safe(20, 200, 20);
	const FColor LineColor_Out(20, 20, 200);
	const bool bUseMidResults = QueryInstance && (QueryInstance->Items.Num() < QueryInstance->DebugData.DebugItems.Num());
	const FEnvQueryDebugData& InstanceDebugData = QueryInstance->DebugData;
	const TArray<FEnvQueryItemDetails>& Details = InstanceDebugData.DebugItemDetails;
	const TArray<FEnvQueryItem>& Items = bUseMidResults ? QueryInstance->DebugData.DebugItems : QueryInstance->Items;
	const TArray<uint8>& RawData = QueryInstance->DebugData.RawData;
	const bool bNoTestsPerformed = QueryInstance->CurrentTest <= 0;
	const bool bSingleItemResult = QueryInstance->DebugData.bSingleItemResult;

	float MinScore = 0.f;
	float MaxScore = -BIG_NUMBER;
	if (!bSingleItemResult)
	{
		if (bUseMidResults /*|| HighlightRangePct < 1.0f*/)
		{
			const FEnvQueryItem* ItemInfo = Items.GetData();
			for (int32 ItemIndex = 0; ItemIndex < Items.Num(); ItemIndex++, ItemInfo++)
			{
				if (ItemInfo->IsValid())
				{
					MinScore = FMath::Min(MinScore, ItemInfo->Score);
					MaxScore = FMath::Max(MaxScore, ItemInfo->Score);
				}
			}
		}
	}
	const float ScoreNormalizer = bUseMidResults && (MaxScore != MinScore) ? (1.f / (MaxScore - MinScore)) : 1.f;

	const auto ItemType = QueryInstance->ItemType;
	if (!ItemType->IsChildOf(UEnvQueryItemType_VectorBase::StaticClass()))
	{
		return;
	}
	const UEnvQueryItemType_VectorBase* DefTypeOb = ItemType->GetDefaultObject<UEnvQueryItemType_VectorBase>();

	EQSDebug::FQueryData DebugItem;
	UEnvQueryDebugHelpers::QueryToDebugData(*QueryInstance, DebugItem, MAX_int32);

	const float OctahedronRadius = 5.f;
	const FVector OctahedronTopOffset(0.f, 0.f, 50.f);
	UWorld* World = InWorld;
	// スコアを求めたアイテムを表示
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		const FEnvQueryItem& Item = Items[i];
		if (Item.IsValid())
		{
			const FVector Pos = DefTypeOb->GetItemLocation(RawData.GetData() + Item.DataOffset);
			const FVector TopPos = Pos + OctahedronTopOffset;
			FColor LineColor = LineColor_Safe;
			if (!bSingleItemResult)
			{
				const float NormalizedScore = bNoTestsPerformed ? 1 : (Item.Score - MinScore) * ScoreNormalizer;
				LineColor = FColor::MakeRedToGreenColorFromScalar(NormalizedScore);
			}
			UCSDebugDraw::OctahedronArrow Arrow;
			Arrow.mBasePos = TopPos;
			Arrow.mTargetPos = Pos;
			Arrow.mRadius = OctahedronRadius;
			Arrow.Draw(World, LineColor);

			const float Score = bNoTestsPerformed ? 1 : Item.Score;


			FCSDebugInfoWindowText DebugInfoWindow;
			if (i == 0)
			{
				DebugInfoWindow.SetWindowName(FString::Printf(TEXT("Winner")));
			}
			DebugInfoWindow.AddText(FString::Printf(TEXT("pos : %s"), *Pos.ToString()));
			DebugInfoWindow.AddText(FString::Printf(TEXT("[%d] Score : %.3f"), i, Score));
			{
				//DebugInfoWindow.BeginCategory(TEXT("Tests"));
				const int32 TestNum = DebugItem.Tests.Num();
				if (i < DebugItem.Items.Num())
				{
					const EQSDebug::FItemData& ItemData = DebugItem.Items[i];
					for (int32 TestIndex = 0; TestIndex < TestNum; ++TestIndex)
					{
						const EQSDebug::FTestData& TestData = DebugItem.Tests[TestIndex];
						DebugInfoWindow.AddText(FString::Printf(TEXT("[%s] : %.3f(%.3f)"), *TestData.ShortName, ItemData.TestScores[TestIndex], ItemData.TestValues[TestIndex]));
					}
				}
				//DebugInfoWindow.EndCategory();
			}
			DebugInfoWindow.SetWindowFrameColor(FLinearColor(LineColor));
			DebugInfoWindow.Draw(InCanvas, TopPos, InShowDetailDistance);
		}
	}

	// スコア求まらなかったアイテム表示
	const TArray<FEnvQueryItem>& DebugQueryItems = InstanceDebugData.DebugItems;

	const int32 DebugItemCountLimit = DebugQueryItems.Num() == Details.Num() ? DebugQueryItems.Num() : 0;

	for (int32 i = 0; i < DebugItemCountLimit; ++i)
	{
		if (DebugQueryItems[i].IsValid())
		{
			continue;
		}

		FColor LineColor = LineColor_Out;

		const float Score = bNoTestsPerformed ? 1 : DebugQueryItems[i].Score;
		const FVector Pos = DefTypeOb->GetItemLocation(RawData.GetData() + DebugQueryItems[i].DataOffset);
		const FVector TopPos = Pos + OctahedronTopOffset;

		UCSDebugDraw::OctahedronArrow Arrow;
		Arrow.mBasePos = TopPos;
		Arrow.mTargetPos = Pos;
		Arrow.mRadius = OctahedronRadius;
		Arrow.Draw(World, LineColor);

		FCSDebugInfoWindowText DebugInfoWindow;
		DebugInfoWindow.AddText(FString::Printf(TEXT("pos : %s"), *Pos.ToString()));
		DebugInfoWindow.AddText(FString::Printf(TEXT("[%d] Score : %.3f"), i, Score));

		if (Details[i].FailedTestIndex != INDEX_NONE)
		{
			int32 FailedTestIndex = Details[i].FailedTestIndex;
			//float FailedScore = Details[i].TestResults[FailedTestIndex];

			const FString Label = InstanceDebugData.PerformedTestNames[FailedTestIndex] + FString::Printf(TEXT("(%d)"), FailedTestIndex);
			DebugInfoWindow.AddText(FString::Printf(TEXT("%s"), *Label));
		}

		DebugInfoWindow.SetWindowFrameColor(FLinearColor(LineColor));
		DebugInfoWindow.Draw(InCanvas, TopPos, InShowDetailDistance);
	}
#endif
}

/**
 * @brief	Canvasに四角形表示
 */
void	UCSDebugDraw::DrawCanvasQuadrangle(UCanvas* InCanvas, const FVector2D& InCenterPos, const FVector2D& InExtent, const FLinearColor InColor)
{
	const uint32 PointListSize = 4;
	const FVector2D PointList[PointListSize] = {
		FVector2D(InCenterPos.X - InExtent.X, InCenterPos.Y + InExtent.Y),//左上
		FVector2D(InCenterPos.X - InExtent.X, InCenterPos.Y - InExtent.Y),//左下
		FVector2D(InCenterPos.X + InExtent.X, InCenterPos.Y - InExtent.Y),//右下
		FVector2D(InCenterPos.X + InExtent.X, InCenterPos.Y + InExtent.Y)//右上
	};
	for (uint32 i = 0; i < PointListSize; ++i)
	{
		DrawDebugCanvas2DLine(InCanvas, PointList[i], PointList[(i + 1) % PointListSize], InColor);
	}
}
void	UCSDebugDraw::DrawCanvasQuadrangle(UCanvas* InCanvas, const FVector& InPos, const FVector2D& InExtent, const FLinearColor InColor)
{
	const FVector ProjectPos = InCanvas->Project(InPos);
	if (ProjectPos.X < 0.f
		|| ProjectPos.X > InCanvas->SizeX
		|| ProjectPos.Y < 0.f
		|| ProjectPos.Y > InCanvas->SizeY
		|| ProjectPos.Z <= 0.f)
	{
		return;
	}

	const FVector2D ScreenPos(ProjectPos);
	UCSDebugDraw::DrawCanvasQuadrangle(InCanvas, ScreenPos, InExtent, InColor);
}

#endif//USE_CSDEBUG