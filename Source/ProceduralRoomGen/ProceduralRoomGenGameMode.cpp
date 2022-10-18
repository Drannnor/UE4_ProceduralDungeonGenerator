// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralRoomGenGameMode.h"
#include "ProceduralRoomGenHUD.h"
#include "ProceduralRoomGenCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProceduralRoomGenGameMode::AProceduralRoomGenGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AProceduralRoomGenHUD::StaticClass();
}
