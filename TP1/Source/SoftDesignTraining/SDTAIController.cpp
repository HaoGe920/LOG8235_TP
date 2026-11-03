#include "SDTAIController.h"
<<<<<<< HEAD
<<<<<<< Updated upstream
#include "SoftDesignTraining.h"
=======

#include "SDTUtils.h"
#include "SDTCollectible.h"
#include "SoftDesignTrainingMainCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"


// ==========================================================
// TICK
// ==========================================================
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

void ASDTAIController::Tick(float deltaTime)
=======

#include "SDTUtils.h"
#include "SDTCollectible.h"
#include "SoftDesignTrainingMainCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


// POSSESSION
void ASDTAIController::OnPossess(APawn* InPawn)
>>>>>>> Stashed changes
{
    Super::OnPossess(InPawn);

    ACharacter* character = Cast<ACharacter>(InPawn);

    if (!character)
    {
        return;
    }
    UCharacterMovementComponent* movement = character->GetCharacterMovement();

    if (!movement)
    {
        return;
    }

<<<<<<< HEAD
<<<<<<< Updated upstream
    //Look for a wall
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

    // ==================================================
    // 1. DEFAULT DESIRED DIRECTION
    // ==================================================

    FVector desiredDirection;

    if (m_velocity.IsNearlyZero())
    {
<<<<<<< HEAD
        FHitResult hitResult;

        if (DetectWall(hitResult))
=======
    // Keep the speed configured in the Blueprint.
    m_configuredMaxSpeed = movement->MaxWalkSpeed;

    movement->bOrientRotationToMovement = true;
    movement->bUseControllerDesiredRotation = false;
    character->bUseControllerRotationYaw = false;
}

// TICK
void ASDTAIController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    ACharacter* character = Cast<ACharacter>(GetPawn());

    if (!character)
    {
        return;
    }

    UCharacterMovementComponent* movement = character->GetCharacterMovement();

    if (!movement)
    {
        return;
    }

    // DEFAULT DIRECTION
    FVector velocity = character->GetVelocity();
    velocity.Z = 0.0f;
    FVector desiredDirection;
    if (velocity.IsNearlyZero())
    {
        desiredDirection = character->GetActorForwardVector();
    }
    else
    {
        desiredDirection = velocity.GetSafeNormal();
    }

    desiredDirection.Z = 0.0f;

    if (!desiredDirection.IsNearlyZero())
    {
        desiredDirection.Normalize();
    }

    bool isFleeing = false;
    ASoftDesignTrainingMainCharacter* fleeingFromPlayer = nullptr;

    // PLAYER TARGET
    if (m_targetPlayer.IsValid())
    {
        ASoftDesignTrainingMainCharacter* player = m_targetPlayer.Get();

        float distanceToPlayer = FVector::Dist2D(character->GetActorLocation(), player->GetActorLocation());

        if (distanceToPlayer > m_playerLoseRadius)
>>>>>>> Stashed changes
        {
            FVector currentDirection;

<<<<<<< Updated upstream
            if (m_velocity.IsNearlyZero())
=======
    if (!m_targetPlayer.IsValid())
    {
        ASoftDesignTrainingMainCharacter* detectedPlayer = nullptr;

        if (DetectPlayer(detectedPlayer) && detectedPlayer)
        {
            m_targetPlayer = detectedPlayer;
        }
    }

    // PLAYER BEHAVIOR
    if (m_targetPlayer.IsValid())
    {
        ASoftDesignTrainingMainCharacter* player = m_targetPlayer.Get();

        // Player has PowerUp -> flee.
        if (player->IsPoweredUp())
        {
            isFleeing = true;
            fleeingFromPlayer = player;
            desiredDirection = character->GetActorLocation() - player->GetActorLocation();

            desiredDirection.Z = 0.0f;

            if (!desiredDirection.IsNearlyZero())
>>>>>>> Stashed changes
            {
                currentDirection = FVector::ForwardVector;
            }
            else
            {
<<<<<<< Updated upstream
                currentDirection = m_velocity.GetSafeNormal();
=======
                desiredDirection = -character->GetActorForwardVector();
                desiredDirection.Z = 0.0f;
                desiredDirection.Normalize();
>>>>>>> Stashed changes
            }

<<<<<<< Updated upstream
            // Find the two directions parallel to the wall

            FVector wallDirection;

            if (hitResult.GetActor() &&
                hitResult.GetActor()->GetName().Contains(TEXT("BP_DeathFloor")))
            {
                // Death floor detected.
                // Pick a direction perpendicular to the current movement.
                wallDirection =
                    FVector::CrossProduct(
                        FVector::UpVector,
                        currentDirection
                    ).GetSafeNormal();
            }
            else
            {
                // Normal wall detection.
                wallDirection =
                    FVector::CrossProduct(
                        FVector::UpVector,
                        hitResult.ImpactNormal
                    ).GetSafeNormal();
            }

            FVector oppositeWallDirection = -wallDirection;

            // Check whether the actual pawn body can fit in either direction.

            bool firstDirectionBlocked = IsDirectionBlocked(wallDirection, m_sideDetectionDistance);

            bool secondDirectionBlocked = IsDirectionBlocked(oppositeWallDirection, m_sideDetectionDistance);

            // Choose our new direction.

            if (!firstDirectionBlocked && secondDirectionBlocked)
            {
                m_avoidanceDirection = wallDirection;
            }
            else if (firstDirectionBlocked && !secondDirectionBlocked)
            {
                m_avoidanceDirection = oppositeWallDirection;
            }
            else if (!firstDirectionBlocked && !secondDirectionBlocked)
            {
                // Both sides are open.Pick whichever one requires the smallest change from our current movement direction.

                float firstAlignment = FVector::DotProduct(currentDirection, wallDirection);

                float secondAlignment = FVector::DotProduct(currentDirection, oppositeWallDirection);

                if (firstAlignment >= secondAlignment)
                {
                    m_avoidanceDirection = wallDirection;
                }
                else
                {
                    m_avoidanceDirection = oppositeWallDirection;
                }
            }
            else
            {
                m_avoidanceDirection = -currentDirection;
            }

            m_avoidanceDirection.Normalize();

            m_isAvoidingWall = true;
        }
    }

    // WALL AVOIDANCE
    if (m_isAvoidingWall)
    {
        FVector currentDirection;

        if (m_velocity.IsNearlyZero())
        {
            currentDirection = FVector::ForwardVector;
        }
        else
        {
            currentDirection = m_velocity.GetSafeNormal();
        }
        // Convert the current and target directions into rotations.
        FQuat currentRotation = currentDirection.ToOrientationQuat();

        FQuat targetRotation = m_avoidanceDirection.ToOrientationQuat();

        // Faster movement results in faster rotation
        float angleDifference = currentRotation.AngularDistance(targetRotation);
        float maxTurnThisFrame = FMath::DegreesToRadians(m_turnSpeed) * deltaTime;

        float alpha;

        if (angleDifference > KINDA_SMALL_NUMBER)
        {
            alpha = FMath::Clamp(maxTurnThisFrame / angleDifference, 0.0f, 1.0f);
        }
        else
        {
            alpha = 1.0f;
        }

        FQuat newRotation = FQuat::Slerp(currentRotation, targetRotation, alpha);

        FVector newDirection = newRotation.GetForwardVector();

        float currentSpeed = m_velocity.Size();
        float newSpeed = FMath::FInterpTo(currentSpeed, m_avoidanceSpeed, deltaTime, 5.0f);

        m_velocity = newDirection * newSpeed;

        float alignment = FVector::DotProduct(newDirection, m_avoidanceDirection);
        if (alignment > 0.9999f)
        {
            m_isAvoidingWall = false;
        }
=======
        desiredDirection = pawn->GetActorForwardVector();
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
    }
    else
    {
        desiredDirection = m_velocity.GetSafeNormal();
    }

    desiredDirection.Z = 0.0f;

    if (!desiredDirection.IsNearlyZero())
    {
        desiredDirection.Normalize();
    }


    // ==================================================
    // BEHAVIOR STATE FOR THIS FRAME
    // ==================================================

    bool isFleeing = false;
    ASoftDesignTrainingMainCharacter* fleeingFromPlayer = nullptr;


    // ==================================================
    // 2. MAINTAIN EXISTING PLAYER LOCK
    // ==================================================

    if (m_targetPlayer.IsValid())
    {
        ASoftDesignTrainingMainCharacter* player = m_targetPlayer.Get();

        float distanceToPlayer = FVector::Distance(
            pawn->GetActorLocation(),
            player->GetActorLocation()
        );

        if (distanceToPlayer > m_playerLoseRadius)
        {
            m_targetPlayer.Reset();
        }
    }


    // ==================================================
    // 3. ACQUIRE PLAYER
    // ==================================================

    if (!m_targetPlayer.IsValid())
    {
        ASoftDesignTrainingMainCharacter* detectedPlayer = nullptr;

        if (DetectPlayer(detectedPlayer) && detectedPlayer)
        {
            m_targetPlayer = detectedPlayer;
        }
    }


    // ==================================================
    // 4. CHOOSE BEHAVIOR
    // ==================================================

    if (m_targetPlayer.IsValid())
    {
        ASoftDesignTrainingMainCharacter* player = m_targetPlayer.Get();


        // ==============================================
        // PLAYER POWERED UP -> FLEE
        // ==============================================

        if (player->IsPoweredUp())
        {
            isFleeing = true;
            fleeingFromPlayer = player;

            desiredDirection = pawn->GetActorLocation() - player->GetActorLocation();
            desiredDirection.Z = 0.0f;

            if (!desiredDirection.IsNearlyZero())
            {
                desiredDirection.Normalize();
            }
            else
            {
                desiredDirection = -pawn->GetActorForwardVector();
                desiredDirection.Z = 0.0f;
                desiredDirection.Normalize();
            }
        }


        // PLAYER NOT POWERED UP -> PURSUE

        else
        {
            desiredDirection = player->GetActorLocation() - pawn->GetActorLocation();
            desiredDirection.Z = 0.0f;
            if (!desiredDirection.IsNearlyZero())
            {
                desiredDirection.Normalize();
            }
        }
    }


    // 5. NO PLAYER TARGET -> LOOK FOR COLLECTIBLE

    else
    {
        ASDTCollectible* collectible = nullptr;

        if (DetectClosestCollectible(collectible) && IsPathClearToCollectible(collectible))
        {
            desiredDirection = collectible->GetActorLocation() - pawn->GetActorLocation();
            desiredDirection.Z = 0.0f;

            if (!desiredDirection.IsNearlyZero())
            {
                desiredDirection.Normalize();
            }
        }
    }


    // 6. DETECT BEHAVIOR TRANSITIONS

    bool justStoppedFleeing = m_wasFleeing && !isFleeing;
    bool justStartedFleeing = !m_wasFleeing && isFleeing;

    if (m_isEmergencyTurning && (justStartedFleeing || justStoppedFleeing))
    {
        m_isEmergencyTurning = false;
        m_emergencyTurnDirection = FVector::ZeroVector;
    }


    // 7. RESET FLEE MEMORY WHEN NOT FLEEING

    if (!isFleeing)
    {
        m_hasFleeDirection = false;
        m_lastFleeDirection = FVector::ZeroVector;
    }


    // 8. CENTRAL NAVIGATION

    FVector safeDirection = FVector::ZeroVector;
    bool foundSafeDirection = false;

    if (!m_isEmergencyTurning)
    {
        // FLEE NAVIGATION

        if (isFleeing && fleeingFromPlayer)
        {
            foundSafeDirection = FindSafeFleeDirection(fleeingFromPlayer,safeDirection);
        }

        // NORMAL NAVIGATION

        else
        {
            foundSafeDirection = FindSafeDirection(desiredDirection,safeDirection);
        }


        // SHOULD WE ENTER EMERGENCY TURN MODE?

        if (foundSafeDirection && !m_velocity.IsNearlyZero())
        {
            FVector currentDirection = m_velocity.GetSafeNormal();

            currentDirection.Z = 0.0f;
            currentDirection.Normalize();

            safeDirection.Z = 0.0f;
            safeDirection.Normalize();

            float alignment = FVector::DotProduct(currentDirection,safeDirection);

            float turnLimitedSpeed = GetTurnLimitedSpeed(currentDirection,safeDirection);

            bool physicalEmergencyTurn = alignment < m_emergencyPhysicalTurnAlignment && turnLimitedSpeed < m_emergencyTurnSpeedThreshold;

            bool transitionEmergencyTurn = justStoppedFleeing && alignment < 0.0f;

            if (physicalEmergencyTurn || transitionEmergencyTurn)
            {
                StartEmergencyTurn(safeDirection);
            }
        }
    }


    // 9. EMERGENCY STOP / TURN

    if (m_isEmergencyTurning)
    {
        UpdateEmergencyTurn(pawn, deltaTime);
    }

    // 10. NORMAL SAFE MOVEMENT

    else if (foundSafeDirection)
    {
        m_lastSafeDirection = safeDirection;

        UpdateVelocityTowardsDirection(safeDirection,deltaTime);
    }


    // 11. NO SAFE DIRECTION -> BRAKE
    else
    {
        if (!m_velocity.IsNearlyZero())
        {
            FVector currentDirection = m_velocity.GetSafeNormal();

            float newSpeed = FMath::FInterpConstantTo(
                m_velocity.Size(),
                0.0f,
                deltaTime,
                m_maxDeceleration
            );

            if (newSpeed <= KINDA_SMALL_NUMBER)
            {
                m_velocity = FVector::ZeroVector;
            }
            else
            {
                m_velocity = currentDirection * newSpeed;
            }
        }
    }


    // 12. PROPOSE MOVEMENT

    FVector desiredMovementDelta = m_velocity * deltaTime;


    // 13. FINAL HARD SAFETY CHECK

    FVector safeMovementDelta = GetSafeMovementDelta(desiredMovementDelta);

    if (safeMovementDelta.SizeSquared() < desiredMovementDelta.SizeSquared())
    {
        if (deltaTime > KINDA_SMALL_NUMBER)
        {
            m_velocity = safeMovementDelta / deltaTime;
        }
        else
        {
            m_velocity = FVector::ZeroVector;
        }
    }


    // 14. APPLY MOVEMENT

    pawn->AddActorWorldOffset(safeMovementDelta,true);


    // 15. ORIENT PAWN WHILE MOVING

    if (!m_velocity.IsNearlyZero())
    {
        pawn->SetActorRotation(m_velocity.ToOrientationQuat());
    }

    // 16. SAVE BEHAVIOR STATE FOR NEXT FRAME

    m_wasFleeing = isFleeing;
}


<<<<<<< HEAD
=======
        // Normal player -> pursue.
        else
        {
            desiredDirection = player->GetActorLocation() - character->GetActorLocation();
            desiredDirection.Z = 0.0f;

            if (!desiredDirection.IsNearlyZero())
            {
                desiredDirection.Normalize();
            }
        }
    }

    // COLLECTIBLE BEHAVIOR
    else
    {
        ASDTCollectible* collectible = nullptr;

        if (DetectClosestCollectible(collectible) && IsPathClearToCollectible(collectible))
        {
            desiredDirection = collectible->GetActorLocation() - character->GetActorLocation();
            desiredDirection.Z = 0.0f;

            if (!desiredDirection.IsNearlyZero())
            {
                desiredDirection.Normalize();
            }
        }
    }

    // SAFE DIRECTION
    FVector safeDirection = FVector::ZeroVector;
    bool foundSafeDirection = false;

    if (!m_isEmergencyTurning)
    {
        if (isFleeing && fleeingFromPlayer)
        {
            foundSafeDirection = FindSafeFleeDirection(fleeingFromPlayer, safeDirection);
        }
        else
        {
            foundSafeDirection = FindSafeDirection(desiredDirection, safeDirection);
        }

        // CHECK IF A STOP-AND-TURN IS REQUIRED
        FVector currentVelocity = character->GetVelocity();
        currentVelocity.Z = 0.0f;

        if (foundSafeDirection && !currentVelocity.IsNearlyZero())
        {
            FVector currentDirection = currentVelocity.GetSafeNormal();
            safeDirection.Z = 0.0f;
            safeDirection.Normalize();
            float alignment = FVector::DotProduct(currentDirection, safeDirection);
            float turnLimitedSpeed = GetTurnLimitedSpeed(currentDirection, safeDirection);

            //If the desired direction is more than 90° away and there is not enough room to make the turn safely, stop before turning.
            if (alignment < 0.0f && turnLimitedSpeed < m_emergencyTurnSpeedThreshold)
            {
                StartEmergencyTurn(safeDirection);
            }
        }
    }
    // MOVEMENT
    if (m_isEmergencyTurning)
    {
        UpdateEmergencyTurn(character, deltaTime);
    }
    else if (foundSafeDirection)
    {
        ApplyCharacterMovement(character, safeDirection);
    }
    // If no safe direction exists, no movement input is supplied and CharacterMovement naturally brakes.
}

