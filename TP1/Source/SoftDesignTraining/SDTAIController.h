// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

/**
 * 
 */
class ASDTCollectible;
class ASoftDesignTrainingMainCharacter;

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
    float const m_maxDeceleration = 900.0f;
    float const m_turnSpeed = 180.0f;
    float const m_minNavigationSpeed = 80.0f;
    FVector m_velocity = FVector::ZeroVector;
    FVector m_lastSafeDirection = FVector::ForwardVector;
    // Emergency
    bool m_isEmergencyTurning = false;
    FVector m_emergencyTurnDirection = FVector::ZeroVector;
    bool m_wasFleeing = false;
    float const m_emergencyTurnSpeedThreshold = 60.0f;
    float const m_emergencyPhysicalTurnAlignment = 0.5f;
    float const m_emergencyTurnAlignment = 0.995f;


    // Nav safety
    float const m_minLookAheadDistance = 150.0f;
    float const m_lookAheadTime = 0.6f;
    float const m_maxLookAheadDistance = 350.0f;
    float const m_wallClearance = 5.0f;
    float const m_hardWallClearance = 1.0f;
    float const m_emergencyProbeDistance = 80.0f;
    float const m_deathFloorClearance = 5.0f;
    float const m_deathFloorProbeSink = 3.0f;

    // Collectible detection
    float const m_collectibleDetectionRadius = 600.0f;
    float const m_visionAngle = 120.0f;

    // Player detection
    float const m_playerDetectionRadius = 700.0f;
    float const m_playerLoseRadius = 1500.0f;
    TWeakObjectPtr<ASoftDesignTrainingMainCharacter> m_targetPlayer;
    bool m_hasFleeDirection = false;
    FVector m_lastFleeDirection = FVector::ZeroVector;

    // Helper functions
        //Nav functions
    float GetLookAheadDistance() const;
    bool SweepDirection(const FVector& direction, float distance, float clearance, FHitResult& hitResult);
    bool IsDeathFloorPathUnsafe(const FVector& direction, float distance);
    bool IsNavigationDirectionSafe(const FVector& direction, float distance);
    bool FindSafeDirection(const FVector& desiredDirection, FVector& safeDirection);    
    float GetTargetSpeedForDirection(const FVector& direction);
    void UpdateVelocityTowardsDirection(const FVector& direction, float deltaTime);
    float GetTurnLimitedSpeed(const FVector& currentDirection,const FVector& targetDirection);
    FVector GetSafeMovementDelta(const FVector& desiredMovementDelta);
        //Collectible functions
    bool DetectClosestCollectible(ASDTCollectible*& collectible);
    bool IsPathClearToCollectible(ASDTCollectible* collectible);
        //Player functions
    bool DetectPlayer(ASoftDesignTrainingMainCharacter*& player);
    float const m_fleeDirectionSwitchThreshold = 0.15f;
    bool FindSafeFleeDirection(ASoftDesignTrainingMainCharacter* player,FVector& safeDirection);
    void StartEmergencyTurn(const FVector& direction);
    void UpdateEmergencyTurn(APawn* pawn,float deltaTime);
};