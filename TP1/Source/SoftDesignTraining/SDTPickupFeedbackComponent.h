#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SDTPickupFeedbackComponent.generated.h"

class USoundBase;
class UParticleSystem;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SOFTDESIGNTRAINING_API USDTPickupFeedbackComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USDTPickupFeedbackComponent();

    // Assignable dans le menu Détails, sans recompiler
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feedback")
    USoundBase* m_CollectSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feedback")
    UParticleSystem* m_CollectEffect;

    // Joue le son et l'effet assignés
    void PlayFeedback();
};