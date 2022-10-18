// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomValidator.h"
#include "Components/BoxComponent.h"

// Sets default values
ARoomValidator::ARoomValidator() {
    Valid = true;

    BoxComp = CreateDefaultSubobject<UBoxComponent>( TEXT( "BoxComponent" ) );
    RootComponent = BoxComp;

    BoxComp->SetBoxExtent( FVector( 400, 400, 500 ) );
    // BoxComp->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
    // BoxComp->SetCollisionResponseToAllChannels( ECR_Ignore );
    // BoxComp->SetCollisionObjectType( ECC_EngineTraceChannel2 );
    // BoxComp->SetCollisionResponseToChannel( ROOM_VALIDATOR_OBJECT, ECR_Block );
    BoxComp->SetCollisionProfileName( TEXT( "RoomValidator" ) );
}


// Called when the game starts or when spawned
void ARoomValidator::BeginPlay() {
    Super::BeginPlay();

    UE_LOG( LogTemp, Log, TEXT( "Box Size: %s" ), *BoxComp->GetUnscaledBoxExtent().ToString() )

    TArray<AActor*> CollidingRooms;
    BoxComp->GetOverlappingActors( CollidingRooms, TSubclassOf<ARoomValidator>() );
    UE_LOG( LogTemp, Log, TEXT( "CollidingRooms size: %d" ), CollidingRooms.Num() )

    TArray<UPrimitiveComponent*> CollidingRooms2;
    BoxComp->GetOverlappingComponents( CollidingRooms2 );
    UE_LOG( LogTemp, Log, TEXT( "CollidingRooms size: %d" ), CollidingRooms2.Num() )

    if( CollidingRooms.Num() > 0 ) {
        Valid = false;
    }
}

void ARoomValidator::SetValidatorExtent( const FVector& Extent ) {
    if( BoxComp != nullptr) {
        BoxComp->SetBoxExtent( Extent );
    } else {
        UE_LOG( LogTemp, Log, TEXT( "BoxComp not found" ) )
    }
}

bool ARoomValidator::IsValid() {
    return Valid;
}

FVector ARoomValidator::GetBoxExtent() {
    return BoxComp->GetUnscaledBoxExtent();
}
