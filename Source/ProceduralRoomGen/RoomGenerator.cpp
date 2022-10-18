#include "RoomGenerator.h"
#include "Components/BoxComponent.h"

// Sets default values
ARoomGenerator::ARoomGenerator() {

    RoomLengthX = 32;
    RoomLengthY = 32;
    TileSize = 400;

    BoxComp = CreateDefaultSubobject<UBoxComponent>( TEXT( "BoxComponent" ) );
    RootComponent = BoxComp;

    FloorMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>( TEXT( "FloorMesh" ) );
    FloorMesh->SetupAttachment( RootComponent );

    WallMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>( TEXT( "WallMesh" ) );
    WallMesh->SetupAttachment( RootComponent );

    DoorMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>( TEXT( "DoorMesh" ) );
    DoorMesh->SetupAttachment( RootComponent );
}


// Called when the game starts or when spawned
void ARoomGenerator::BeginPlay() {
    Super::BeginPlay();


    // SetActorLocation( GetActorLocation() - OffSet );

    //Set BOX size
    BoxComp->SetBoxExtent( FVector( RoomLengthX * TileSize, RoomLengthY * TileSize, 0.0f ) );
    RoomLayout.Init( FNestedArrayTest( RoomLengthY ), RoomLengthX );

    TArray<EDirectionTest> PossibleDirs = { EDirectionTest::North, EDirectionTest::East, EDirectionTest::South, EDirectionTest::West };
    PossibleDirs.Sort( []( const EDirectionTest& A, const EDirectionTest& B ) {
        return 0.0f > FMath::FRandRange( 0.0f, 1.0f ) - 0.5f;
    } );

    const uint8 NumberOfDoors = FMath::RandRange( 1, 4 );
    
    TArray<FDoorInfo> DoorPositions;
    for( int i = 0; i < NumberOfDoors; i++ ) {
        const EDirectionTest Dir = PossibleDirs.Pop( true );
        DoorPositions.Emplace( FDoorInfo( Dir, RoomLengthX, RoomLengthY ) );
    }

    //SetupWalls
    for( uint32 x = 0; x < RoomLengthX; x++ ) {
        for( uint32 y = 0; y < RoomLengthY; y++ ) {
            if( IsInBoarder( x, y ) ) {
                RoomLayout[x].Column[y] = FTileTest( ETileTypeTest::Wall );
            }
        }
    }

    //Setup Exits
    for( FDoorInfo Door : DoorPositions ) {
        RoomLayout[Door.PosX].Column[Door.PosY] = FTileTest( ETileTypeTest::Door, Door.Orientation );
    }

    BuildRoom();
}

void ARoomGenerator::BuildRoom() {

    //Offset Room location
    const FVector OffSet = FVector( ( float )( RoomLengthX ) / 2.0f * TileSize,
                                    ( float )( RoomLengthY ) / 2.0f * TileSize, 0.0f );
    UE_LOG( LogTemp, Log, TEXT( "Offset: %s" ), *OffSet.ToString() );

    for( uint32 x = 0; x < RoomLengthX; x++ ) {
        for( uint32 y = 0; y < RoomLengthY; y++ ) {
            FTileTest TileInfo = RoomLayout[x].Column[y];
            FTransform InstanceTransform = FTransform( FVector( x * TileSize, y * TileSize, 0.0f ) - OffSet );
            FQuat Rotation = TileInfo.Rotation.Quaternion();
            InstanceTransform.SetRotation( Rotation );
            switch( TileInfo.Type ) {
            case ETileTypeTest::Wall: {
                WallMesh->AddInstance( InstanceTransform );
                break;
            }
            case ETileTypeTest::Door: {
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

bool ARoomGenerator::IsInBoarder( const uint32 X, const uint32 Y ) {
    return X == 0 || Y == 0 || X == RoomLengthX - 1 || Y == RoomLengthY - 1;
}
