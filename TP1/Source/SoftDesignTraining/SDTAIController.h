#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SDTAIController.generated.h"

<<<<<<< Updated upstream
/**
 * 
 */
=======

class ACharacter;
class ASDTCollectible;
class ASoftDesignTrainingMainCharacter;


>>>>>>> Stashed changes
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()

public:
    virtual void Tick(float deltaTime) override;
    virtual void OnPossess(APawn* InPawn) override;


protected:
<<<<<<< Updated upstream
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
=======

    // MOVEMENT
    float m_configuredMaxSpeed = 0.0f;

    // NAVIGATION
    float const m_minLookAheadDistance = 150.0f;
    float const m_lookAheadTime = 0.6f;
    float const m_turnProbeDistance = 350.0f;
    float const m_wallClearance = 5.0f;
    float const m_emergencyProbeDistance = 80.0f;
    float const m_deathFloorClearance = 5.0f;
    float const m_deathFloorProbeSink = 3.0f;

    // COLLECTIBLES
    float const m_collectibleDetectionRadius = 600.0f;
    float const m_visionAngle = 120.0f;

    // PLAYER
    float const m_playerDetectionRadius = 700.0f;
    float const m_playerLoseRadius = 1200.0f;

    TWeakObjectPtr<ASoftDesignTrainingMainCharacter> m_targetPlayer;

    // EMERGENCY TURN
    bool m_isEmergencyTurning = false;
    FVector m_emergencyTurnDirection = FVector::ZeroVector;
    float const m_emergencyTurnSpeedThreshold = 60.0f;
    float const m_emergencyTurnAlignment = 0.995f;
    float const m_emergencyStopSpeed = 5.0f;

    // NAVIGATION
    float GetLookAheadDistance() const;
    bool SweepDirection(const FVector& direction, float distance, FHitResult& hitResult);
    bool IsDeathFloorPathUnsafe(const FVector& direction, float distance);
    bool IsNavigationDirectionSafe(const FVector& direction, float distance);
    bool FindSafeDirection(const FVector& desiredDirection, FVector& safeDirection);
    float GetTargetSpeedForDirection(const FVector& direction);
    float GetTurnLimitedSpeed(const FVector& currentDirection, const FVector& targetDirection);
    void ApplyCharacterMovement(ACharacter* character, const FVector& safeDirection);

    // FLEE
    bool FindSafeFleeDirection(ASoftDesignTrainingMainCharacter* player, FVector& safeDirection);

    // EMERGENCY TURN
    void StartEmergencyTurn(const FVector& direction);
    void UpdateEmergencyTurn(ACharacter* character, float deltaTime);

    // COLLECTIBLES
    bool DetectClosestCollectible(ASDTCollectible*& collectible);
    bool IsPathClearToCollectible(ASDTCollectible* collectible);

    // PLAYER
    bool DetectPlayer(ASoftDesignTrainingMainCharacter*& player);
};
>>>>>>> Stashed changes
