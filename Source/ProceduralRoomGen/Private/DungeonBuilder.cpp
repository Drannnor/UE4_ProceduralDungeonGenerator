// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonBuilder.h"


#include "DrawDebugHelpers.h"
#include "Room.h"
#include "RoomValidator.h"
#include "Kismet/GameplayStatics.h"
#include "Math/RandomStream.h"
#include "Windows/LiveCoding/Private/External/LC_MemoryFile.h"

FRandomStream ADungeonBuilder::RandomStream = FRandomStream( 1337 );
// FRandomStream ADungeonBuilder::RandomStreamDoors = FRandomStream( 1337 );

// Sets default values
ADungeonBuilder::ADungeonBuilder() {

    MaxNumberOfRooms = 8;
    MinNumberOfRooms = 6;

    MaxRoomSizeX = 8;
    MinRoomSizeX = 8;
    MaxRoomSizeY = 8;
    MinRoomSizeY = 8;

    TileSize = 400;

    FloorMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>( TEXT( "FloorMesh" ) );
    FloorMesh->SetupAttachment( RootComponent );

    WallMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>( TEXT( "WallMesh" ) );
    WallMesh->SetupAttachment( RootComponent );

    DoorMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>( TEXT( "DoorMesh" ) );
    DoorMesh->SetupAttachment( RootComponent );

    FRoom::CurrentRoomID = 0;
    bUseSeed = false;

}

// Called when the game starts or when spawned
void ADungeonBuilder::BeginPlay() {
    Super::BeginPlay();

    if( bUseSeed ) {
        RandomStream.Initialize( Seed );
    } else {
        RandomStream.Initialize( FMath::Rand() );
    }
    // RandomStreamDoors.Initialize( Seed );

    NumberOfRooms = RandomStream.RandRange( int32( MinNumberOfRooms ), MaxNumberOfRooms );
    Rooms = TMap<int32, FRoom*>();
    OpenDoors = new TArray<FDoor*>();

    GenerateDungeon();
    BuildDungeon();

}

bool ADungeonBuilder::CheckIfValidRoom( FRoom* Room ) {
    bool bResult = true;
    UE_LOG( LogTemp, Log, TEXT( "Checking if room %d collides with other rooms" ), Room->GetRoomID() )
    UWorld* World = GetWorld();

    const FVector ValidatorScale = FVector( Room->GetRoomSizeX(), Room->GetRoomSizeY(), 2 ) / 2.0f;
    const FVector ValidatorLocation = FVector( Room->GetPositions() ) + FVector(
        Room->GetRoomSizeX() - 1, Room->GetRoomSizeY() - 1, 1.0f ) * TileSize / 2.0f;

    const FTransform RoomValidatorTransform = FTransform( FQuat::Identity, ValidatorLocation, ValidatorScale );
    UE_LOG( LogTemp, Log, TEXT( "Validator Transform: %s" ), *RoomValidatorTransform.ToString() )

    if( World ) {
        FActorSpawnParameters SpawnParams = FActorSpawnParameters();
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
        ARoomValidator* Validator = World->SpawnActor<ARoomValidator>( ARoomValidator::StaticClass(),
                                                                       RoomValidatorTransform, SpawnParams );
        // DrawDebugBox( World, ValidatorCenter, ValidatorExtent, FColor::Red, true, -1, 0, 5 );

        if( Validator ) {
            bResult = true;
        } else {
            bResult = false;
        }
    }

    return bResult;
}


