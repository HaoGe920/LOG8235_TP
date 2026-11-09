// Fill out your copyright notice in the Description page of Project Settings.


#include "SDTBoatAIController.h"
#include "Kismet/GameplayStatics.h"
#include "SDTPathFollowingComponent.h"
#include "SDTBridge.h"
#include "SDTBoat.h"

#include "SDTBoatOperator.h"

namespace
{
	// Boats reach about 400 units ahead of their origin and this wait point is close to
	// Bridge_0, so stop early enough to keep the bow out from under the deck.
	const float StartWaitPointAcceptanceRadius = 400.f;
}

void ASDTBoatAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

void ASDTBoatAIController::GoToBestTarget(float deltaTime)
{
	APawn* pawn = GetPawn();
	ASDTBoat* boat = Cast<ASDTBoat>(pawn);

	switch (m_BoatState)
	{
		case BoatState::SPAWNED:
		{
			FString tag("WaitPoint_Start_Water");
			AActor* actor = FindActorWithTag(tag, false);

			if (actor != nullptr)
			{
				// TODO : Agents wants to move towards actor
				EPathFollowingRequestResult::Type result =	MoveToActor(actor, StartWaitPointAcceptanceRadius);

				if (result == EPathFollowingRequestResult::RequestSuccessful)
				{
					m_ReachedTarget = false;
					m_BoatState = BoatState::GO_TO_START_BRIDGE;
				}
				else if (result == EPathFollowingRequestResult::AlreadyAtGoal)
				{
					m_BoatState = BoatState::WAIT_AT_START_BRIDGE;
				}

			}

			break;
		}
		case BoatState::GO_TO_START_BRIDGE:
		{
			// Nothing to do
			break;
		}
		case BoatState::WAIT_AT_START_BRIDGE:
		{
			// Check if bridge is down
			FString tag("Bridge_0");
			AActor* actor = FindActorWithTag(tag, false);
			ASDTBridge* bridge = Cast<ASDTBridge>(actor);

			// Once the bridge is down, we go through
			if (bridge != nullptr && bridge->GetState() == EBridgeState::BRIDGE_UP)
			{
				m_BoatState = BoatState::GO_TO_OPERATOR;
			}

			break;
		}
		case BoatState::GO_TO_OPERATOR:
		{
			// A boat retrying a failed move keeps the operator it already reserved
			ASDTBoatOperator* boatOperator = m_ReservedOperator.Get();
			if (boatOperator == nullptr)
			{
				TArray<AActor*> foundActors;
				UGameplayStatics::GetAllActorsOfClass(this, ASDTBoatOperator::StaticClass(), foundActors);

				for (AActor* actor : foundActors)
				{
					ASDTBoatOperator* candidate = Cast<ASDTBoatOperator>(actor);
					if (candidate->IsAvailable())
					{
						boatOperator = candidate;
						break;
					}
				}
			}

			if (boatOperator == nullptr)
			{
				break;
			}

			// Reserve here rather than at the wait point: the boat may have been waiting
			// for a free operator while the bridge came down.
			ASDTBridge* bridge = Cast<ASDTBridge>(FindActorWithTag(TEXT("Bridge_0"), false));
			if (bridge == nullptr || (!m_CrossingBridge.IsValid() && !bridge->TryReserveForBoat(pawn)))
			{
				break;
			}

			EPathFollowingRequestResult::Type result =
				MoveToLocation(boatOperator->GetDropLocation(), 75.f);

			if (result == EPathFollowingRequestResult::RequestSuccessful)
			{
				boatOperator->Reserve(this);
				m_ReservedOperator = boatOperator;

				// TODO : we want to move the agent towards the DropLocation of the boatOperator
				// Check ASDTBoatOperator::GetDropLocation to get the location.
				// Note that m_ReachedTarget should be set to FALSE if the move is valid!
				m_ReachedTarget = false;
				m_CrossingBridge = bridge;
			}
			else if (result == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				boatOperator->Reserve(this);
				bridge->ReleaseReservation(pawn);
				m_CrossingBridge = nullptr;
				m_BoatState = BoatState::WAIT_AT_OPERATOR;
			}
			else
			{
				bridge->ReleaseReservation(pawn);
				m_CrossingBridge = nullptr;
			}

			break;
		}
		case BoatState::WAIT_AT_OPERATOR:
		{
			// Nothing to do, wait for notification to advance
			break;
		}
		case BoatState::GO_TO_END_BRIDGE:
		{
			// Only reached with m_ReachedTarget set, meaning the last move didn't arrive
			MoveToEndBridge();
			break;
		}
		case BoatState::WAIT_AT_END_BRIDGE:
		{
			// Check if bridge is down
			FString tag("Bridge_1");
			AActor* actor = FindActorWithTag(tag, false);
			ASDTBridge* bridge = Cast<ASDTBridge>(actor);

			// Once the bridge is down, we go through
			if (bridge != nullptr && (m_CrossingBridge.IsValid() || bridge->TryReserveForBoat(pawn)))
			{
				tag = "WaitPoint_Water_2";
				actor = FindActorWithTag(tag, false);
				EPathFollowingRequestResult::Type result = EPathFollowingRequestResult::Failed;
				if (actor != nullptr)
				{
					// TODO : Agents wants to move towards actor
					result = MoveToActor(actor, 75.f);
				}

				if (result == EPathFollowingRequestResult::RequestSuccessful)
				{
					m_BoatState = BoatState::GO_TO_DESPAWN;
					m_ReachedTarget = false;
					m_CrossingBridge = bridge;
				}
				else if (result == EPathFollowingRequestResult::AlreadyAtGoal)
				{
					bridge->ReleaseReservation(pawn);
					m_CrossingBridge = nullptr;
					m_BoatState = BoatState::DESPAWN;
				}
				else
				{
					bridge->ReleaseReservation(pawn);
					m_CrossingBridge = nullptr;
				}
			}

			break;
		}
		case BoatState::GO_TO_DESPAWN:
		{
			// Nothing to do
			break;
		}
		case BoatState::DESPAWN:
		{
			UnPossess();
			Destroy();

			pawn->Destroy();
			break;
		}
	}
}

