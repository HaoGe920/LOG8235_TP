// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTPathFollowingComponent.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SDTAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "NavLinkCustomInterface.h"

#include "DrawDebugHelpers.h"

USDTPathFollowingComponent::USDTPathFollowingComponent(const FObjectInitializer& ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UCurveFloat> curveAsset(TEXT("/Game/Blueprint/JumpCurve.JumpCurve"));

    if (curveAsset.Succeeded())
    {
        jumpCurve = curveAsset.Object;
    }
}

/**
* This function is called every frame while the AI is following a path.
* MoveSegmentStartIndex and MoveSegmentEndIndex specify where we are on the path point array.
*/
void USDTPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];
    const FNavPathPoint& segmentEnd = points[MoveSegmentEndIndex];

    if (SDTUtils::HasJumpFlag(segmentStart))
    {
        // Update jump along path / nav link proxy
        if (!isJumping) return;

        AController* controller = Cast<AController>(GetOwner());
        ACharacter* character = controller ? Cast<ACharacter>(controller->GetPawn()) : nullptr;

        if (!character)
        {
            FinishJump();
            return;
        }
        
        // Orient character towards jump destination
        FVector direction = jumpEnd - jumpStart;
        direction.Z = 0.f;

        FRotator targetRotation = direction.Rotation();

        FRotator newRotation = FMath::RInterpTo(
            character->GetActorRotation(),
            targetRotation,
            DeltaTime,
            5.f
        );

        character->SetActorRotation(newRotation);

        // Calculate jump duration
        float distance = FVector::Dist2D(jumpStart, jumpEnd);
        float duration = FMath::Max(0.1f, distance / jumpSpeed);

        // Update progress
        jumProgress = FMath::Min(1.f,jumProgress + DeltaTime / duration);

        // Horizontal interpolation
        float horizontalAlpha = FMath::SmoothStep(0.f, 1.f, jumProgress);

        FVector position = FMath::Lerp(jumpStart, jumpEnd, horizontalAlpha);

        // Vertical parabolic trajectory
        float minValue, maxValue;
        jumpCurve->GetValueRange(minValue, maxValue);

        float heightFactor = jumpCurve->GetFloatValue(jumProgress)/maxValue;

        position.Z += heightFactor * jumpHeight;

        // Move character
        character->SetActorLocation(position, false);

        if (jumProgress >= 1.f)
            FinishJump();
    }

    else
    {
        AController* controller = Cast<AController>(GetOwner());
        ACharacter* character = Cast<ACharacter>(controller->GetPawn());
        UCharacterMovementComponent* movement = character->GetCharacterMovement();

        const bool approachingJump =
            points.IsValidIndex(MoveSegmentEndIndex + 1) &&
            SDTUtils::HasJumpFlag(segmentEnd);

        const bool leavingJump =
            MoveSegmentStartIndex > 0 &&
            SDTUtils::HasJumpFlag(points[MoveSegmentStartIndex - 1]);

        if (!approachingJump && !leavingJump)
        {
            Super::FollowPathSegment(DeltaTime);
            return;
        }

        const FVector A = segmentStart.Location;
        const FVector B = segmentEnd.Location;

        const FVector pathDirection = (B - A).GetSafeNormal2D();
        const FVector jumpDirection = approachingJump
            ? (points[MoveSegmentEndIndex + 1].Location - B).GetSafeNormal2D()
            : (A - points[MoveSegmentStartIndex - 1].Location).GetSafeNormal2D();

        const float segmentLength = FVector::Dist2D(A, B);

        const float anticipationTime = 1.2f;
        const float curveLength = FMath::Min(
            segmentLength, movement->GetMaxSpeed() * anticipationTime);

        const float controlRatio = 0.35f;
        const float correctionRatio = 0.15f;

        // Local Bezier endpoints
        const FVector curveStart = approachingJump
            ? B - pathDirection * curveLength
            : A;

        const FVector curveEnd = approachingJump
            ? B
            : A + pathDirection * curveLength;

        // Match the incoming and outgoing tangents
        const FVector startDirection = approachingJump
            ? pathDirection
            : jumpDirection;

        const FVector endDirection = approachingJump
            ? jumpDirection
            : pathDirection;

        // Cubic Bezier control points
        const FVector P1 =
            curveStart + startDirection * curveLength * controlRatio;

        const FVector P2 =
            curveEnd - endDirection * curveLength * controlRatio;

        const FVector position = character->GetActorLocation();

        // Use the normal path outside the local Bezier
        const float traveled = FVector::DotProduct(
            position - curveStart, pathDirection);

        if ((approachingJump && traveled < 0.f) ||
            (leavingJump && traveled > curveLength))
        {
            Super::FollowPathSegment(DeltaTime);
            return;
        }

        // Cubic Bezier evaluation
        auto Bezier = [&](float t)
            {
                const float u = 1.f - t;

                return
                    u * u * u * curveStart +
                    3.f * u * u * t * P1 +
                    3.f * u * t * t * P2 +
                    t * t * t * curveEnd;
            };

        // Find the closest position on the Bezier
        constexpr int32 samples = 20;

        float t = 0.f;
        float bestDistance = BIG_NUMBER;

        FVector previous = curveStart;

        for (int32 i = 1; i <= samples; ++i)
        {
            const FVector current = Bezier(
                static_cast<float>(i) / samples);

            FVector edge = current - previous;
            edge.Z = 0.f;

            FVector offset = position - previous;
            offset.Z = 0.f;

            const float alpha = FMath::Clamp(
                FVector::DotProduct(offset, edge) /
                FMath::Max(edge.SizeSquared(), KINDA_SMALL_NUMBER),
                0.f, 1.f);

            const FVector closest = previous + edge * alpha;

            const float distance = FVector::DistSquared2D(
                position, closest);

            if (distance < bestDistance)
            {
                bestDistance = distance;
                t = (i - 1 + alpha) / samples;
            }

            previous = current;
        }

        if (approachingJump)
            approachProgress = t;

        // Cubic Bezier tangent
        const float u = 1.f - t;

        const FVector tangent = (
            3.f * u * u * (P1 - curveStart) +
            6.f * u * t * (P2 - P1) +
            3.f * t * t * (curveEnd - P2)
            ).GetSafeNormal2D();

        // Correct the difference between character and curve
        FVector correction = Bezier(t) - position;
        correction.Z = 0.f;

        const FVector direction = (
            tangent + correction / (curveLength * correctionRatio)
            ).GetSafeNormal2D();

        movement->RequestDirectMove(
            direction * movement->GetMaxSpeed(), false);
    }

}

