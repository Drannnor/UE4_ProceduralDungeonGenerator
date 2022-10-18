#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomGenerator.generated.h"

class UBoxComponent;

UENUM()
enum class EDirectionTest: uint8 {
    North,
    East,
    South,
    West,
};

UENUM()
enum class ETileTypeTest: uint8 {
    Floor,
    Wall,
    Door,
};


USTRUCT()
struct FTileTest {
    GENERATED_BODY()

    ETileTypeTest Type;
    FRotator Rotation;

    FTileTest() {
        Type = ETileTypeTest::Floor;
        Rotation = FRotator::ZeroRotator;
    }

    explicit FTileTest( const ETileTypeTest Type, const FRotator Rotation = FRotator::ZeroRotator ) {
        this->Type = Type;
        this->Rotation = Rotation;
    }
};

USTRUCT()
struct FNestedArrayTest {
    GENERATED_BODY()

    TArray<FTileTest> Column;

    FNestedArrayTest() {
        Column.Init( FTileTest(), 0 );
    }

    explicit FNestedArrayTest( const uint32 Size ) {
        Column.Init( FTileTest(), Size );
    }
};

UCLASS()
class ARoomGenerator : public AActor {
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ARoomGenerator();

protected:

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UBoxComponent* BoxComp;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UInstancedStaticMeshComponent* FloorMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UInstancedStaticMeshComponent* WallMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UInstancedStaticMeshComponent* DoorMesh;

    UPROPERTY(EditInstanceOnly, Category= "RoomSettings", meta = (ClampMin = 0))
    uint32 RoomLengthX;

    UPROPERTY(EditInstanceOnly, Category= "RoomSettings", meta = (ClampMin = 0))
    uint32 RoomLengthY;

    TArray<FNestedArrayTest> RoomLayout;

    UPROPERTY(EditDefaultsOnly, Category= "RoomSettings", meta = (ClampMin = 0))
    int TileSize;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    void BuildRoom();


    bool IsInBoarder( uint32 X, uint32 Y );

};

USTRUCT()
struct FDoorInfo {
    GENERATED_BODY()

    FRotator Orientation;
    uint32 PosX;
    uint32 PosY;

    FDoorInfo() {
        PosX = 0;
        PosY = 0;
        Orientation = FRotator::ZeroRotator;
    }

    FDoorInfo( const EDirectionTest Direction, const uint32 WallX, const uint32 WallY ) {
        switch( Direction ) {
        case EDirectionTest::North: {
            PosX = SelectRandomDoorPos( WallX );
            PosY = WallY - 1;
            Orientation = FRotator::ZeroRotator;
            break;
        }
        case EDirectionTest::East: {
            PosX = WallX - 1;
            PosY = SelectRandomDoorPos( WallY );
            Orientation = FRotator( 0, 90, 0 );
            break;
        }
        case EDirectionTest::South: {
            PosX = SelectRandomDoorPos( WallX );
            PosY = 0;
            Orientation = FRotator::ZeroRotator;
            break;
        }
        case EDirectionTest::West: {
            PosX = 0;
            PosY = SelectRandomDoorPos( WallY );
            Orientation = FRotator( 0, 90, 0 );
            break;
        }
        default: {
            PosX = 0;
            PosY = 0;
            Orientation = FRotator::ZeroRotator;
        }
        }
    }

    uint32 SelectRandomDoorPos( uint32 WallSize ) {
        const float RelativePos = FMath::FRandRange( 0.0f, 1.0f );
        int32 CeilToInt = FMath::CeilToInt( ( WallSize - 2 ) * RelativePos );
        UE_LOG( LogTemp, Log, TEXT( "WallSize: %s, Rand: %s,Result: %s" ), *FString::FromInt( WallSize ),
                *FString::FromInt( RelativePos ), *FString::FromInt( CeilToInt ) )
        return CeilToInt;
    }
};
