// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Interface/BulletHitInterface.h"
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EDeathPose: uint8
{
	EDP_DeathPose1 UMETA(DisplayName="DeathPose1"),
	EDP_DeathPose2 UMETA(DisplayName="DeathPose2"),
	EDP_DeathPose3 UMETA(DisplayName="DeathPose3"),
	EDP_DeathPose4 UMETA(DisplayName="DeathPose4"),
	EDP_DeathPose5 UMETA(DisplayName="DeathPose5"),
	EDP_Max UMETA(DisplayName="Max")
};


UCLASS()
class SHOOTERSERIES_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	//anim notify event after death montage
	UFUNCTION(BlueprintCallable)
	void FinishedDying();
	//to destroy enemy
	UFUNCTION()
	void EnemyDestroy();

	//set the owner of the health bar component
	void InitializeHealthBarComponent();
	//Transforms and return patrol vector to world vector
	FVector PatrolToWorldVector(const FVector& T_PatrolVector) const;
	// reflects the bullet damage amount in screen space
	UFUNCTION(BlueprintImplementableEvent)
	void ShowBulletHitNumber(int T_BulletDamageAmount, const FVector& T_BulletHitLocation, bool isHeadShot);

	UFUNCTION()
	void DestroyBulletHitWidget(UUserWidget* T_HitNumberWidget);

	//function set the enemy stunned bool and blackboard value
	UFUNCTION(BlueprintCallable)
	void SetStunnedState(bool T_isStunned);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BulletHit_Implementation(FHitResult& T_HitResult, AActor* T_Shooter, AController* T_ShooterController) override;

	//function to play attack montage
	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(const FName& T_AttackSection, float T_PlayRate);

	//returns a random attack montage section name
	UFUNCTION(BlueprintPure)
	FName GetRandAttackSection() const;


	// function are anim notifies
	UFUNCTION(BlueprintCallable)
	void ActivateRightAttack();
	UFUNCTION(BlueprintCallable)
	void DeActivateRightAttack();
	UFUNCTION(BlueprintCallable)
	void ActivateLeftAttack();
	UFUNCTION(BlueprintCallable)
	void DeActivateLeftAttack();
	//inflict Damage on foe
	void DoDamage(AShooterCharacter* Victim);

protected:
	// reset canAttack to true
	UFUNCTION()
	void ResetCanAttack();

	void PlayDeathMontage();

	UFUNCTION()
	void OnRightHandBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);

	AShooterCharacter* CastToShooterCharacter(AActor* T_Actor) const;

	UFUNCTION()
	void OnLeftHandBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                            const FHitResult& SweepResult);

	//function set bool in attack radius and black board key attack radius
	void SetInAttackRadius(bool T_isInAttackRadius);

	UFUNCTION()
	void OnAttackSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                           const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//init allBullet HitNumber and location.
	UFUNCTION(BlueprintCallable)
	void StoreHitNumberAndLocation(UUserWidget* T_HitNumberWidget, const FVector& BulletHit);
	//called when actor dies
	void Die();
	//call back of hit react timer handle
	UFUNCTION()
	void ResetCanHitReact();
	//updated the location of bullet hit number in screen
	void UpdateBulletHitNumberWidget();

	//function triggers AgroOverLapSphere event
	UFUNCTION()
	void OnAgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                         const FHitResult& SweepResult);
	

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

	//plays the enemy hit react section
	void PlayHitReactMontage(const FName& T_MontageSection, float PlayRate = 1.0f) const;
	/*Health Bar Functions*/
	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();
	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

