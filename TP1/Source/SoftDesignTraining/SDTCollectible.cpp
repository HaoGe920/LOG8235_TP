// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTCollectible.h"
#include "SoftDesignTraining.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


ASDTCollectible::ASDTCollectible()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASDTCollectible::BeginPlay()
{
    Super::BeginPlay();
}

void ASDTCollectible::Collect()
{
    if (IsOnCooldown())
    {
        return;
    }
    
    if (m_pickupSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this,m_pickupSound,GetActorLocation());
    }
    
    if (m_pickupFX)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),m_pickupFX,GetActorLocation());
    }

    GetWorld()->GetTimerManager().SetTimer(m_CollectCooldownTimer, this, &ASDTCollectible::OnCooldownDone, m_CollectCooldownDuration, false);

    GetStaticMeshComponent()->SetVisibility(false);
}

void ASDTCollectible::OnCooldownDone()
{
    GetWorld()->GetTimerManager().ClearTimer(m_CollectCooldownTimer);

    GetStaticMeshComponent()->SetVisibility(true);
}

bool ASDTCollectible::IsOnCooldown()
{
    return m_CollectCooldownTimer.IsValid();
}

void ASDTCollectible::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}
