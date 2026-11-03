// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "SDTCollectible.generated.h"

/**
 * 
 */

class USoundBase;
class UNiagaraSystem;

UCLASS()
class SOFTDESIGNTRAINING_API ASDTCollectible : public AStaticMeshActor
{
	GENERATED_BODY()
public:
    ASDTCollectible();

    void Collect();
    void OnCooldownDone();
    bool IsOnCooldown();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_CollectCooldownDuration = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        bool isMoveable = false;

    virtual void Tick(float deltaTime) override;
    virtual void BeginPlay() override;

    FVector initialPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feedback")
    USoundBase* m_pickupSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feedback")
    UParticleSystem* m_pickupFX = nullptr;

protected:
    FTimerHandle m_CollectCooldownTimer;
	
};
