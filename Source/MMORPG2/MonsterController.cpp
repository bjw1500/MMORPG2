// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterController.h"
#include "Network/GameManager.h"

AMonsterController::AMonsterController()
{


}

void AMonsterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AMonsterController::OnUnPossess()
{
	Super::OnUnPossess();

}
