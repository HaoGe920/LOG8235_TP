// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SoftDesignTrainingPlayerController.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingMainCharacter.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "SDTBridge.h"
#include "SDTBoatOperator.h"
#include "Engine/OverlapResult.h"

ASoftDesignTrainingPlayerController::ASoftDesignTrainingPlayerController()
{
<<<<<<< HEAD
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
    // Make a path following component
    m_PathFollowingComponent = CreateDefaultSubobject<USDTPathFollowingComponent>(TEXT("PathFollowingComponent"));
}

<<<<<<< HEAD
void ASoftDesignTrainingPlayerController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    if (m_PathFollowingComponent == nullptr || GetWorld() == nullptr)
        return;

    const FNavPathSharedPtr path =
        m_PathFollowingComponent->GetPath();

    if (!path.IsValid())
        return;

    const TArray<FNavPathPoint>& points = path->GetPathPoints();

    for (int32 i = 1; i < points.Num(); ++i)
    {
        FVector start = points[i - 1].Location;
        FVector end = points[i].Location;

        start.Z += 25.f;
        end.Z += 25.f;

        DrawDebugLine(
            GetWorld(), start, end,
            FColor::Yellow, false, 0.f, 0, 3.f);

        DrawDebugSphere(
            GetWorld(), end, 12.f, 8,
            FColor::Yellow, false, 0.f);
    }
}

=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
void ASoftDesignTrainingPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Move camera
    InputComponent->BindAxis("MoveForward", this, &ASoftDesignTrainingPlayerController::MoveCameraForward);
    InputComponent->BindAxis("MoveRight", this, &ASoftDesignTrainingPlayerController::MoveCameraRight);

    // Zoom camera
    InputComponent->BindAxis("Zoom", this, &ASoftDesignTrainingPlayerController::ZoomCamera);

    // Move character on click
    InputComponent->BindAction("MoveCharacter", EInputEvent::IE_Released, this, &ASoftDesignTrainingPlayerController::MoveCharacter);
    
    InputComponent->BindAction("Activate", EInputEvent::IE_Pressed, this, &ASoftDesignTrainingPlayerController::Activate);
    InputComponent->BindAction("Activate", EInputEvent::IE_Released, this, &ASoftDesignTrainingPlayerController::Deactivate);
}

void ASoftDesignTrainingPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Always show the mouse
    bShowMouseCursor = true;

    // Allow character to be moved by default 
    m_CanMoveCharacter = true;

    // In case we are activating a bridge
    m_BridgeActivated = nullptr;

    // In case we are activating a boat operator
    m_BoatOperatorActivated = nullptr;
}

void ASoftDesignTrainingPlayerController::MoveCameraForward(float value)
{
    ASoftDesignTrainingMainCharacter* character = Cast<ASoftDesignTrainingMainCharacter>(GetPawn());

    if (character)
    {
        character->MoveCameraForward(value);
    }
}

void ASoftDesignTrainingPlayerController::MoveCameraRight(float value)
{
    ASoftDesignTrainingMainCharacter* character = Cast<ASoftDesignTrainingMainCharacter>(GetPawn());

    if (character)
    {
        character->MoveCameraRight(value);
    }
}

void ASoftDesignTrainingPlayerController::ZoomCamera(float axisValue)
{
    ASoftDesignTrainingMainCharacter* character = Cast<ASoftDesignTrainingMainCharacter>(GetPawn());

    if (character)
    {
        character->ZoomCamera(axisValue);
    }
}

void ASoftDesignTrainingPlayerController::MoveCharacter()
{
    // TODO : find the position of the mouse in the world 
    // And move the agent to this position IF possible
    // Validate you can move through m_CanMoveCharacter
<<<<<<< HEAD

    // Find the position under the mouse cursor
    FHitResult hitResult;

    if (!GetHitResultUnderCursor(ECC_Visibility, true, hitResult))
        return;

    // Check if the player is allowed to move
    if (!m_CanMoveCharacter || !GetPawn())
        return;

    // Move the character to the clicked position
    UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, hitResult.ImpactPoint);
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
}

void ASoftDesignTrainingPlayerController::Activate()
{
    APawn* pawn = GetPawn();
    if (pawn == nullptr)
    {
        return;
    }

    m_CanMoveCharacter = false;
    // TODO : Mouvement of the agent should be stopped !!

    // Make an overlap to find what is near us to activate it
    TArray<FOverlapResult> results;
    GetWorld()->OverlapMultiByChannel(results, pawn->GetActorLocation(), pawn->GetActorRotation().Quaternion(), ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(200.f));

    for (FOverlapResult& result : results)
    {
        AActor* actor = result.GetActor();
        ASDTBridge* bridge = Cast<ASDTBridge>(actor);
        if (bridge)
        {
            bridge->Activate();
            m_BridgeActivated = bridge;
            break;
        }

        ASDTBoatOperator* boatOperator = Cast<ASDTBoatOperator>(actor);
        if (boatOperator)
        {
            boatOperator->Activate();
            m_BoatOperatorActivated = boatOperator;
            break;
        }
    }
}

void ASoftDesignTrainingPlayerController::Deactivate()
{
    m_CanMoveCharacter = true;

    if (m_BridgeActivated)
    {
        m_BridgeActivated->Deactivate();
    }
    if (m_BoatOperatorActivated)
    {
        m_BoatOperatorActivated->Deactivate();
    }
}