#include "SDTAIController.h"
<<<<<<< Updated upstream
#include "SoftDesignTraining.h"

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

<<<<<<< Updated upstream
    //Look for a wall

    if (!m_isAvoidingWall)
    {
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
    }
    else
    {
        FVector movementDirection;

        if (m_velocity.IsNearlyZero())
        {
            movementDirection = FVector::ForwardVector;
        }
        else
        {
            movementDirection = m_velocity.GetSafeNormal();
        }
        m_velocity += movementDirection * m_maxAcceleration * deltaTime;
    }

    // Limit the speed.
    m_velocity = m_velocity.GetClampedToMaxSize(m_maxSpeed);

    // Move
    pawn->AddActorWorldOffset(m_velocity * deltaTime, true);

    if (!m_velocity.IsNearlyZero())
    {
        pawn->SetActorRotation(m_velocity.ToOrientationQuat());
    }
}

bool ASDTAIController::DetectWall(FHitResult& hitResult)
{

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
    APawn* pawn = GetPawn();

    if (!pawn)
    {
        return false;
    }

    FVector start = pawn->GetActorLocation();

    FVector direction;

    if (m_velocity.IsNearlyZero())
    {
        direction = FVector::ForwardVector;
    }
    else
    {
        direction = m_velocity.GetSafeNormal();
    }
    // Faster agent = look farther ahead.

    return SweepDirection(direction, m_wallDetectionDistance, hitResult);
}

bool ASDTAIController::IsDirectionBlocked(const FVector& direction, float detectionDistance)
{
    FHitResult hitResult;
    return SweepDirection(direction, detectionDistance, hitResult);
}

bool ASDTAIController::SweepDirection(const FVector& direction, float detectionDistance, FHitResult& hitResult)
{
    APawn* pawn = GetPawn();

    if (!pawn)
    {
        return true;
    }

    // Get the pawn's collision capsule.
    UCapsuleComponent* capsule =
        pawn->FindComponentByClass<UCapsuleComponent>();

    if (!capsule)
    {
        return true;
    }

    // Get the real scaled dimensions of the pawn.
    float capsuleRadius =
        capsule->GetScaledCapsuleRadius() * m_sweepScale;

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
    FVector start = pawn->GetActorLocation();

<<<<<<< Updated upstream
    FVector normalizedDirection = direction.GetSafeNormal();

    FVector end =
        start + normalizedDirection * detectionDistance;

    // Create a capsule matching the pawn.
    FCollisionShape collisionShape =
        FCollisionShape::MakeCapsule(
            capsuleRadius,
            capsuleHalfHeight
        );
=======
    FCollisionShape collisionShape = FCollisionShape::MakeCapsule(capsule->GetScaledCapsuleRadius(), capsule->GetScaledCapsuleHalfHeight());
>>>>>>> Stashed changes

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(pawn);

<<<<<<< Updated upstream
    // ---------------------------------------------------------
    // WALL DETECTION
    // ---------------------------------------------------------
=======

    // The player is a behavioral target, not a wall.
    if (m_targetPlayer.IsValid())
    {
        queryParams.AddIgnoredActor(m_targetPlayer.Get());
    }
>>>>>>> Stashed changes

    bool hasHit = GetWorld()->SweepSingleByChannel(
        hitResult,
        start,
        end,
        FQuat::Identity,
        ECC_Visibility,
        collisionShape,
        queryParams
    );

<<<<<<< Updated upstream
    // Debug visualization for wall detection.
    FColor debugColor =
        hasHit ? FColor::Red : FColor::Green;

    DrawDebugLine(
        GetWorld(),
        start,
        end,
        debugColor,
        false,
        0.0f,
        0,
        2.0f
    );

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

    // Put the DeathFloor detection capsule ahead of the pawn.
    FVector deathFloorStart =
        start + normalizedDirection * detectionDistance;

    // Extend the bottom of the detection capsule below
    // the normal pawn capsule.
    const float deathFloorExtension = 50.0f;

    deathFloorStart.Z -= deathFloorExtension;

    // Sweep the detection capsule downward.
    FVector deathFloorEnd =
        deathFloorStart;

    deathFloorEnd.Z -= deathFloorExtension;

    FCollisionShape deathFloorCollisionShape =
        FCollisionShape::MakeCapsule(
            capsuleRadius,
            capsuleHalfHeight
        );

    TArray<FHitResult> deathFloorHits;

    bool hasHitDeathFloor = GetWorld()->SweepMultiByChannel(
        deathFloorHits,
        deathFloorStart,
        deathFloorEnd,
        FQuat::Identity,
        ECC_Visibility,
        deathFloorCollisionShape,
        queryParams
    );
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

    // Debug visualization.
    FColor deathFloorDebugColor =
        hasHitDeathFloor ? FColor::Red : FColor::Green;

    DrawDebugLine(
        GetWorld(),
        deathFloorStart,
        deathFloorEnd,
        deathFloorDebugColor,
        false,
        0.0f,
        0,
        3.0f
    );

    DrawDebugCapsule(
        GetWorld(),
        deathFloorStart,
        capsuleHalfHeight,
        capsuleRadius,
        FQuat::Identity,
        deathFloorDebugColor,
        false,
        0.0f,
        0,
        2.0f
    );

    DrawDebugCapsule(
        GetWorld(),
        deathFloorEnd,
        capsuleHalfHeight,
        capsuleRadius,
        FQuat::Identity,
        deathFloorDebugColor,
        false,
        0.0f,
        0,
        2.0f
    );

    for (const FHitResult& hit : deathFloorHits)
    {
        AActor* hitActor = hit.GetActor();

        if (hitActor &&
            hitActor->GetName().Contains(TEXT("BP_DeathFloor")))
        {
<<<<<<< Updated upstream
            hitResult = hit;
=======
            player = detectedPlayer;
>>>>>>> Stashed changes

            return true;
        }
    }

    return false;
}




