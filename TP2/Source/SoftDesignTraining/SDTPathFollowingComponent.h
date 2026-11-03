// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "SDTPathFollowingComponent.generated.h"

/**
*
*/
<<<<<<< HEAD
class ACharacter;
class UCurveFloat;

=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API USDTPathFollowingComponent : public UPathFollowingComponent
{
    GENERATED_UCLASS_BODY()

public:
    virtual void FollowPathSegment(float deltaTime) override;
    virtual void SetMoveSegment(int32 segmentStartIndex) override;
<<<<<<< HEAD
    virtual void OnPathFinished(const FPathFollowingResult& Result) override;
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

    UPROPERTY(BlueprintReadOnly)
    float jumProgress{ 0.f };

    UPROPERTY(BlueprintReadOnly)
    bool isJumping{ false };
<<<<<<< HEAD

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
    TObjectPtr<UCurveFloat> jumpCurve;

private:
    FVector jumpStart;
    FVector jumpEnd;

    float jumpSpeed = 600.f;
    float jumpHeight = 180.f;

    void FinishJump();

    float normalWalkSpeed = -1.f;
    float approachProgress = 0.f;

    TEnumAsByte<EMovementMode> previousMovementMode = MOVE_Walking;
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
};