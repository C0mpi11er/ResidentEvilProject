// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSERIES_API AEnemyController : public AAIController
{
	GENERATED_BODY()


public:
	AEnemyController();
	virtual void OnPossess(APawn* InPawn) override;

private:
	//Black board of the ai controller more like the brain that hold variables
    UPROPERTY(BlueprintReadWrite,Category="Ai Controller",meta=(AllowPrivateAccess))
	TObjectPtr<class UBlackboardComponent>MBlackboardComponent;
	// behaviour tree comp of the ai controller 
	UPROPERTY(BlueprintReadWrite,Category="Ai Controller",meta=(AllowPrivateAccess))
	TObjectPtr<class UBehaviorTreeComponent>MBehaviorTreeComponent;


public:
	FORCEINLINE UBlackboardComponent* GetBlackBoardComponent()const{return MBlackboardComponent;}
	FORCEINLINE UBehaviorTreeComponent*GetBehaviourTreeComponent()const{return MBehaviorTreeComponent;}
	
};