// LOOK AHEAD
float ASDTAIController::GetLookAheadDistance() const
{
    const ACharacter* character = Cast<ACharacter>(GetPawn());

    if (!character)
    {
        return m_minLookAheadDistance;
    }

    const UCharacterMovementComponent* movement = character->GetCharacterMovement();

    if (!movement)
    {
        return m_minLookAheadDistance;
    }

    FVector velocity = character->GetVelocity();

    velocity.Z = 0.0f;
    float speed = velocity.Size();

    // Basic anticipation based on current speed.
    float lookAheadDistance = speed * m_lookAheadTime;

    //d = v² / 2a
    float deceleration = movement->BrakingDecelerationWalking;


    if (deceleration > KINDA_SMALL_NUMBER)
    {
        float brakingDistance = FMath::Square(speed) / (2.0f * deceleration);
        lookAheadDistance = FMath::Max(lookAheadDistance, brakingDistance + m_wallClearance);
    }

    return FMath::Max(lookAheadDistance, m_minLookAheadDistance);
}

// WALL DETECTION
bool ASDTAIController::SweepDirection(const FVector& direction, float distance, FHitResult& hitResult)
{
>>>>>>> Stashed changes
=======
// START EMERGENCY TURN
void ASDTAIController::StartEmergencyTurn(const FVector& direction)
{
    FVector emergencyDirection = direction;

    emergencyDirection.Z = 0.0f;

    if (emergencyDirection.IsNearlyZero())
    {
        return;
    }

    emergencyDirection.Normalize();

    m_emergencyTurnDirection = emergencyDirection;
    m_isEmergencyTurning = true;
}


// UPDATE EMERGENCY TURN
void ASDTAIController::UpdateEmergencyTurn(APawn* pawn,float deltaTime)
{
    if (!pawn || !m_isEmergencyTurning)
    {
        return;
    }

    // PHASE 1 - BRAKE TO ZERO

    if (!m_velocity.IsNearlyZero())
    {
        FVector currentDirection = m_velocity.GetSafeNormal();

        float newSpeed = FMath::FInterpConstantTo(
            m_velocity.Size(),
            0.0f,
            deltaTime,
            m_maxDeceleration
        );

        if (newSpeed <= KINDA_SMALL_NUMBER)
        {
            m_velocity = FVector::ZeroVector;
        }
        else
        {
            m_velocity = currentDirection * newSpeed;
        }

        return;
    }

    // PHASE 2 - ROTATE IN PLACE
    m_velocity = FVector::ZeroVector;

    FVector targetDirection = m_emergencyTurnDirection;
    targetDirection.Z = 0.0f;

    if (targetDirection.IsNearlyZero())
    {
        m_isEmergencyTurning = false;
        return;
    }

    targetDirection.Normalize();

    FQuat currentRotation = pawn->GetActorQuat();
    FQuat targetRotation = targetDirection.ToOrientationQuat();

    float angleDifference = currentRotation.AngularDistance(targetRotation);

    float maxTurnThisFrame = FMath::DegreesToRadians(m_turnSpeed) * deltaTime;

    float alpha;

    if (angleDifference > KINDA_SMALL_NUMBER)
    {
        alpha = FMath::Clamp(
            maxTurnThisFrame / angleDifference,
            0.0f,
            1.0f
        );
    }
    else
    {
        alpha = 1.0f;
    }

    FQuat newRotation = FQuat::Slerp(
        currentRotation,
        targetRotation,
        alpha
    );

    pawn->SetActorRotation(newRotation);


    // CHECK WHETHER TURN IS COMPLETE
    FVector newForwardDirection = newRotation.GetForwardVector();

    newForwardDirection.Z = 0.0f;
    newForwardDirection.Normalize();

    float alignment = FVector::DotProduct(
        newForwardDirection,
        targetDirection
    );

    if (alignment > m_emergencyTurnAlignment)
    {
        pawn->SetActorRotation(targetRotation);

        m_lastSafeDirection = targetDirection;
        m_isEmergencyTurning = false;
    }
}


// LOOK-AHEAD DISTANCE

float ASDTAIController::GetLookAheadDistance() const
{
    float lookAheadDistance = m_velocity.Size() * m_lookAheadTime;

    return FMath::Clamp(
        lookAheadDistance,
        m_minLookAheadDistance,
        m_maxLookAheadDistance
    );
}


// WALL CAPSULE SWEEP
bool ASDTAIController::SweepDirection(
    const FVector& direction,
    float distance,
    float clearance,
    FHitResult& hitResult
)
{
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
    APawn* pawn = GetPawn();

    if (!pawn)
    {
        return false;
    }

    UCapsuleComponent* capsule = pawn->FindComponentByClass<UCapsuleComponent>();

    if (!capsule)
    {
        return false;
    }

    FVector normalizedDirection = direction;
    normalizedDirection.Z = 0.0f;

<<<<<<< HEAD
<<<<<<< Updated upstream
    float capsuleHalfHeight =
        capsule->GetScaledCapsuleHalfHeight() * m_sweepScale;

    // Start at the pawn's current position.
=======
    if (normalizedDirection.IsNearlyZero())
    {
        return false;
    }

    normalizedDirection.Normalize();
>>>>>>> Stashed changes
=======
    if (normalizedDirection.IsNearlyZero())
    {
        return false;
    }

    normalizedDirection.Normalize();

>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
    FVector start = pawn->GetActorLocation();
    FVector end = start + normalizedDirection * distance;

<<<<<<< HEAD
<<<<<<< Updated upstream
    FVector normalizedDirection = direction.GetSafeNormal();
=======
    float radius = capsule->GetScaledCapsuleRadius();
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

    float halfHeight = capsule->GetScaledCapsuleHalfHeight();

<<<<<<< HEAD
    // Create a capsule matching the pawn.
    FCollisionShape collisionShape =
        FCollisionShape::MakeCapsule(
            capsuleRadius,
            capsuleHalfHeight
        );
=======
    FCollisionShape collisionShape = FCollisionShape::MakeCapsule(capsule->GetScaledCapsuleRadius(), capsule->GetScaledCapsuleHalfHeight());
>>>>>>> Stashed changes
=======
    FCollisionShape collisionShape = FCollisionShape::MakeCapsule(radius,halfHeight);
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(pawn);

<<<<<<< HEAD
<<<<<<< Updated upstream
    // ---------------------------------------------------------
    // WALL DETECTION
    // ---------------------------------------------------------
=======

    // The player is a behavioral target, not a wall.
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
    if (m_targetPlayer.IsValid())
    {
        queryParams.AddIgnoredActor(m_targetPlayer.Get());
    }
<<<<<<< HEAD
>>>>>>> Stashed changes
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

    return GetWorld()->SweepSingleByChannel(
        hitResult,
        start,
        end,
        FQuat::Identity,
        ECC_Visibility,
        collisionShape,
        queryParams
    );
}

<<<<<<< HEAD
<<<<<<< Updated upstream
    // Debug visualization for wall detection.
    FColor debugColor =
        hasHit ? FColor::Red : FColor::Green;
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

// DEATH-FLOOR PATH CHECK
bool ASDTAIController::IsDeathFloorPathUnsafe(
    const FVector& direction,
    float distance
)
{
    APawn* pawn = GetPawn();

<<<<<<< HEAD
    DrawDebugCapsule(
        GetWorld(),
        end,
        capsuleHalfHeight,
        capsuleRadius,
        FQuat::Identity,
        debugColor,
        false,
        0.0f,
        0,
        1.0f
    );

    if (hasHit)
=======
// DEATH FLOOR
bool ASDTAIController::IsDeathFloorPathUnsafe(const FVector& direction, float distance)
{
    APawn* pawn = GetPawn();

=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
    if (!pawn)
    {
        return true;
    }

    UCapsuleComponent* capsule = pawn->FindComponentByClass<UCapsuleComponent>();
<<<<<<< HEAD

    if (!capsule)
    {
        return true;
    }

    FVector normalizedDirection = direction;
    normalizedDirection.Z = 0.0f;

    if (normalizedDirection.IsNearlyZero())
    {
        return false;
    }

    normalizedDirection.Normalize();
    float capsuleRadius = capsule->GetScaledCapsuleRadius();
    float capsuleHalfHeight = capsule->GetScaledCapsuleHalfHeight();
    float footprintRadius = capsuleRadius + m_deathFloorClearance;
    FVector groundProbeStart = pawn->GetActorLocation() - FVector::UpVector * (capsuleHalfHeight - footprintRadius + m_deathFloorProbeSink);
    FVector groundProbeEnd = groundProbeStart + normalizedDirection * distance;

    FCollisionShape footprintShape = FCollisionShape::MakeSphere(footprintRadius);

    FCollisionObjectQueryParams objectQueryParams;
    objectQueryParams.AddObjectTypesToQuery(COLLISION_DEATH_OBJECT);


    FCollisionQueryParams queryParams;

    queryParams.AddIgnoredActor(pawn);

    FHitResult hitResult;

    return GetWorld()->SweepSingleByObjectType(
        hitResult,
        groundProbeStart,
        groundProbeEnd,
        FQuat::Identity,
        objectQueryParams,
        footprintShape,
        queryParams
    );
}

// DIRECTION SAFETY
bool ASDTAIController::IsNavigationDirectionSafe(const FVector& direction, float distance)
{
    if (direction.IsNearlyZero())
    {
        return false;
    }

    FHitResult wallHit;
    if (SweepDirection(direction, distance, wallHit))
    {
        return false;
    }

    if (IsDeathFloorPathUnsafe(direction, distance))
    {
        return false;
    }

    return true;
}

// NORMAL SAFE DIRECTION
bool ASDTAIController::FindSafeDirection(const FVector& desiredDirection, FVector& safeDirection)
{
    APawn* pawn = GetPawn();

    if (!pawn)
    {
        return false;
    }

    FVector desired = desiredDirection;
    desired.Z = 0.0f;

    if (desired.IsNearlyZero())
    {
        return false;
    }

    desired.Normalize();
    FVector velocity = pawn->GetVelocity();
    velocity.Z = 0.0f;
    FVector currentDirection;

    if (velocity.IsNearlyZero())
    {
        currentDirection = desired;
    }
    else
    {
        currentDirection = velocity.GetSafeNormal();
    }


    float const candidateAngles[] =
    {
        0.0f,
        15.0f, -15.0f,
        30.0f, -30.0f,
        45.0f, -45.0f,
        60.0f, -60.0f,
        75.0f, -75.0f,
        90.0f, -90.0f,
        120.0f, -120.0f,
        150.0f, -150.0f,
        180.0f
    };

    auto FindBestCandidate = [&](float probeDistance, FVector& bestDirection) -> bool
        {
            bool foundDirection = false;
            float bestScore = -FLT_MAX;

            for (float angle : candidateAngles)
            {
                FVector candidateDirection = desired.RotateAngleAxis(angle, FVector::UpVector);
                candidateDirection.Z = 0.0f;
                candidateDirection.Normalize();

                if (!IsNavigationDirectionSafe(
                    candidateDirection,
                    probeDistance))
                {
                    continue;
                }


                // Mainly favor the objective, while slightly favoring continuity with the current movement.
                float desiredAlignment = FVector::DotProduct(candidateDirection, desired);
                float currentAlignment = FVector::DotProduct(candidateDirection, currentDirection);
                float score = desiredAlignment * 0.80f + currentAlignment * 0.20f;


                if (score > bestScore)
                {
                    bestScore = score;
                    bestDirection = candidateDirection;
                    foundDirection = true;
                }
            }

            return foundDirection;
        };


    // First try to anticipate normally.
    if (FindBestCandidate(GetLookAheadDistance(), safeDirection))
    {
        return true;
    }

    // If the environment is tight, look for an immediate safe direction instead.
    return FindBestCandidate(m_emergencyProbeDistance, safeDirection);
}

// FLEE DIRECTION
bool ASDTAIController::FindSafeFleeDirection(ASoftDesignTrainingMainCharacter* player, FVector& safeDirection)
{
    APawn* pawn = GetPawn();

    if (!pawn || !player)
    {
        return false;
    }

    FVector awayDirection = pawn->GetActorLocation() - player->GetActorLocation();

    awayDirection.Z = 0.0f;


    if (awayDirection.IsNearlyZero())
    {
        return false;
    }

    awayDirection.Normalize();
    FVector currentDirection = pawn->GetVelocity();

    currentDirection.Z = 0.0f;

    if (currentDirection.IsNearlyZero())
    {
        currentDirection = pawn->GetActorForwardVector();
    }

    currentDirection.Z = 0.0f;
    currentDirection.Normalize();


    float const candidateAngles[] =
    {
        0.0f,
        15.0f, -15.0f,
        30.0f, -30.0f,
        45.0f, -45.0f,
        60.0f, -60.0f,
        75.0f, -75.0f,
        90.0f, -90.0f,
        120.0f, -120.0f,
        150.0f, -150.0f,
        180.0f
    };

    auto FindBestFleeDirection = [&](float probeDistance, FVector& bestDirection) -> bool
        {
            bool foundDirection = false;
            float bestScore = -FLT_MAX;
            float currentDistance = FVector::Dist2D(pawn->GetActorLocation(), player->GetActorLocation());

            for (float angle : candidateAngles)
            {
                FVector candidateDirection = awayDirection.RotateAngleAxis(angle, FVector::UpVector);

                candidateDirection.Z = 0.0f;
                candidateDirection.Normalize();

                if (!IsNavigationDirectionSafe(candidateDirection, probeDistance))
                {
                    continue;
                }

                FVector futurePosition = pawn->GetActorLocation() + candidateDirection * probeDistance;
                float futureDistance = FVector::Dist2D(futurePosition, player->GetActorLocation());
                float distanceGain = (futureDistance - currentDistance) / probeDistance;
                distanceGain = FMath::Clamp(distanceGain, -1.0f, 1.0f);
                float currentAlignment = FVector::DotProduct(candidateDirection, currentDirection);
                float score = distanceGain * 0.75f + currentAlignment * 0.25f;

                if (score > bestScore)
                {
                    bestScore = score;
                    bestDirection = candidateDirection;
                    foundDirection = true;
                }
            }

            return foundDirection;
        };

    if (FindBestFleeDirection(GetLookAheadDistance(), safeDirection))
    {
        return true;
    }

    return FindBestFleeDirection(m_emergencyProbeDistance, safeDirection);
}

// TARGET SPEED
float ASDTAIController::GetTargetSpeedForDirection(const FVector& direction)
{
    ACharacter* character = Cast<ACharacter>(GetPawn());

    if (!character)
    {
        return 0.0f;
    }

    UCharacterMovementComponent* movement = character->GetCharacterMovement();

    if (!movement)
    {
        return 0.0f;
    }
    float maximumSpeed = m_configuredMaxSpeed;

    if (maximumSpeed <= KINDA_SMALL_NUMBER)
    {
        maximumSpeed = movement->MaxWalkSpeed;
    }
    float lookAheadDistance = GetLookAheadDistance();


    //Never intentionally move toward a death floor.
    if (IsDeathFloorPathUnsafe(direction, lookAheadDistance))
    {
        return 0.0f;
    }

    FHitResult wallHit;

    if (!SweepDirection(direction, lookAheadDistance, wallHit))
    {
        return maximumSpeed;
    }


    float availableDistance = FMath::Max(wallHit.Distance - m_wallClearance, 0.0f);
    float deceleration = movement->BrakingDecelerationWalking;

    if (deceleration <= KINDA_SMALL_NUMBER)
    {
        return 0.0f;
    }

    // v² = 2ad Maximum speed from which the pawn can still stop* before reaching the wall.
    float safeSpeed = FMath::Sqrt(2.0f * deceleration * availableDistance);

    return FMath::Clamp(safeSpeed, 0.0f, maximumSpeed);
}

// TURN SPEED LIMIT
float ASDTAIController::GetTurnLimitedSpeed(const FVector& currentDirection, const FVector& targetDirection)
{
    ACharacter* character = Cast<ACharacter>(GetPawn());

    if (!character)
    {
        return 0.0f;
    }
    UCharacterMovementComponent* movement = character->GetCharacterMovement();

    if (!movement)
    {
        return 0.0f;
    }

    float maximumSpeed = m_configuredMaxSpeed;

    if (maximumSpeed <= KINDA_SMALL_NUMBER)
    {
        maximumSpeed = movement->MaxWalkSpeed;
    }

    FVector current = currentDirection.GetSafeNormal();
    FVector target = targetDirection.GetSafeNormal();

    if (current.IsNearlyZero() || target.IsNearlyZero())
    {
        return 0.0f;
    }


    float alignment = FMath::Clamp(FVector::DotProduct(current, target), -1.0f, 1.0f);
    float angleDifference = FMath::Acos(alignment);


    if (angleDifference < FMath::DegreesToRadians(5.0f))
    {
        return maximumSpeed;
    }

    FHitResult wallHit;

    // Only limit turning speed when the pawn is currently* travelling toward a nearby wall.
    if (!SweepDirection(current, m_turnProbeDistance, wallHit))
    {
        return maximumSpeed;
    }

    float availableDistance = FMath::Max(wallHit.Distance - m_wallClearance, 0.0f);
    float turnSpeed = FMath::Abs(movement->RotationRate.Yaw);
    float angularSpeed = FMath::DegreesToRadians(turnSpeed);

    if (angularSpeed <= KINDA_SMALL_NUMBER)
    {
        return 0.0f;
    }

    float turnTime = angleDifference / angularSpeed;

    if (turnTime <= KINDA_SMALL_NUMBER)
    {
        return maximumSpeed;
    }

    // Slow down if there is not enough room to finish the turn.
    float maximumTurnSpeed = availableDistance / turnTime;

    return FMath::Clamp(maximumTurnSpeed, 0.0f, maximumSpeed);
}

// CHARACTER MOVEMENT
void ASDTAIController::ApplyCharacterMovement(ACharacter* character, const FVector& safeDirection)
{
    if (!character)
    {
        return;
    }

    UCharacterMovementComponent* movement = character->GetCharacterMovement();

    if (!movement)
    {
        return;
    }

    FVector direction = safeDirection;
    direction.Z = 0.0f;

    if (direction.IsNearlyZero())
    {
        return;
    }

    direction.Normalize();
    FVector velocity = character->GetVelocity();
    velocity.Z = 0.0f;
    float currentSpeed = velocity.Size();
    FVector currentDirection;

    if (currentSpeed > KINDA_SMALL_NUMBER)
    {
        currentDirection = velocity / currentSpeed;
    }
    else
    {
        currentDirection = direction;
    }

    float maximumSpeed = m_configuredMaxSpeed;

    if (maximumSpeed <= KINDA_SMALL_NUMBER)
    {
        maximumSpeed = movement->MaxWalkSpeed;
    }
    // Check both the direction the pawn is physically moving and the direction it wants to move toward.
    float currentPathSpeed = currentSpeed > KINDA_SMALL_NUMBER ? GetTargetSpeedForDirection(currentDirection) : maximumSpeed;


    float targetPathSpeed = GetTargetSpeedForDirection(direction);
    float turnLimitedSpeed = currentSpeed > KINDA_SMALL_NUMBER ? GetTurnLimitedSpeed(currentDirection, direction) : maximumSpeed;
    float targetSpeed = FMath::Min(currentPathSpeed, targetPathSpeed);
    targetSpeed = FMath::Min(targetSpeed, turnLimitedSpeed);
    movement->MaxWalkSpeed = FMath::Clamp(targetSpeed, 0.0f, maximumSpeed);
    movement->bOrientRotationToMovement = true;
    character->AddMovementInput(direction, 1.0f);
}

// EMERGENCY TURN
void ASDTAIController::StartEmergencyTurn(const FVector& direction)
{
    FVector targetDirection = direction;

    targetDirection.Z = 0.0f;


    if (targetDirection.IsNearlyZero())
    {
        return;
    }

    targetDirection.Normalize();

    m_emergencyTurnDirection = targetDirection;
    m_isEmergencyTurning = true;

    ACharacter* character = Cast<ACharacter>(GetPawn());


    if (character && character->GetCharacterMovement())
    {
        character->GetCharacterMovement()->bOrientRotationToMovement = false;
    }
}

void ASDTAIController::UpdateEmergencyTurn(ACharacter* character, float deltaTime)
{
    if (!character || !m_isEmergencyTurning)
    {
        return;
    }

    UCharacterMovementComponent* movement = character->GetCharacterMovement();

    if (!movement)
    {
        m_isEmergencyTurning = false;
        return;
    }

    FVector velocity = character->GetVelocity();

    velocity.Z = 0.0f;


    // First allow CharacterMovement to brake.
    if (velocity.Size() > m_emergencyStopSpeed)
    {
        return;
    }

    movement->StopMovementImmediately();

    // Then rotate in place.
    FVector targetDirection = m_emergencyTurnDirection;

    targetDirection.Z = 0.0f;

    if (targetDirection.IsNearlyZero())
    {
        movement->bOrientRotationToMovement = true;
        m_isEmergencyTurning = false;
        return;
    }

    targetDirection.Normalize();

    FQuat currentRotation = character->GetActorQuat();
    FQuat targetRotation = targetDirection.ToOrientationQuat();
    float angleDifference = currentRotation.AngularDistance(targetRotation);
    float turnSpeed = FMath::Abs(movement->RotationRate.Yaw);
    float maxTurnThisFrame = FMath::DegreesToRadians(turnSpeed) * deltaTime;

    if (maxTurnThisFrame <= KINDA_SMALL_NUMBER)
    {
        return;
    }

    float alpha = 1.0f;

    if (angleDifference > KINDA_SMALL_NUMBER)
    {
        alpha = FMath::Clamp(maxTurnThisFrame / angleDifference, 0.0f, 1.0f);
    }

    FQuat newRotation = FQuat::Slerp(currentRotation, targetRotation, alpha);
    character->SetActorRotation(newRotation);
    FVector newForward = newRotation.GetForwardVector();
    newForward.Z = 0.0f;
    newForward.Normalize();

    float alignment = FVector::DotProduct(newForward, targetDirection);

    if (alignment > m_emergencyTurnAlignment)
    {
        character->SetActorRotation(targetRotation);
        movement->MaxWalkSpeed = m_configuredMaxSpeed;
        movement->bOrientRotationToMovement = true;
        m_isEmergencyTurning = false;
    }
}

// COLLECTIBLE DETECTION
bool ASDTAIController::DetectClosestCollectible(ASDTCollectible*& collectible)
{
    APawn* pawn = GetPawn();
    collectible = nullptr;

    if (!pawn)
    {
        return false;
    }


    FCollisionObjectQueryParams objectQueryParams;

    objectQueryParams.AddObjectTypesToQuery(COLLISION_COLLECTIBLE);

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(pawn);

    FCollisionShape detectionSphere = FCollisionShape::MakeSphere(m_collectibleDetectionRadius);

    TArray<FOverlapResult> overlapResults;

    if (!GetWorld()->OverlapMultiByObjectType(
        overlapResults,
        pawn->GetActorLocation(),
        FQuat::Identity,
        objectQueryParams,
        detectionSphere,
        queryParams))
    {
        return false;
    }

    FVector currentDirection = pawn->GetVelocity();

    currentDirection.Z = 0.0f;

    if (currentDirection.IsNearlyZero())
    {
        currentDirection = pawn->GetActorForwardVector();
    }

    currentDirection.Z = 0.0f;
    currentDirection.Normalize();

    float minimumAlignment = FMath::Cos(FMath::DegreesToRadians(m_visionAngle * 0.5f));
    float closestDistanceSquared = FLT_MAX;

    for (const FOverlapResult& overlap : overlapResults)
    {
        ASDTCollectible* candidate = Cast<ASDTCollectible>(overlap.GetActor());

        if (!candidate || candidate->IsOnCooldown())
        {
            continue;
        }

        FVector directionToCollectible = candidate->GetActorLocation() - pawn->GetActorLocation();

        directionToCollectible.Z = 0.0f;
        float distanceSquared = directionToCollectible.SizeSquared();

        if (distanceSquared <= KINDA_SMALL_NUMBER)
        {
            continue;
        }
        directionToCollectible.Normalize();
        float alignment = FVector::DotProduct(currentDirection, directionToCollectible);


        if (alignment < minimumAlignment)
        {
            continue;
        }

        if (distanceSquared < closestDistanceSquared)
        {
            closestDistanceSquared = distanceSquared;

            collectible = candidate;
        }
    }


    return collectible != nullptr;
}

// COLLECTIBLE PATH
bool ASDTAIController::IsPathClearToCollectible(ASDTCollectible* collectible)
{
    APawn* pawn = GetPawn();


    if (!pawn || !collectible)
    {
        return false;
    }

    UCapsuleComponent* capsule = pawn->FindComponentByClass<UCapsuleComponent>();

    if (!capsule)
    {
        return false;
    }

    FVector start = pawn->GetActorLocation();
    FVector direction = collectible->GetActorLocation() - start;
    direction.Z = 0.0f;
    float distance = direction.Size();

    if (distance <= KINDA_SMALL_NUMBER)
>>>>>>> Stashed changes
    {
        return true;
    }

<<<<<<< Updated upstream
    // ---------------------------------------------------------
    // DEATH FLOOR DETECTION
    // ---------------------------------------------------------
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

    if (!capsule)
    {
        return true;
    }

    FVector normalizedDirection = direction;
    normalizedDirection.Z = 0.0f;

    if (normalizedDirection.IsNearlyZero())
    {
        return false;
    }

    normalizedDirection.Normalize();

    float capsuleRadius = capsule->GetScaledCapsuleRadius();

    float capsuleHalfHeight = capsule->GetScaledCapsuleHalfHeight();

    float footprintRadius = capsuleRadius + m_deathFloorClearance;

    FVector groundProbeStart = pawn->GetActorLocation() - FVector::UpVector * (capsuleHalfHeight - footprintRadius + m_deathFloorProbeSink);
    FVector groundProbeEnd = groundProbeStart + normalizedDirection * distance;
    FCollisionShape footprintShape = FCollisionShape::MakeSphere(footprintRadius);

    FCollisionObjectQueryParams objectQueryParams;
    objectQueryParams.AddObjectTypesToQuery(COLLISION_DEATH_OBJECT);

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(pawn);

    FHitResult deathFloorHit;

    return GetWorld()->SweepSingleByObjectType(
        deathFloorHit,
        groundProbeStart,
        groundProbeEnd,
        FQuat::Identity,
        objectQueryParams,
        footprintShape,
        queryParams
    );
<<<<<<< HEAD
=======
    direction.Normalize();
    FVector end = start + direction * distance;

    FCollisionShape collisionShape = FCollisionShape::MakeCapsule(capsule->GetScaledCapsuleRadius(), capsule->GetScaledCapsuleHalfHeight());
    FCollisionQueryParams queryParams;

    queryParams.AddIgnoredActor(pawn);
    queryParams.AddIgnoredActor(collectible);

    FHitResult hitResult;
    bool hasObstacle =
        GetWorld()->SweepSingleByChannel(
            hitResult,
            start,
            end,
            FQuat::Identity,
            ECC_Visibility,
            collisionShape,
            queryParams
        );


    return !hasObstacle;
}

// PLAYER DETECTION
bool ASDTAIController::DetectPlayer(ASoftDesignTrainingMainCharacter*& player)
{
    APawn* pawn = GetPawn();
    player = nullptr;

    if (!pawn)
    {
        return false;
    }

    FCollisionObjectQueryParams objectQueryParams;
    objectQueryParams.AddObjectTypesToQuery(COLLISION_PLAYER);

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(pawn);

    FCollisionShape detectionSphere = FCollisionShape::MakeSphere(m_playerDetectionRadius);

    TArray<FOverlapResult> overlapResults;


    if (!GetWorld()->OverlapMultiByObjectType(
        overlapResults,
        pawn->GetActorLocation(),
        FQuat::Identity,
        objectQueryParams,
        detectionSphere,
        queryParams))
    {
        return false;
    }

    for (const FOverlapResult& overlap :
        overlapResults)
    {
        ASoftDesignTrainingMainCharacter* detectedPlayer = Cast<ASoftDesignTrainingMainCharacter>(overlap.GetActor());
>>>>>>> Stashed changes
=======
}
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65


// COMPLETE NAVIGATION SAFETY CHECK
bool ASDTAIController::IsNavigationDirectionSafe(const FVector& direction,float distance)
{
    if (direction.IsNearlyZero())
    {
        return false;
    }

    FHitResult wallHit;

    if (SweepDirection(
        direction,
        distance,
        0.0f,
        wallHit))
    {
        return false;
    }

    if (IsDeathFloorPathUnsafe(
        direction,
        distance))
    {
        return false;
    }

    return true;
}


// FIND NORMAL SAFE DIRECTION
bool ASDTAIController::FindSafeDirection(const FVector& desiredDirection,FVector& safeDirection)
{
    FVector desired = desiredDirection;
    desired.Z = 0.0f;

    if (desired.IsNearlyZero())
    {
        return false;
    }
    desired.Normalize();

    FVector currentDirection;

    if (m_velocity.IsNearlyZero())
    {
        currentDirection = desired;
    }
    else
    {
        currentDirection = m_velocity.GetSafeNormal();
    }

    currentDirection.Z = 0.0f;
    currentDirection.Normalize();


    FVector previousSafeDirection = m_lastSafeDirection;

    previousSafeDirection.Z = 0.0f;

    if (previousSafeDirection.IsNearlyZero())
    {
        previousSafeDirection = currentDirection;
    }
    else
    {
        previousSafeDirection.Normalize();
    }

    float const candidateAngles[] =
    {
        0.0f,
        15.0f, -15.0f,
        30.0f, -30.0f,
        45.0f, -45.0f,
        60.0f, -60.0f,
        75.0f, -75.0f,
        90.0f, -90.0f,
        120.0f, -120.0f,
        150.0f, -150.0f,
        180.0f
    };


    auto FindBestCandidate =
        [&](float probeDistance, FVector& bestDirection) -> bool
        {
<<<<<<< HEAD
<<<<<<< Updated upstream
            hitResult = hit;
=======
            player = detectedPlayer;
>>>>>>> Stashed changes
=======
            bool foundDirection = false;
            float bestScore = -FLT_MAX;
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

            for (float angle : candidateAngles)
            {
                FVector candidateDirection =
                    desired.RotateAngleAxis(
                        angle,
                        FVector::UpVector
                    );

                candidateDirection.Z = 0.0f;
                candidateDirection.Normalize();

                if (!IsNavigationDirectionSafe(
                    candidateDirection,
                    probeDistance))
                {
                    continue;
                }

                float desiredAlignment = FVector::DotProduct(
                    candidateDirection,
                    desired
                );

                float currentAlignment = FVector::DotProduct(
                    candidateDirection,
                    currentDirection
                );

                float previousAlignment = FVector::DotProduct(
                    candidateDirection,
                    previousSafeDirection
                );

                float score =
                    desiredAlignment * 0.65f
                    + currentAlignment * 0.20f
                    + previousAlignment * 0.15f;

                if (score > bestScore)
                {
                    bestScore = score;
                    bestDirection = candidateDirection;
                    foundDirection = true;
                }
            }

            return foundDirection;
        };


    if (FindBestCandidate(
        GetLookAheadDistance(),
        safeDirection))
    {
        return true;
    }

    if (FindBestCandidate(
        m_emergencyProbeDistance,
        safeDirection))
    {
        return true;
    }

    return false;
}


// FIND SAFE FLEE DIRECTION WITH HYSTERESIS

bool ASDTAIController::FindSafeFleeDirection(
    ASoftDesignTrainingMainCharacter* player,
    FVector& safeDirection
)
{
    APawn* pawn = GetPawn();

    if (!pawn || !player)
    {
        return false;
    }


    FVector directionAwayFromPlayer =
        pawn->GetActorLocation()
        - player->GetActorLocation();

    directionAwayFromPlayer.Z = 0.0f;

    if (directionAwayFromPlayer.IsNearlyZero())
    {
        return false;
    }

    directionAwayFromPlayer.Normalize();


    FVector currentDirection;

    if (m_velocity.IsNearlyZero())
    {
        currentDirection =
            pawn->GetActorForwardVector();
    }
    else
    {
        currentDirection =
            m_velocity.GetSafeNormal();
    }

    currentDirection.Z = 0.0f;

    if (!currentDirection.IsNearlyZero())
    {
        currentDirection.Normalize();
    }


    float const candidateAngles[] =
    {
        0.0f,
        15.0f, -15.0f,
        30.0f, -30.0f,
        45.0f, -45.0f,
        60.0f, -60.0f,
        75.0f, -75.0f,
        90.0f, -90.0f,
        120.0f, -120.0f,
        150.0f, -150.0f,
        180.0f
    };


    auto EvaluateDirection =
        [&](const FVector& candidateDirection,
            float probeDistance) -> float
        {
            FVector futurePosition =
                pawn->GetActorLocation()
                + candidateDirection * probeDistance;

            float currentDistanceToPlayer =
                FVector::Dist2D(
                    pawn->GetActorLocation(),
                    player->GetActorLocation()
                );

            float futureDistanceToPlayer =
                FVector::Dist2D(
                    futurePosition,
                    player->GetActorLocation()
                );

            float distanceGain =
                (
                    futureDistanceToPlayer
                    - currentDistanceToPlayer
                    )
                / probeDistance;

            distanceGain = FMath::Clamp(
                distanceGain,
                -1.0f,
                1.0f
            );

            float currentAlignment =
                FVector::DotProduct(
                    candidateDirection,
                    currentDirection
                );

            float previousAlignment = 0.0f;

            if (m_hasFleeDirection &&
                !m_lastFleeDirection.IsNearlyZero())
            {
                previousAlignment =
                    FVector::DotProduct(
                        candidateDirection,
                        m_lastFleeDirection
                    );
            }

            return
                distanceGain * 0.65f
                + currentAlignment * 0.20f
                + previousAlignment * 0.15f;
        };


    auto FindDirectionAtDistance =
        [&](float probeDistance,
            FVector& selectedDirection) -> bool
        {
            bool foundCandidate = false;

            FVector bestCandidate =
                FVector::ZeroVector;

            float bestCandidateScore =
                -FLT_MAX;


            // ----------------------------------------------
            // SEARCH NEW CANDIDATES
            // ----------------------------------------------

            for (float angle : candidateAngles)
            {
                FVector candidateDirection =
                    directionAwayFromPlayer.RotateAngleAxis(
                        angle,
                        FVector::UpVector
                    );

                candidateDirection.Z = 0.0f;
                candidateDirection.Normalize();

                if (!IsNavigationDirectionSafe(
                    candidateDirection,
                    probeDistance))
                {
                    continue;
                }

                float score =
                    EvaluateDirection(
                        candidateDirection,
                        probeDistance
                    );

                if (score > bestCandidateScore)
                {
                    bestCandidateScore = score;
                    bestCandidate = candidateDirection;
                    foundCandidate = true;
                }
            }


            // ----------------------------------------------
            // CHECK REMEMBERED FLEE DIRECTION
            // ----------------------------------------------

            bool previousDirectionIsSafe = false;
            float previousDirectionScore = -FLT_MAX;

            if (m_hasFleeDirection &&
                !m_lastFleeDirection.IsNearlyZero())
            {
                previousDirectionIsSafe =
                    IsNavigationDirectionSafe(
                        m_lastFleeDirection,
                        probeDistance
                    );

                if (previousDirectionIsSafe)
                {
                    previousDirectionScore =
                        EvaluateDirection(
                            m_lastFleeDirection,
                            probeDistance
                        );
                }
            }


            // ----------------------------------------------
            // KEEP CURRENT FLEE DIRECTION
            // ----------------------------------------------

            if (previousDirectionIsSafe)
            {
                if (!foundCandidate ||
                    bestCandidateScore <=
                    previousDirectionScore
                    + m_fleeDirectionSwitchThreshold)
                {
                    selectedDirection =
                        m_lastFleeDirection;

                    return true;
                }
            }


            // ----------------------------------------------
            // SWITCH TO BETTER FLEE DIRECTION
            // ----------------------------------------------

            if (foundCandidate)
            {
                selectedDirection = bestCandidate;
                m_lastFleeDirection = bestCandidate;
                m_hasFleeDirection = true;

                return true;
            }

            return false;
        };


    if (FindDirectionAtDistance(
        GetLookAheadDistance(),
        safeDirection))
    {
        return true;
    }

    if (FindDirectionAtDistance(
        m_emergencyProbeDistance,
        safeDirection))
    {
        return true;
    }

    m_hasFleeDirection = false;
    m_lastFleeDirection = FVector::ZeroVector;

    return false;
}


// ==========================================================
// WALL / BRAKING LIMITED SPEED
// ==========================================================

float ASDTAIController::GetTargetSpeedForDirection(
    const FVector& direction
)
{
    float lookAheadDistance =
        GetLookAheadDistance();

    FHitResult wallHit;

    bool wallAhead = SweepDirection(
        direction,
        lookAheadDistance,
        0.0f,
        wallHit
    );

    if (!wallAhead)
    {
        return m_maxSpeed;
    }

    float brakingDistance = FMath::Max(
        wallHit.Distance - m_wallClearance,
        0.0f
    );

    float maximumSafeSpeed = FMath::Sqrt(
        2.0f
        * m_maxDeceleration
        * brakingDistance
    );

    return FMath::Clamp(
        maximumSafeSpeed,
        0.0f,
        m_maxSpeed
    );
}


// ==========================================================
// TURN-LIMITED SPEED
// ==========================================================

float ASDTAIController::GetTurnLimitedSpeed(
    const FVector& currentDirection,
    const FVector& targetDirection
)
{
    FVector current = currentDirection.GetSafeNormal();

    FVector target = targetDirection.GetSafeNormal();

    if (current.IsNearlyZero() || target.IsNearlyZero())
    {
        return 0.0f;
    }

    float alignment = FMath::Clamp(
        FVector::DotProduct(
            current,
            target
        ),
        -1.0f,
        1.0f
    );

    float angleDifference = FMath::Acos(alignment);

    if (angleDifference < FMath::DegreesToRadians(5.0f))
    {
        return m_maxSpeed;
    }


    FHitResult wallHit;

    bool wallAhead = SweepDirection(
        current,
        m_maxLookAheadDistance,
        0.0f,
        wallHit
    );

    if (!wallAhead)
    {
        return m_maxSpeed;
    }

    float availableDistance = FMath::Max(
        wallHit.Distance - m_wallClearance,
        0.0f
    );

    float angularSpeed =
        FMath::DegreesToRadians(
            m_turnSpeed
        );

    if (angularSpeed < KINDA_SMALL_NUMBER)
    {
        return 0.0f;
    }

    float turnTime =
        angleDifference / angularSpeed;

    if (turnTime < KINDA_SMALL_NUMBER)
    {
        return m_maxSpeed;
    }

    float maximumTurnSpeed =
        availableDistance / turnTime;

    return FMath::Clamp(
        maximumTurnSpeed,
        0.0f,
        m_maxSpeed
    );
}


// SMOOTH VELOCITY UPDATE
void ASDTAIController::UpdateVelocityTowardsDirection(const FVector& direction,float deltaTime)
{
    APawn* pawn = GetPawn();

    if (!pawn)
    {
        return;
    }

    FVector targetDirection = direction;
    targetDirection.Z = 0.0f;

    if (targetDirection.IsNearlyZero())
    {
        return;
    }

    targetDirection.Normalize();


    // CURRENT DIRECTION
    FVector currentDirection;

    if (!m_velocity.IsNearlyZero())
    {
        currentDirection = m_velocity.GetSafeNormal();
    }
    else
    {
        currentDirection = pawn->GetActorForwardVector();

        currentDirection.Z = 0.0f;
        currentDirection.Normalize();
    }

    // ROTATE TOWARD TARGET DIRECTION
    FQuat currentRotation = currentDirection.ToOrientationQuat();

    FQuat targetRotation = targetDirection.ToOrientationQuat();

    float angleDifference = currentRotation.AngularDistance(targetRotation);

    float maxTurnThisFrame =FMath::DegreesToRadians(m_turnSpeed) * deltaTime;

    float alpha;

    if (angleDifference > KINDA_SMALL_NUMBER)
    {
        alpha = FMath::Clamp(
            maxTurnThisFrame / angleDifference,
            0.0f,
            1.0f
        );
    }
    else
    {
        alpha = 1.0f;
    }

    FQuat newRotation = FQuat::Slerp(
        currentRotation,
        targetRotation,
        alpha
    );

    FVector newDirection =
        newRotation.GetForwardVector();

    newDirection.Z = 0.0f;
    newDirection.Normalize();


    // SPEED LIMITS
    float obstacleLimitedSpeed = GetTargetSpeedForDirection(newDirection);
    float turnLimitedSpeed = GetTurnLimitedSpeed(currentDirection,targetDirection);
    float targetSpeed = FMath::Min(obstacleLimitedSpeed,turnLimitedSpeed);


    // ACCELERATION / DECELERATION
    float currentSpeed = m_velocity.Size();

    float speedChangeRate;

    if (targetSpeed >= currentSpeed)
    {
        speedChangeRate = m_maxAcceleration;
    }
    else
    {
        speedChangeRate = m_maxDeceleration;
    }

    float newSpeed = FMath::FInterpConstantTo(currentSpeed,targetSpeed,deltaTime,speedChangeRate);

    m_velocity = newDirection * newSpeed;
}

// FINAL HARD MOVEMENT SAFETY
FVector ASDTAIController::GetSafeMovementDelta(const FVector& desiredMovementDelta)
{
    float desiredDistance = desiredMovementDelta.Size();

    if (desiredDistance < KINDA_SMALL_NUMBER)
    {
        return FVector::ZeroVector;
    }

    FVector movementDirection = desiredMovementDelta / desiredDistance;


    // WALL CHECK
    FHitResult wallHit;

    bool wallWouldBeHit =
        SweepDirection(
            movementDirection,
            desiredDistance,
            0.0f,
            wallHit
        );

    float safeDistance = desiredDistance;

    if (wallWouldBeHit)
    {
        safeDistance = FMath::Max(
            wallHit.Distance - m_wallClearance,
            0.0f
        );
    }


    // DEATH-FLOOR CHECK
    if (safeDistance > KINDA_SMALL_NUMBER &&
        IsDeathFloorPathUnsafe(
            movementDirection,
            safeDistance))
    {
        return FVector::ZeroVector;
    }


    return movementDirection * safeDistance;
}


// COLLECTIBLE DETECTION

bool ASDTAIController::DetectClosestCollectible(ASDTCollectible*& collectible)
{
    APawn* pawn = GetPawn();

    collectible = nullptr;

    if (!pawn)
    {
        return false;
    }

    FCollisionObjectQueryParams objectQueryParams;
    objectQueryParams.AddObjectTypesToQuery(COLLISION_COLLECTIBLE);

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(pawn);

    FCollisionShape detectionSphere = FCollisionShape::MakeSphere(m_collectibleDetectionRadius);

    TArray<FOverlapResult> overlapResults;

    bool hasOverlap =
        GetWorld()->OverlapMultiByObjectType(
            overlapResults,
            pawn->GetActorLocation(),
            FQuat::Identity,
            objectQueryParams,
            detectionSphere,
            queryParams
        );

    if (!hasOverlap)
    {
        return false;
    }

    FVector currentDirection;

    if (m_velocity.IsNearlyZero())
    {
        currentDirection = pawn->GetActorForwardVector();
    }
    else
    {
        currentDirection = m_velocity.GetSafeNormal();
    }
    currentDirection.Z = 0.0f;
    currentDirection.Normalize();

    float minimumAlignment = FMath::Cos(FMath::DegreesToRadians(m_visionAngle * 0.5f));
    float closestDistanceSquared = FLT_MAX;

    for (const FOverlapResult& overlap : overlapResults)
    {
        ASDTCollectible* candidate = Cast<ASDTCollectible>(overlap.GetActor());

        if (!candidate)
        {
            continue;
        }

        if (candidate->IsOnCooldown())
        {
            continue;
        }

        FVector directionToCollectible = candidate->GetActorLocation() - pawn->GetActorLocation();
        directionToCollectible.Z = 0.0f;

        float distanceSquared = directionToCollectible.SizeSquared();

        if (distanceSquared < KINDA_SMALL_NUMBER)
        {
            continue;
        }

        directionToCollectible.Normalize();

        float alignment = FVector::DotProduct(currentDirection,directionToCollectible);

        if (alignment < minimumAlignment)
        {
            continue;
        }

        if (distanceSquared < closestDistanceSquared)
        {
            closestDistanceSquared = distanceSquared;

            collectible = candidate;
        }
    }

    return collectible != nullptr;
}


// COLLECTIBLE PATH CHECK
bool ASDTAIController::IsPathClearToCollectible(ASDTCollectible* collectible)
{
    APawn* pawn = GetPawn();

    if (!pawn || !collectible)
    {
        return false;
    }

    UCapsuleComponent* capsule = pawn->FindComponentByClass<UCapsuleComponent>();

    if (!capsule)
    {
        return false;
    }

    FVector start = pawn->GetActorLocation();

    FVector direction = collectible->GetActorLocation() - start;

    direction.Z = 0.0f;

    float distance = direction.Size();

    if (distance < KINDA_SMALL_NUMBER)
    {
        return true;
    }

    direction.Normalize();

    FVector end = start + direction * distance;

    float radius = capsule->GetScaledCapsuleRadius();

    float halfHeight = capsule->GetScaledCapsuleHalfHeight();

    FCollisionShape collisionShape = FCollisionShape::MakeCapsule(radius,halfHeight);
    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(pawn);
    queryParams.AddIgnoredActor(collectible);

    FHitResult hitResult;

    bool hasObstacle =GetWorld()->SweepSingleByChannel(hitResult,start,end,FQuat::Identity,ECC_Visibility,collisionShape,queryParams);

    return !hasObstacle;
}

// PLAYER DETECTION
bool ASDTAIController::DetectPlayer(ASoftDesignTrainingMainCharacter*& player)
{
    APawn* pawn = GetPawn();

    player = nullptr;

    if (!pawn)
    {
        return false;
    }

    FCollisionObjectQueryParams objectQueryParams;
    objectQueryParams.AddObjectTypesToQuery(COLLISION_PLAYER);

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(pawn);

    FCollisionShape detectionSphere =FCollisionShape::MakeSphere(m_playerDetectionRadius);

    TArray<FOverlapResult> overlapResults;

    bool hasOverlap =
        GetWorld()->OverlapMultiByObjectType(
            overlapResults,
            pawn->GetActorLocation(),
            FQuat::Identity,
            objectQueryParams,
            detectionSphere,
            queryParams
        );

    if (!hasOverlap)
    {
        return false;
    }

    for (const FOverlapResult& overlap : overlapResults)
    {
        ASoftDesignTrainingMainCharacter* detectedPlayer =Cast<ASoftDesignTrainingMainCharacter>(overlap.GetActor());

        if (detectedPlayer)
        {
            player = detectedPlayer;
            return true;
        }
    }

    return false;
}