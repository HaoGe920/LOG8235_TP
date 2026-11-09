// Fill out your copyright notice in the Description page of Project Settings.


#include "SDTBridge.h"
#include "SDTBoat.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

namespace
{
	// Local axes of the bridge: pedestrians cross along X, boats pass under along Y
	const int32 PedestrianAxis = 0;
	const int32 BoatAxis = 1;
}

// Sets default values
ASDTBridge::ASDTBridge()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASDTBridge::BeginPlay()
{
	Super::BeginPlay();
	
	m_State = EBridgeState::BRIDGE_UP;
	m_IsMoving = false;
	m_BridgeOpeningAlpha = 1.0f;
	m_BridgeOpenningSpeed = 0.5f;
}

// Called every frame
void ASDTBridge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RefreshCrossings();

	// The player may be holding the key while the bridge waits for agents to clear
	if (m_IsActivated && !m_IsMoving)
	{
		TryStartMoving();
	}

	if (!m_IsMoving)
	{
		return;
	}

	if (m_State == EBridgeState::BRIDGE_GOING_UP)
	{
		m_BridgeOpeningAlpha += DeltaTime * m_BridgeOpenningSpeed;
		if (m_BridgeOpeningAlpha > 1.f)
		{
			m_BridgeOpeningAlpha = 1.f;
			m_IsMoving = false;
			m_IsActivated = false; // one press moves the bridge once
			m_State = EBridgeState::BRIDGE_UP;
		}
	}
	else if (m_State == EBridgeState::BRIDGE_GOING_DOWN)
	{
		m_BridgeOpeningAlpha -= DeltaTime * m_BridgeOpenningSpeed;
		if (m_BridgeOpeningAlpha < 0.f)
		{
			m_BridgeOpeningAlpha = 0.f;
			m_IsMoving = false;
			m_IsActivated = false; // one press moves the bridge once
			m_State = EBridgeState::BRIDGE_DOWN;
		}
	}
}

void ASDTBridge::Activate()
{
	m_IsActivated = true;
	RefreshCrossings();
	TryStartMoving();
}

void ASDTBridge::Deactivate()
{
	m_IsActivated = false;
	m_IsMoving = false;
}

void ASDTBridge::TryStartMoving()
{
	if (m_State == EBridgeState::BRIDGE_UP)
	{
		if (m_BoatsCrossing > 0 || IsBoatNearDeck())
		{
			ShowBlockedMessage(TEXT("Bridge can't lower: boat passing"));
			return;
		}
		m_State = EBridgeState::BRIDGE_GOING_DOWN;
	}
	else if (m_State == EBridgeState::BRIDGE_DOWN)
	{
		if (m_PedestriansCrossing > 0)
		{
			ShowBlockedMessage(TEXT("Bridge can't lift: pedestrians crossing"));
			return;
		}
		m_State = EBridgeState::BRIDGE_GOING_UP;
	}

	m_IsMoving = true;
}

bool ASDTBridge::TryReserveForBoat(APawn* boat)
{
	if (boat == nullptr || m_State != EBridgeState::BRIDGE_UP || m_IsActivated)
	{
		return false;
	}

	AddCrossing(m_CrossingBoats, boat, BoatAxis);
	m_BoatsCrossing = m_CrossingBoats.Num();
	return true;
}

bool ASDTBridge::TryReserveForPedestrian(APawn* pedestrian)
{
	if (pedestrian == nullptr || m_State != EBridgeState::BRIDGE_DOWN || m_IsActivated)
	{
		return false;
	}

	AddCrossing(m_CrossingPedestrians, pedestrian, PedestrianAxis);
	m_PedestriansCrossing = m_CrossingPedestrians.Num();
	return true;
}

void ASDTBridge::ReleaseReservation(const APawn* pawn)
{
	const auto isPawn = [pawn](const FBridgeCrossing& crossing) { return crossing.Pawn.Get() == pawn; };

	m_CrossingBoats.RemoveAll(isPawn);
	m_CrossingPedestrians.RemoveAll(isPawn);
	m_BoatsCrossing = m_CrossingBoats.Num();
	m_PedestriansCrossing = m_CrossingPedestrians.Num();
}

void ASDTBridge::RefreshCrossings()
{
	m_CrossingBoats.RemoveAll([this](const FBridgeCrossing& crossing)
	{
		return HasCleared(crossing, BoatAxis, m_BoatClearance);
	});
	m_CrossingPedestrians.RemoveAll([this](const FBridgeCrossing& crossing)
	{
		return HasCleared(crossing, PedestrianAxis, m_PedestrianClearance);
	});

	m_BoatsCrossing = m_CrossingBoats.Num();
	m_PedestriansCrossing = m_CrossingPedestrians.Num();
}

bool ASDTBridge::HasCleared(const FBridgeCrossing& crossing, int32 axis, float clearance) const
{
	const APawn* pawn = crossing.Pawn.Get();
	if (pawn == nullptr)
	{
		return true; // destroyed while crossing
	}

	const FVector local = GetActorQuat().UnrotateVector(pawn->GetActorLocation() - GetActorLocation());
	return crossing.bStartedOnPositiveSide ? local[axis] < -clearance : local[axis] > clearance;
}

void ASDTBridge::AddCrossing(TArray<FBridgeCrossing>& crossings, APawn* pawn, int32 axis)
{
	if (crossings.ContainsByPredicate([pawn](const FBridgeCrossing& crossing) { return crossing.Pawn.Get() == pawn; }))
	{
		return;
	}

	const FVector local = GetActorQuat().UnrotateVector(pawn->GetActorLocation() - GetActorLocation());
	crossings.Add({ pawn, local[axis] >= 0.f });
}

// Also catches boats that hold no reservation, such as one queuing close to the bridge
bool ASDTBridge::IsBoatNearDeck() const
{
	for (TActorIterator<ASDTBoat> boat(GetWorld()); boat; ++boat)
	{
		const FVector local = GetActorQuat().UnrotateVector(boat->GetActorLocation() - GetActorLocation());
		if (FMath::Abs(local[BoatAxis]) < m_BoatClearance)
		{
			return true;
		}
	}

	return false;
}

void ASDTBridge::ShowBlockedMessage(const FString& message) const
{
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetUniqueID()), 0.5f, FColor::Orange, message);
	}
}

EBridgeState ASDTBridge::GetState() const
{
	return m_State;
}