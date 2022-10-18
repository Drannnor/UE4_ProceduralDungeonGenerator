// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FDoor;
struct FNestedArray;
struct FTile;

UENUM()
enum class EDirection: uint8 {
    North,
    East,
    South,
    West,
};

/**
* 
*/
class PROCEDURALROOMGEN_API FRoom {
public:
    static int32 CurrentRoomID;
    
    FRoom(): RoomSizeX( 0 ), RoomSizeY( 4 ), RoomdID( 0 ) { }

    FRoom( uint32 RoomSizeX, uint32 RoomSizeY, int8 NumberOfDoors = -1,
           uint32 MaxNumberOfDoors = 4, TArray<EDirection>* KnownDoorDirections = nullptr );
    ~FRoom();

    int32 GetRoomSizeX();
    int32 GetRoomSizeY();

    FTile GetTile( uint32 X, uint32 Y );
    FIntVector& GetPositions();
    TArray<FDoor*> GetDoors();
    void SetPosition( const FIntVector& RoomPos );

    FDoor* GetDoorByDir( EDirection Direction );
    int32 GetRoomID();
    void CloseDoor( uint32 X, uint32 Y );

    static const TMap<EDirection, EDirection> ReverseDirectionMap;


private:
    FIntVector Position;

    int32 RoomSizeX;

    int32 RoomSizeY;

    TArray<FNestedArray> Layout;

    TMap<EDirection, FDoor*> Doors;
    
    int32 RoomdID;


    
    bool IsWallTile( uint32 X, uint32 Y );

    uint32 SelectRandomDoorPos( uint32 RoomSize );

    /*
     * Generate Doors to be placed in the Room
     */
    void GenerateDoors( TArray<EDirection>* KnownDoorDirections, int8 NumberOfDoors, uint32 MaxNumberOfDoors );

};


UENUM()
enum class ETileType: uint8 {
    Floor,
    Wall,
    Door,
};

struct FTile {

    ETileType Type;
    FQuat Rotation;

    FTile() {
        Type = ETileType::Floor;
        Rotation = FQuat::Identity;
    }

    explicit FTile( const ETileType Type, const FQuat Rotation = FQuat::Identity ) {
        this->Type = Type;
        this->Rotation = Rotation;
    }
};


struct FNestedArray {

    TArray<FTile> Tiles;

    FNestedArray() {
        Tiles.Init( FTile(), 0 );
    }

    explicit FNestedArray( const uint32 Size ) {
        Tiles.Init( FTile(), Size );
    }
};

struct FDoor {

    uint32 PosX;
    uint32 PosY;
    FQuat Rotation;
    EDirection Direction;
    int32 RoomId;

    FDoor(): PosX( 0 ), PosY( 0 ), Rotation( FQuat::Identity ), Direction( EDirection::North ), RoomId( 0 ) { }

    FDoor( const uint32 DoorX, const uint32 DoorY, const EDirection Dir, const FQuat& Rotation,
           const int32 RoomId ): PosX( DoorX ),
                                 PosY( DoorY ),
                                 Rotation( Rotation ),
                                 Direction( Dir ), RoomId( RoomId ) { }
};
