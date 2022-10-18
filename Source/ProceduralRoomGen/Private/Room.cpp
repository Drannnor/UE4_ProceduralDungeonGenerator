// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"

#include "DungeonBuilder.h"

int32 FRoom::CurrentRoomID = 0;


const TMap<EDirection, EDirection> FRoom::ReverseDirectionMap = {
    { EDirection::North, EDirection::South },
    { EDirection::East, EDirection::West },
    { EDirection::South, EDirection::North },
    { EDirection::West, EDirection::East },

};

FRoom::FRoom( const uint32 RoomSizeX, const uint32 RoomSizeY, const int8 NumberOfDoors, const uint32 MaxNumberOfDoors,
              TArray<EDirection>* KnownDoorDirections ) {

    RoomdID = ++CurrentRoomID;
    this->RoomSizeX = RoomSizeX;
    this->RoomSizeY = RoomSizeY;

    this->Position = FIntVector( 0 );

    //initialize Room are with floor tiles
    Layout.Init( FNestedArray( RoomSizeY ), RoomSizeX );

    //change perimeter to walls
    for( uint32 x = 0; x < RoomSizeX; x++ ) {
        for( uint32 y = 0; y < RoomSizeY; y++ ) {
            if( IsWallTile( x, y ) ) {
                Layout[x].Tiles[y] = FTile( ETileType::Wall );
            }
        }
    }

    GenerateDoors( KnownDoorDirections, NumberOfDoors, MaxNumberOfDoors );

    TArray<FDoor*> OutArray;
    Doors.GenerateValueArray( OutArray );
    //Setup Exits
    for( FDoor* Door : OutArray ) {
        Layout[Door->PosX].Tiles[Door->PosY] = FTile( ETileType::Door, Door->Rotation );
    }

}

void FRoom::GenerateDoors( TArray<EDirection>* KnownDoorDirections, int8 NumberOfDoors, uint32 MaxNumberOfDoors ) {
    //Array with All possible Directions
    TArray<EDirection> AllDirections = { EDirection::North, EDirection::East, EDirection::South, EDirection::West };
    //Shuffle the Directions Array

    AllDirections.Sort( []( const EDirection& A, const EDirection& B ) {
        return 0.0f > ADungeonBuilder::RandomStream.RandRange( 0.0f, 1.0f ) - 0.5f;
    } );

    TArray<EDirection> DoorDirections;
    // Add All Know Directions to the Door Directions Array, and remove them from AllDirections
    if( KnownDoorDirections ) {
        for( auto Dir : *KnownDoorDirections ) {
            AllDirections.Remove( Dir );
            DoorDirections.Emplace( Dir );
            MaxNumberOfDoors --;
        }
    }

    if( NumberOfDoors < 0 ) {
        NumberOfDoors = ADungeonBuilder::RandomStream.RandRange( 1, MaxNumberOfDoors );
    }
    // NumberOfDoors = 3;

    //Fill the array with the remaining directions
    while( DoorDirections.Num() < NumberOfDoors ) {
        DoorDirections.Emplace( AllDirections.Pop( true ) );
    }

    //For each directions define a door
    for( EDirection Dir : DoorDirections ) {
        uint32 DoorX = 0;
        uint32 DoorY = 0;
        FQuat DoorRotation = FRotator::ZeroRotator.Quaternion();

        switch( Dir ) {
        case EDirection::North: {
            DoorX = SelectRandomDoorPos( RoomSizeX );
            DoorY = RoomSizeY - 1;
            DoorRotation = FRotator::ZeroRotator.Quaternion();
            break;
        }
        case EDirection::East: {
            DoorX = RoomSizeX - 1;
            DoorY = SelectRandomDoorPos( RoomSizeY );
            DoorRotation = FRotator( 0, 90, 0 ).Quaternion();
            break;
        }
        case EDirection::South: {
            DoorX = SelectRandomDoorPos( RoomSizeX );
            DoorY = 0;
            DoorRotation = FRotator::ZeroRotator.Quaternion();
            break;
        }
        case EDirection::West: {
            DoorX = 0;
            DoorY = SelectRandomDoorPos( RoomSizeY );
            DoorRotation = FRotator( 0, 90, 0 ).Quaternion();
            break;
        }
        }


        Doors.Emplace( Dir, new FDoor( DoorX, DoorY, Dir, DoorRotation, RoomdID ) );
    }
}

FRoom::~FRoom() {}

int32 FRoom::GetRoomSizeX() {
    return RoomSizeX;
}

int32 FRoom::GetRoomSizeY() {
    return RoomSizeY;
}

FTile FRoom::GetTile( const uint32 X, const uint32 Y ) {
    return Layout[X].Tiles[Y];
}

FIntVector& FRoom::GetPositions() {
    return Position;
}

TArray<FDoor*> FRoom::GetDoors() {
    TArray<FDoor*> OutArray;
    Doors.GenerateValueArray( OutArray );
    return OutArray;
}

void FRoom::SetPosition( const FIntVector& RoomPos ) {
    Position = RoomPos;
}

FDoor* FRoom::GetDoorByDir( EDirection Direction ) {
    return Doors[Direction];
}

int32 FRoom::GetRoomID() {
    return RoomdID;
}

void FRoom::CloseDoor( const uint32 X, const uint32 Y ) {
    Layout[X].Tiles[Y].Type = ETileType::Wall;
}


bool FRoom::IsWallTile( const uint32 X, const uint32 Y ) {
    return X == 0 || Y == 0 || X == RoomSizeX - 1 || Y == RoomSizeY - 1;
}

uint32 FRoom::SelectRandomDoorPos( const uint32 RoomSize ) {
    const float RelativePos = ADungeonBuilder::RandomStream.FRandRange( 0.0f, 1.0f );
    const int32 CeilToInt = FMath::CeilToInt( ( RoomSize - 2 ) * RelativePos );

    return CeilToInt;
}
