#include "SDTPickupFeedbackComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

USDTPickupFeedbackComponent::USDTPickupFeedbackComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USDTPickupFeedbackComponent::PlayFeedback()
{
    FVector location = GetOwner()->GetActorLocation();

    if (m_CollectSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_CollectSound, location);
    }

    if (m_CollectEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_CollectEffect, location);
    }
}