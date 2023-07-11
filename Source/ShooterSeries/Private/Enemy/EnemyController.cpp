// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Enemy/Enemy.h"
#include "BehaviorTree/BehaviorTreeComponent.h"



AEnemyController::AEnemyController()
{  //init default sub object and performing assertion
	MBehaviorTreeComponent=CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviourTreeComponent"));
	check(MBehaviorTreeComponent);

	MBlackboardComponent=CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
	check(MBlackboardComponent);
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn)
	{
		if (const AEnemy*Enemy{Cast<AEnemy>(InPawn)};Enemy->GetBehaviourTree())
        {
	        //init black board component with enemy behaviour tree black board data
			MBlackboardComponent->InitializeBlackboard(*Enemy->GetBehaviourTree()->GetBlackboardAsset());
        }
		
	}
	
}
