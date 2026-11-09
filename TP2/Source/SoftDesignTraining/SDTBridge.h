// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SDTBridge.generated.h"

enum class EBridgeState
{
	BRIDGE_UP,
	BRIDGE_GOING_UP,
	BRIDGE_GOING_DOWN,
	BRIDGE_DOWN
};

struct FBridgeCrossing
{
	TWeakObjectPtr<APawn> Pawn;
	bool bStartedOnPositiveSide;
};

UCLASS()
class SOFTDESIGNTRAINING_API ASDTBridge : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDTBridge();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Bridge")
	void Activate();
	UFUNCTION(BlueprintCallable, Category = "Bridge")
	void Deactivate();

	EBridgeState GetState() const;

	// Agents call these before committing to a crossing. While a reservation is held the
	// bridge can't move, and it is released automatically once the agent is past the bridge.
	bool TryReserveForBoat(APawn* boat);
	bool TryReserveForPedestrian(APawn* pedestrian);
	void ReleaseReservation(const APawn* pawn);

protected:
	EBridgeState m_State;
	bool m_IsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bridge")
	float m_BridgeOpeningAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bridge")
	float m_BridgeOpenningSpeed;

	// Distance along the river from the bridge center within which a boat could touch the
	// deck: deck half-width (150) + how far a boat reaches from its origin (400) + margin.
	UPROPERTY(EditAnywhere, Category = "Bridge|Safety")
	float m_BoatClearance{ 600.f };

	// Distance along the deck from the bridge center that a pedestrian must pass to be
	// off the moving part: hinge (500) + capsule radius and margin.
	UPROPERTY(EditAnywhere, Category = "Bridge|Safety")
	float m_PedestrianClearance{ 560.f };

	UPROPERTY(VisibleInstanceOnly, Category = "Bridge|Safety")
	int32 m_BoatsCrossing{ 0 };

	UPROPERTY(VisibleInstanceOnly, Category = "Bridge|Safety")
	int32 m_PedestriansCrossing{ 0 };

	// True while the player holds the activate key: no new agent may reserve the bridge,
	// so it empties and can then move even if agents keep arriving.
	UPROPERTY(VisibleInstanceOnly, Category = "Bridge|Safety")
	bool m_IsActivated{ false };

private:
	void TryStartMoving();
	void RefreshCrossings();
	bool IsBoatNearDeck() const;
	bool HasCleared(const FBridgeCrossing& crossing, int32 axis, float clearance) const;
	void AddCrossing(TArray<FBridgeCrossing>& crossings, APawn* pawn, int32 axis);
	void ShowBlockedMessage(const FString& message) const;

	TArray<FBridgeCrossing> m_CrossingBoats;
	TArray<FBridgeCrossing> m_CrossingPedestrians;
};
