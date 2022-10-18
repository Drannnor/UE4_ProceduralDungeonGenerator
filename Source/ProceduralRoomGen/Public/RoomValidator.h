// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomValidator.generated.h"

class UBoxComponent;
UCLASS()
class PROCEDURALROOMGEN_API ARoomValidator : public AActor {
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ARoomValidator();
    void SetValidatorExtent( const FVector& Extent );
    bool IsValid();
    FVector GetBoxExtent();


protected:

    UBoxComponent* BoxComp;
    bool Valid;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

};
