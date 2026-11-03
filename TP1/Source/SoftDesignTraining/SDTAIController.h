#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SDTAIController.generated.h"

<<<<<<< Updated upstream
/**
 * 
 */
<<<<<<< HEAD
=======

class ACharacter;
class ASDTCollectible;
class ASoftDesignTrainingMainCharacter;


>>>>>>> Stashed changes
=======
class ASDTCollectible;
class ASoftDesignTrainingMainCharacter;

>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()

public:
    virtual void Tick(float deltaTime) override;
    virtual void OnPossess(APawn* InPawn) override;


protected:
<<<<<<< HEAD
<<<<<<< Updated upstream
    // Movement
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

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
<<<<<<< HEAD
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
=======
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
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
