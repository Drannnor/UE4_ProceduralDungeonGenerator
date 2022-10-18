// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonBuilder.generated.h"

struct FDoor;
class FRoom;
struct FRandomStream;
UCLASS()
class PROCEDURALROOMGEN_API ADungeonBuilder : public AActor {
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ADungeonBuilder();
protected:

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UInstancedStaticMeshComponent* FloorMesh;
    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UInstancedStaticMeshComponent* WallMesh;
    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UInstancedStaticMeshComponent* DoorMesh;

    int32 NumberOfRooms;
    UPROPERTY(EditInstanceOnly, Category = "DungeonSettings")
    int32 MaxNumberOfRooms;
    UPROPERTY(EditInstanceOnly, Category = "DungeonSettings")
    int32 MinNumberOfRooms;

    UPROPERTY(EditInstanceOnly, Category = "DungeonSettings")
    int32 MaxRoomSizeX;
    UPROPERTY(EditInstanceOnly, Category = "DungeonSettings")
    int32 MinRoomSizeX;
    UPROPERTY(EditInstanceOnly, Category = "DungeonSettings")
    int32 MaxRoomSizeY;
    UPROPERTY(EditInstanceOnly, Category = "DungeonSettings")
    int32 MinRoomSizeY;

    int32 TileSize;

    TMap<int32, FRoom*> Rooms;

    TArray<FDoor*>* OpenDoors;

    UPROPERTY(EditInstanceOnly, Category="GameMode")
    bool bUseSeed;
    UPROPERTY(EditInstanceOnly, Category="GameMode", meta=(EditCondition="bUseSeed"))
    int32 Seed = 2;


    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    bool CheckIfValidRoom( FRoom* Room );
    void CloseDeadEnd();
    void GenerateDungeon();
    void BuildDungeon();

public:
    static FRandomStream RandomStream;
    // static FRandomStream RandomStreamDoors;

};
