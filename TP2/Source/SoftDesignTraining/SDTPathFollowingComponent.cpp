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
<<<<<<< HEAD
    static ConstructorHelpers::FObjectFinder<UCurveFloat> curveAsset(TEXT("/Game/Blueprint/JumpCurve.JumpCurve"));

    if (curveAsset.Succeeded())
    {
        jumpCurve = curveAsset.Object;
    }
=======

>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
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
<<<<<<< HEAD
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
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
    }
    else
    {
        // Update navigation along path (move along)
<<<<<<< HEAD
        AController* controller = Cast<AController>(GetOwner());
        ACharacter* character = Cast<ACharacter>(controller->GetPawn());
        UCharacterMovementComponent* movement = character->GetCharacterMovement();
        
        bool approachingJump = points.IsValidIndex(MoveSegmentEndIndex + 1) && SDTUtils::HasJumpFlag(segmentEnd);

        bool leavingJump = MoveSegmentStartIndex > 0 && SDTUtils::HasJumpFlag(points[MoveSegmentStartIndex - 1]);

        if (approachingJump || leavingJump)
        {
            FVector A = segmentStart.Location;
            FVector B = segmentEnd.Location;

            float length = FVector::Dist2D(A, B);
            float controlRatio = 0.35f;

            // Direction of the NavLink
            FVector jumpDirection = approachingJump
                ? (points[MoveSegmentEndIndex + 1].Location - B).GetSafeNormal2D()
                : (A - points[MoveSegmentStartIndex - 1].Location).GetSafeNormal2D();

            // Bezier control point
            FVector P = approachingJump
                ? B - jumpDirection * length * controlRatio
                : A + jumpDirection * length * controlRatio;

            // Quadratic Bezier
            auto Bezier = [&](float t)
                {
                    float u = 1.f - t;
                    return u * u * A + 2.f * u * t * P + t * t * B;
                };

            // Progression based on actual character speed
            float speed = character->GetVelocity().Size2D();

            approachProgress = FMath::Min(1.f,approachProgress + speed * DeltaTime / length);

            float t = approachProgress;

            // Bezier tangent
            FVector tangent = (2.f * (1.f - t) * (P - A) + 2.f * t * (B - P)).GetSafeNormal2D();

            // Lateral correction
            FVector correction = Bezier(t) - character->GetActorLocation();
            correction.Z = 0.f;

            correction -= FVector::DotProduct(correction, tangent) * tangent;

            float correctionRatio = 0.15f;

            FVector direction = (tangent + correction / (length * correctionRatio)).GetSafeNormal2D();

            movement->RequestDirectMove(direction * movement->GetMaxSpeed(), false);
        }
        else
        {
            Super::FollowPathSegment(DeltaTime);
        }
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
    }
}

/**
* This function is called every time the AI has reached a new point on the path.
* If you need to do something at a given point in the path, this is the place.
*/
void USDTPathFollowingComponent::SetMoveSegment(int32 segmentStartIndex)
{
    Super::SetMoveSegment(segmentStartIndex);

<<<<<<< HEAD
    if (!Path.IsValid()) return;

    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    if (!points.IsValidIndex(MoveSegmentStartIndex) || !points.IsValidIndex(MoveSegmentEndIndex)) return;
=======
    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];

    if (SDTUtils::HasJumpFlag(segmentStart) && FNavMeshNodeFlags(segmentStart.Flags).IsNavLink())
    {
        // Handle starting jump
<<<<<<< HEAD
        if (isJumping)  return;

        AController* controller = Cast<AController>(GetOwner());
        ACharacter* character = controller ? Cast<ACharacter>(controller->GetPawn()) : nullptr;

        if (!character) return;

        FVector jumpDirection =(points[MoveSegmentEndIndex].Location - segmentStart.Location).GetSafeNormal2D();

        float alignment = FVector::DotProduct(character->GetVelocity().GetSafeNormal2D(),jumpDirection);

        UE_LOG(LogTemp, Warning,
            TEXT("NavLink START - ApproachProgress: %.2f - Alignment: %.2f"),
            approachProgress, alignment);

        // Initialize jump
        jumpStart = character->GetActorLocation();
        jumpEnd = points[MoveSegmentEndIndex].Location;
        jumpEnd.Z += character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

        jumProgress = 0.f;
        isJumping = true;

        // Temporarily disable walking and gravity
        UCharacterMovementComponent* movement = character->GetCharacterMovement();

        previousMovementMode = movement->MovementMode;
        movement->StopMovementKeepPathing();
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
    }
    else
    {
        // Handle normal segments
<<<<<<< HEAD
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
=======
    }
}

>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
