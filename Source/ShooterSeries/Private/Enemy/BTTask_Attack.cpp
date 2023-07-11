// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BTTask_Attack.h"

#include "AIController.h"
#include "Enemy/Enemy.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName=TEXT("AttackNode");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetAIOwner())
	{
		if( AEnemy*Enemy{(Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn()))})
			Enemy->PlayAttackMontage(Enemy->GetRandAttackSection(),1.f);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
