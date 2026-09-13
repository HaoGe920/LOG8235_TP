// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()
public:
    virtual void Tick(float deltaTime) override;

protected:
    // Movement

    float const m_maxSpeed = 500.0f;
    float const m_maxAcceleration = 500.0f;
    FVector m_velocity = FVector::ZeroVector;

    // Wall detection
    float const m_wallDetectionDistance = 120.0f;
    float const m_sideDetectionDistance = 150.0f;
    float const m_sweepScale = 0.95f;


    // Wall avoidance
    bool m_isAvoidingWall = false;
    FVector m_avoidanceDirection = FVector::ZeroVector;
    float const m_avoidanceSpeed = 120.0f;

    // Rotation
    float const m_turnSpeed = 180.0f;

    // Helper functions
    bool DetectWall(FHitResult& hitResult);
    bool IsDirectionBlocked(const FVector& direction, float detectionDistance);
    bool SweepDirection(const FVector& direction, float detectionDistance, FHitResult& hitResult);

};
