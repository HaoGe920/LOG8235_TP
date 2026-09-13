// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"

void ASDTAIController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    APawn* pawn = GetPawn();

    if (!pawn)
    {
        return;
    }

    //Look for a wall

    if (!m_isAvoidingWall)
    {
        FHitResult hitResult;

        if (DetectWall(hitResult))
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

            // Find the two directions parallel to the wall

            FVector wallDirection = FVector::CrossProduct(FVector::UpVector, hitResult.ImpactNormal).GetSafeNormal();

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
    UCapsuleComponent* capsule = pawn->FindComponentByClass<UCapsuleComponent>();
    if (!capsule)
    {
        return true;
    }


    // Get the real scaled dimensions of the pawn.
    float capsuleRadius = capsule->GetScaledCapsuleRadius() * m_sweepScale;

    float capsuleHalfHeight = capsule->GetScaledCapsuleHalfHeight() * m_sweepScale;


    // Start at the pawn's current position.
    FVector start = pawn->GetActorLocation();
    FVector normalizedDirection = direction.GetSafeNormal();


    FVector end = start + normalizedDirection * detectionDistance;


    // Create a capsule matching the pawn.
    FCollisionShape collisionShape = FCollisionShape::MakeCapsule(capsuleRadius, capsuleHalfHeight);

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(pawn);

    // Sweep the capsule through space.

    bool hasHit = GetWorld()->SweepSingleByChannel(hitResult, start, end, FQuat::Identity, ECC_Visibility, collisionShape, queryParams);

    // Debug visualization

    FColor debugColor = hasHit ? FColor::Red : FColor::Green;

    // Center path of the sweep.
    DrawDebugLine(GetWorld(), start, end, debugColor, false, 0.0f, 0, 2.0f);

    // Show the capsule at the destination.
    DrawDebugCapsule(GetWorld(), end, capsuleHalfHeight, capsuleRadius, FQuat::Identity, debugColor, false, 0.0f, 0, 1.0f);


    return hasHit;
}