void ASDTBoatAIController::NotifyUnloadComplete()
{
	m_BoatState = BoatState::GO_TO_END_BRIDGE;
	m_ReservedOperator = nullptr;
	MoveToEndBridge();
}

void ASDTBoatAIController::MoveToEndBridge()
{
	FString tag("WaitPoint_End_Water");
	AActor* actor = FindActorWithTag(tag, false);
	if (actor != nullptr)
	{
		// TODO : Agents wants to move towards actor
		EPathFollowingRequestResult::Type result = MoveToActor(actor, 75.f);

		if (result == EPathFollowingRequestResult::RequestSuccessful)
		{
			m_ReachedTarget = false;
		}
		else if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			m_BoatState = BoatState::WAIT_AT_END_BRIDGE;
		}
	}
}

void ASDTBoatAIController::ShowNavigationPath()
{
	// Show current navigation path DrawDebugLine and DrawDebugSphere
	// Use the UPathFollowingComponent of the AIController to get the path
	// This function is called while m_ReachedTarget is false 
	// Check void ASDTBaseAIController::Tick for how it works.
	UPathFollowingComponent* pathFollowing = GetPathFollowingComponent();

	if (pathFollowing == nullptr)
		return;

	const FNavPathSharedPtr path = pathFollowing->GetPath();

	if (!path.IsValid())
		return;

	const TArray<FNavPathPoint>& points = path->GetPathPoints();

	for (int32 i = 1; i < points.Num(); ++i)
	{
		FVector start = points[i - 1].Location;
		FVector end = points[i].Location;

		// Raise the debug path slightly above the water so it's visible
		start.Z += 25.f;
		end.Z += 25.f;

		DrawDebugLine(
			GetWorld(), start, end,
			FColor::Cyan, false, 0.f, 0, 3.f);

		DrawDebugSphere(
			GetWorld(), end, 12.f, 8,
			FColor::Cyan, false, 0.f);
	}
}

BoatState ASDTBoatAIController::GetBoatState()
{
	return m_BoatState;
}

void ASDTBoatAIController::AIStateInterrupted()
{
	StopMovement();

	m_ReachedTarget = false;
}

void ASDTBoatAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	m_ReachedTarget = true;

	// Didn't arrive: stay in this state and retry from GoToBestTarget, keeping any
	// reservations. These two states don't issue their own move, so step back to the one that does.
	if (Result.Code != EPathFollowingResult::Success)
	{
		if (m_BoatState == BoatState::GO_TO_START_BRIDGE)
		{
			m_BoatState = BoatState::SPAWNED;
		}
		else if (m_BoatState == BoatState::GO_TO_DESPAWN)
		{
			m_BoatState = BoatState::WAIT_AT_END_BRIDGE;
		}
		return;
	}

	if (m_CrossingBridge.IsValid())
	{
		m_CrossingBridge->ReleaseReservation(GetPawn());
		m_CrossingBridge = nullptr;
	}

	switch (m_BoatState)
	{
		case BoatState::GO_TO_START_BRIDGE:
		{
			m_BoatState = BoatState::WAIT_AT_START_BRIDGE;
			break;
		}
		case BoatState::GO_TO_OPERATOR:
		{
			m_BoatState = BoatState::WAIT_AT_OPERATOR;
			break;
		}
		case BoatState::GO_TO_END_BRIDGE:
		{
			m_BoatState = BoatState::WAIT_AT_END_BRIDGE;
			break;
		}
		case BoatState::GO_TO_DESPAWN:
		{
			m_BoatState = BoatState::DESPAWN;
			break;
		}
	}
}