private:
	/*particle effect that spawn from Enemy Bullet Hit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<class UNiagaraSystem> MParticleSystem;
	/*Sound Triggers from Enemy Bullet Hit*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<USoundBase> MEnemyHitSound;
	/*Current Health of Enemy*/
	UPROPERTY(VisibleAnywhere, Category=Combat)
	float MHealth{100.f};
	/*Total Health Percentage of Enemy*/
	UPROPERTY(EditAnywhere, Category=Combat)
	float MMaxHealth{100.f};

	UPROPERTY(EditAnywhere)
	FName MHeadBoneName{"head"};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<class UWidgetComponent> MHealthBarComponent;
	/*handle that keeps check of health bar timer*/
	UPROPERTY(VisibleAnywhere, Category=Combat)
	FTimerHandle MHealthBarTimerHandle;

	float MHealthBarTimer{4.f};
	//stores Address of the HitReact Montage
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UAnimMontage> MHitReactMontage;

	/*HitReact Variables*/
	UPROPERTY(EditAnywhere)
	float MHitReactMinTime{0.85f};
	UPROPERTY(EditAnywhere)
	float MHitReactMaxTime{1.75};
	bool bMCanPlayHitReact{true};
	FTimerHandle MHitReactTimerHandle;
	/*</>*/
	/*map that stores each bullet hit widget and the location it spawns*/
	UPROPERTY(VisibleAnywhere)
	TMap<UUserWidget*, FVector> MBulletHitMap;
	//Time for bullet hit widget to be destroyed
	UPROPERTY(EditAnywhere)
	float MBulletHitDestroyTimer{1.5f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=BehaviourTree)
	TObjectPtr<class UBehaviorTree> MEnemyBehaviorTree;

	//enemy patrol location
	UPROPERTY(EditAnywhere, Category="BehaviourTree", meta=(AllowPrivateAccess, MakeEditWidget="true"))
	FVector MPatrolPoint;

	UPROPERTY(EditAnywhere, Category="BehaviourTree", meta=(AllowPrivateAccess, MakeEditWidget="true"))
	FVector MPatrolPoint2;
	// the ai controller of the enemy

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AEnemyController> MEnemyController;

	//sphere triggers enemy attack when overlapped.
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USphereComponent> MAgroSphereComponent;

	//this variable is used to keep track if enemy is stoned or not animeNotify
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	bool MisStunned{false};
	//probability chance of enemy being stunned after a bullet hit or not
	//0.f-1.f 0=enemy is nt stunned 1=100% Stunned
	float MChanceOfBeingStunned{0.5};


	//AttackVariables
	//when sphere is overlapped it triggers enemy attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USphereComponent> MAttackSphereComponent;

	bool bMisInAttackRadius{false};

	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UAnimMontage> MAttackMontage;

	FName MAttackSection1{TEXT("Attack1")};
	FName MAttackSection2{TEXT("Attack2")};
	FName MAttackSection3{TEXT("Attack3")};
	FName MAttackSection4{TEXT("Attack4")};


	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<class UBoxComponent> MRightBoxComponent;

	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<class UBoxComponent> MLeftBoxComponent;
	// to inflict Damage on shooter char
	float MBaseDamage{20.f};

	//socket name for zombie
	FName MRightHandSocket{"RightHandSocket"};

	FName MLeftHandSocket{"LeftHandSocket"};

	//determines if enemy can attack or not
	bool bCanAttack{true};

	FTimerHandle CanAttackTimerHandle;
	//wait time till next attack
	float MAttackWaitTime{2.f};

	//play when enemy actor dies
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UAnimMontage> MDeathMontage;

	//pose after Death;
	EDeathPose MDeathPose{EDeathPose::EDP_DeathPose1};
	//check for enemy death or not
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool bisDead{false};

	UPROPERTY(EditAnywhere, Category=RunWalkSpeed)
	float MMovementRunSpeed{270.f};

	UPROPERTY(EditAnywhere, Category=RunWalkSpeed)
	float MMovementWalkSpeed{78.f};

	bool bIsHavingTarget{false};

public:
	FName GetHeadBoneName() const { return MHeadBoneName; }
	FORCEINLINE float GetHealth() const { return MHealth; }
	FORCEINLINE float GetMaxHealth() const { return MMaxHealth; }
	FORCEINLINE UBehaviorTree* GetBehaviourTree() const { return MEnemyBehaviorTree; }
	FORCEINLINE EDeathPose GetDeathPose() const { return MDeathPose; }
	FORCEINLINE bool GetIsDead() const { return bisDead; }
	FORCEINLINE AEnemyController* GetEnemyController() const { return MEnemyController; }
};