void ADungeonBuilder::GenerateDungeon() {

    //Generate 1 Room
    int32 RoomSizeX = RandomStream.RandRange( MinRoomSizeX, MaxRoomSizeX );
    int32 RoomSizeY = RandomStream.RandRange( MinRoomSizeY, MaxRoomSizeY );

    FRoom* SpawnRoom = new FRoom( RoomSizeX, RoomSizeY, 4 );
    Rooms.Emplace( SpawnRoom->GetRoomID(), SpawnRoom );
    CheckIfValidRoom( SpawnRoom );

    OpenDoors->Append( SpawnRoom->GetDoors() );
    while( OpenDoors->Num() > 0 && Rooms.Num() < NumberOfRooms ) {
        //Pick a Random Door
        const uint32 RandIndex = RandomStream.RandRange( 0, OpenDoors->Num() - 1 );
        FDoor* DoorStart = ( *OpenDoors )[RandIndex];

        //pick a size for the new room
        RoomSizeX = RandomStream.RandRange( MinRoomSizeX, MaxRoomSizeX );
        RoomSizeY = RandomStream.RandRange( MinRoomSizeY, MaxRoomSizeY );

        //Define the default door directions
        EDirection ReverseDir = FRoom::ReverseDirectionMap[DoorStart->Direction];
        TArray<EDirection> DoorDirections = TArray<EDirection>();
        DoorDirections.Emplace( ReverseDir );

        //Create the new Room
        FRoom* NewRoom = new FRoom( RoomSizeX, RoomSizeY, -1, 4, &DoorDirections );

        //Get the newly created door that will be adjacent to OpenDoor
        FDoor* NewDoor = NewRoom->GetDoorByDir( ReverseDir );

        //Calculate the offSet
        int32 OffSetX = DoorStart->PosX - NewDoor->PosX;
        int32 OffSetY = DoorStart->PosY - NewDoor->PosY;
        FRoom* StartRoom = Rooms[DoorStart->RoomId];

        switch( DoorStart->Direction ) {
        case EDirection::North: {
            OffSetY = StartRoom->GetRoomSizeY();
            break;
        }
        case EDirection::South: {
            OffSetY = -NewRoom->GetRoomSizeY();
            break;
        }
        case EDirection::East: {
            OffSetX = StartRoom->GetRoomSizeX();
            break;
        }
        case EDirection::West: {
            OffSetX = -NewRoom->GetRoomSizeX();
            break;
        }
        }


        FIntVector OffSet = FIntVector( OffSetX, OffSetY, 0.0f );
        const FIntVector NewPosition = ( StartRoom->GetPositions() + OffSet * TileSize );
        UE_LOG( LogTemp, Log,
                TEXT( "RoomID: %d, OriginRoomPosition: %s, OffSet: %s, NewRoomPosition: %s \nDoorInfo: Pos: %d, %d" ),
                StartRoom->GetRoomID(),
                *StartRoom->GetPositions().ToString(), *OffSet.ToString(), *NewPosition.ToString(),
                DoorStart->PosX, DoorStart->PosY )

        //Set the Position of the new Room according to the offset
        NewRoom->SetPosition( NewPosition );

        const bool ValidRoom = CheckIfValidRoom( NewRoom );
        if( ValidRoom ) {
            UE_LOG( LogTemp, Log, TEXT( "Room is good to go" ) )
            Rooms.Emplace( NewRoom->GetRoomID(), NewRoom );
            //Get New Doors
            OpenDoors->Append( NewRoom->GetDoors() );
            OpenDoors->Remove( NewDoor );
            OpenDoors->Remove( DoorStart );


        } else {
            UE_LOG( LogTemp, Log, TEXT( "Room Colliding, Discarting" ) )
            
        }

        //Remove linked Doors
    }

     CloseDeadEnd();
}

void ADungeonBuilder::CloseDeadEnd() {
    for( FDoor* DeadEnd : *OpenDoors ) {
        FRoom* RoomWithDeadEnd = Rooms[DeadEnd->RoomId];
        RoomWithDeadEnd->CloseDoor(DeadEnd->PosX, DeadEnd->PosY);
    }
}


void ADungeonBuilder::BuildDungeon() {

    TArray<FRoom*> RoomList;
    Rooms.GenerateValueArray( RoomList );

    for( FRoom* Room : RoomList ) {
        for( int32 x = 0; x < Room->GetRoomSizeX(); x++ ) {
            for( int32 y = 0; y < Room->GetRoomSizeY(); y++ ) {
                const FTile TileInfo = Room->GetTile( x, y );
                FTransform InstanceTransform = FTransform(
                    static_cast<FVector>( Room->GetPositions() ) + FVector( x * TileSize, y * TileSize, 0.0f ) );
                FQuat Rotation = TileInfo.Rotation;
                InstanceTransform.SetRotation( Rotation );
                switch( TileInfo.Type ) {
                case ETileType::Wall: {
                    WallMesh->AddInstance( InstanceTransform );
                    break;
                }
                case ETileType::Door: {
                    DoorMesh->AddInstance( InstanceTransform );
                    break;
                }
                default: {
                    FloorMesh->AddInstance( InstanceTransform );
                }
                break;
                }
            }
        }
    }
}
