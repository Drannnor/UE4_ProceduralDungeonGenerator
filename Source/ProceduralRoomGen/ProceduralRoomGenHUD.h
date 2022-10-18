// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ProceduralRoomGenHUD.generated.h"

UCLASS()
class AProceduralRoomGenHUD : public AHUD
{
	GENERATED_BODY()

public:
	AProceduralRoomGenHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

