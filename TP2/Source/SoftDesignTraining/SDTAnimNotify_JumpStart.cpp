// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAnimNotify_JumpStart.h"
#include "SoftDesignTraining.h"
#include "SDTAIController.h"
#include "SoftDesignTrainingCharacter.h"
<<<<<<< HEAD
#include "SDTPathFollowingComponent.h"
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65

void USDTAnimNotify_JumpStart::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
    //Notify that the NPC has launched
<<<<<<< HEAD
    Super::Notify(MeshComp, Animation, EventReference);

    UE_LOG(LogTemp, Warning,
        TEXT("SDT JumpStart Notify triggered"));
    
    ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

    if (character && character->GetController())
    {
        USDTPathFollowingComponent* pathFollowing = character->GetController() ->FindComponentByClass<USDTPathFollowingComponent>();

        if (pathFollowing && pathFollowing->isJumping)
        {
            character->GetCharacterMovement() ->SetMovementMode(MOVE_Flying);
        }
    }
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
}
