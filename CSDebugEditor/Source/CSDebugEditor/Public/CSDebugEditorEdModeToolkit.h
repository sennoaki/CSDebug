// Copyright 2020 SensyuGames.
/**
 * @file CSDebugEditorEdModeToolkit.h
 * @brief CSDebugEditorEdModeToolkit
 * @author SensyuGames
 * @date 2020/7/24
 */

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

class FCSDebugEditorEdModeToolkit : public FModeToolkit
{
public:

	FCSDebugEditorEdModeToolkit();
	
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }

private:

	TSharedPtr<SWidget> ToolkitWidget;
};
