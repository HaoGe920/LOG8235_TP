// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTCollectible.h"
#include "SoftDesignTraining.h"

ASDTCollectible::ASDTCollectible()
{
    PrimaryActorTick.bCanEverTick = true;
    m_FeedbackComponent = CreateDefaultSubobject<USDTPickupFeedbackComponent>(TEXT("FeedbackComponent"));
}

void ASDTCollectible::BeginPlay()
{
    Super::BeginPlay();
}

void ASDTCollectible::Collect()
{
    GetWorld()->GetTimerManager().SetTimer(m_CollectCooldownTimer, this, &ASDTCollectible::OnCooldownDone, m_CollectCooldownDuration, false);

    if (m_FeedbackComponent)
    {
        m_FeedbackComponent->PlayFeedback();
    }

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
