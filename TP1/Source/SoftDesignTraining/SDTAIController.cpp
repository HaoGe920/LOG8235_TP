// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"

#include "SDTUtils.h"
#include "SDTCollectible.h"
#include "SoftDesignTrainingMainCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"


// ==========================================================
// TICK
// ==========================================================

void ASDTAIController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    APawn* pawn = GetPawn();

    if (!pawn)
    {
        return;
    }


    // ==================================================
    // 1. DEFAULT DESIRED DIRECTION
    // ==================================================

    FVector desiredDirection;

    if (m_velocity.IsNearlyZero())
    {
        desiredDirection = pawn->GetActorForwardVector();
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

    if (normalizedDirection.IsNearlyZero())
    {
        return false;
    }

    normalizedDirection.Normalize();

    FVector start = pawn->GetActorLocation();
    FVector end = start + normalizedDirection * distance;

    float radius = capsule->GetScaledCapsuleRadius();

    float halfHeight = capsule->GetScaledCapsuleHalfHeight();

    FCollisionShape collisionShape = FCollisionShape::MakeCapsule(radius,halfHeight);

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(pawn);

    if (m_targetPlayer.IsValid())
    {
        queryParams.AddIgnoredActor(m_targetPlayer.Get());
    }

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


// DEATH-FLOOR PATH CHECK
bool ASDTAIController::IsDeathFloorPathUnsafe(
    const FVector& direction,
    float distance
)
{
    APawn* pawn = GetPawn();

    if (!pawn)
    {
        return true;
    }

    UCapsuleComponent* capsule = pawn->FindComponentByClass<UCapsuleComponent>();

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
}


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
            bool foundDirection = false;
            float bestScore = -FLT_MAX;

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