/**
* This function is called every time the AI has reached a new point on the path.
* If you need to do something at a given point in the path, this is the place.
*/
void USDTPathFollowingComponent::SetMoveSegment(int32 segmentStartIndex)
{
    Super::SetMoveSegment(segmentStartIndex);

    if (!Path.IsValid()) return;

    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    if (!points.IsValidIndex(MoveSegmentStartIndex) || !points.IsValidIndex(MoveSegmentEndIndex)) return;

    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];

    if (SDTUtils::HasJumpFlag(segmentStart) && FNavMeshNodeFlags(segmentStart.Flags).IsNavLink())
    {
        // Handle starting jump
        if (isJumping)  return;

        AController* controller = Cast<AController>(GetOwner());
        ACharacter* character = controller ? Cast<ACharacter>(controller->GetPawn()) : nullptr;

        if (!character) return;

        FVector jumpDirection =(points[MoveSegmentEndIndex].Location - segmentStart.Location).GetSafeNormal2D();

        float alignment = FVector::DotProduct(character->GetVelocity().GetSafeNormal2D(),jumpDirection);

        UE_LOG(LogTemp, Warning,
            TEXT("NavLink START - ApproachProgress: %.2f - Alignment: %.2f"),
            approachProgress, alignment);

        const FVector B = points[MoveSegmentStartIndex].Location;
        const FVector C = points[MoveSegmentEndIndex].Location;

        const FVector position = character->GetActorLocation();
        const FVector direction = (C - B).GetSafeNormal2D();
        const FVector offset = position - B;

        const float lateralDistance =
            FMath::Abs(FVector::CrossProduct(direction, offset).Z);

        UE_LOG(LogTemp, Warning,
            TEXT("NAVLINK ENTRY - Distance2D: %.0f - Lateral: %.0f"),
            FVector::Dist2D(position, B),
            lateralDistance);

        // Initialize jump
        jumpStart = character->GetActorLocation();
        jumpEnd = points[MoveSegmentEndIndex].Location;
        jumpEnd.Z += character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

        jumProgress = 0.f;
        isJumping = true;

        // Temporarily disable walking and gravity
        UCharacterMovementComponent* movement = character->GetCharacterMovement();

        float signedDistance = FVector::DotProduct(
            character->GetActorLocation() - segmentStart.Location,
            jumpDirection);

        UE_LOG(LogTemp, Warning,
            TEXT("Signed distance from NavLink start: %.0f"),
            signedDistance);

        previousMovementMode = movement->MovementMode;
        movement->StopMovementKeepPathing();
        movement->SetMovementMode(MOVE_Flying);
    }
    else
    {
        // Handle normal segments
        FinishJump();
        approachProgress = 0.f;
    }
}

void USDTPathFollowingComponent::FinishJump()
{
    if (!isJumping)
        return;

    AController* controller = Cast<AController>(GetOwner());
    ACharacter* character = controller ? Cast<ACharacter>(controller->GetPawn()) : nullptr;

    if (character)
    {
        character->GetCharacterMovement()->SetMovementMode(previousMovementMode);
    }

    isJumping = false;
    jumProgress = 0.f;
}

void USDTPathFollowingComponent::OnPathFinished(const FPathFollowingResult& Result)
{
    FinishJump();
    Super::OnPathFinished(Result);
}

bool USDTPathFollowingComponent::HasReachedCurrentTarget(const FVector& CurrentLocation) const
{
    const TArray<FNavPathPoint>& points = Path->GetPathPoints();

    // Special reach condition before a jump NavLink
    if (points.IsValidIndex(MoveSegmentEndIndex + 1) &&
        SDTUtils::HasJumpFlag(points[MoveSegmentEndIndex]))
    {
        const FVector B = points[MoveSegmentEndIndex].Location;
        const FVector C = points[MoveSegmentEndIndex + 1].Location;

        const FVector jumpDirection = (C - B).GetSafeNormal2D();

        FVector offset = CurrentLocation - B;
        offset.Z = 0.f;

        // Distance to the NavLink entrance
        const float distance = offset.Size2D();

        // Signed distance to the entrance plane
        const float forwardDistance =
            FVector::DotProduct(offset, jumpDirection);

        // Lateral distance from the NavLink axis
        const float lateralDistance =
            FMath::Abs(FVector::CrossProduct(jumpDirection, offset).Z);

        const float reachRadius = 60.f;

        // Reach B or cross its plane within the allowed zone
        return distance <= reachRadius ||
            (forwardDistance >= 0.f &&
                forwardDistance <= reachRadius &&
                lateralDistance <= reachRadius);
    }

    // Standard reach condition for other segments
    return Super::HasReachedCurrentTarget(CurrentLocation);
}