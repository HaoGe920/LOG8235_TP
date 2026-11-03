// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAnimNotify_JumpEnd.h"
#include "SoftDesignTraining.h"
#include "SDTAIController.h"
#include "SoftDesignTrainingCharacter.h"

void USDTAnimNotify_JumpEnd::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
    //Notify that our NPC has landed
<<<<<<< HEAD
    Super::Notify(MeshComp, Animation, EventReference);

    UE_LOG(LogTemp, Warning,
        TEXT("SDT JumpEnd Notify triggered"));
=======
>>>>>>> ada6483b550039c10135d5416a93ce7a164b7a65
}
