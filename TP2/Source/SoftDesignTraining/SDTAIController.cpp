// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SDTBridge.h"
#include "SDTPathFollowingComponent.h"
#include "SoftDesignTrainingMainCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

#include "SDTUtils.h"
#include "EngineUtils.h"

#include "NavigationSystem.h"

ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USDTPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
           
}

void ASDTAIController::GoToBestTarget(float deltaTime)
{
    //Move to target depending on current behavior
    APawn* pawn = GetPawn();

    UPathFollowingComponent* path = GetPathFollowingComponent();
    if (path == nullptr)
    {
        return;
    }

    switch (m_PedestrianState)
    {
        case PedestrianState::SPAWNED:
        {
            FString tag("WaitPoint_Bridge_");
            AActor* actor = FindActorWithTag(tag);
            
            if (actor != nullptr)
            {
                // TODO : Agents wants to move towards actor
<<<<<<< HEAD
                UE_LOG(LogTemp, Warning,
                    TEXT("Agent %s - Target: %s - Position: %s"),
                    *GetNameSafe(GetPawn()),
                    *GetNameSafe(actor),
                    *actor->GetActorLocation().ToString());

                EPathFollowingRequestResult::Type result = MoveToActor(actor, 75.f);

                if (result == EPathFollowingRequestResult::RequestSuccessful)
                {
                    m_ReachedTarget = false;
                    m_PedestrianState = PedestrianState::GO_TO_BRIDGE;
                }
                else if (result == EPathFollowingRequestResult::AlreadyAtGoal)
                {
                    m_PedestrianState = PedestrianState::WAIT_AT_BRIDGE;
                }
=======

                m_PedestrianState = PedestrianState::GO_TO_BRIDGE;
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
            }
            break;
        }
        case PedestrianState::GO_TO_BRIDGE:
        {
            // Nothing to do
            break;
        }
        case PedestrianState::WAIT_AT_BRIDGE:
        {
            // Check if bridge is down
            FString tag("Bridge_");
            AActor* actor = FindActorWithTag(tag);
            ASDTBridge* bridge = Cast<ASDTBridge>(actor);
            
            // Once the bridge is down, we go through
            if (bridge != nullptr && bridge->GetState() == EBridgeState::BRIDGE_DOWN)
            {
                m_PedestrianState = PedestrianState::GO_TO_DESPAWN;
            }

            break;
        }
        case PedestrianState::GO_TO_DESPAWN:
        {
            FString tag("Despawn_");
            AActor* actor = FindActorWithTag(tag);
            
            if (actor != nullptr)
            {
                // TODO : Agents wants to move towards actor
<<<<<<< HEAD
                EPathFollowingRequestResult::Type result = MoveToActor(actor, 75.f);

                if (result == EPathFollowingRequestResult::RequestSuccessful)
                {
                    m_ReachedTarget = false;
                }
                else if (result == EPathFollowingRequestResult::AlreadyAtGoal)
                {
                    m_PedestrianState = PedestrianState::DESPAWN;
                }
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
            }
            
            break;
        }
        case PedestrianState::DESPAWN:
        {
            UnPossess();
            Destroy();

            pawn->Destroy();
            break;
        }
    }
}

void ASDTAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    m_ReachedTarget = true;

    // Safeguard
    if (Result.Code == EPathFollowingResult::Aborted)
    {
        return;
    }

    switch (m_PedestrianState)
    {
        case PedestrianState::GO_TO_BRIDGE:
        {
            m_PedestrianState = PedestrianState::WAIT_AT_BRIDGE;
            break;
        }
        case PedestrianState::GO_TO_DESPAWN:
        {
            m_PedestrianState = PedestrianState::DESPAWN;
            break;
        }
    }
}

void ASDTAIController::ShowNavigationPath()
{
    // Show current navigation path DrawDebugLine and DrawDebugSphere
    // Use the UPathFollowingComponent of the AIController to get the path
    // This function is called while m_ReachedTarget is false 
    // Check void ASDTBaseAIController::Tick for how it works.
<<<<<<< HEAD
    UPathFollowingComponent* pathFollowing = GetPathFollowingComponent();

    if (pathFollowing == nullptr)
        return;

    const FNavPathSharedPtr path = pathFollowing->GetPath();

    if (!path.IsValid())
        return;

    const TArray<FNavPathPoint>& points = path->GetPathPoints();

    for (int32 i = 1; i < points.Num(); ++i)
    {
        FVector start = points[i - 1].Location;
        FVector end = points[i].Location;

        // Raise the debug path slightly above the ground
        start.Z += 25.f;
        end.Z += 25.f;

        DrawDebugLine(
            GetWorld(), start, end,
            FColor::Green, false, 0.f, 0, 3.f);

        DrawDebugSphere(
            GetWorld(), end, 12.f, 8,
            FColor::Green, false, 0.f);
    }
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
}

void ASDTAIController::AIStateInterrupted()
{
    StopMovement();

    m_ReachedTarget = false;
}