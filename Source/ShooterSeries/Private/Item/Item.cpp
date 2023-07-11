// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"
#include "Curves/CurveVector.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Character/ShooterCharacter.h"
#include "HUD/PopUpWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"

// Sets default values
AItem::AItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MItemMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(MItemMeshComponent);
	MBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	MBoxComponent->SetupAttachment(RootComponent);
	MBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	MPopUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PopUpWidget"));
	MPopUpWidget->SetupAttachment(RootComponent);
	MOverlappingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	MOverlappingSphere->SetupAttachment(GetRootComponent());
	MOverlappingSphere->SetSphereRadius(400.f);
}

void AItem::InitializeWidgetInvisibility() const
{
	if (GetPopUpWidgetComponent())
		GetPopUpWidgetComponent()->SetVisibility(false);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	/*makes widget invisible*/
	InitializeWidgetInvisibility();
	/*function Triggers When Sphere comp is overlapped with*/
	if (MOverlappingSphere)
		MOverlappingSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);

	/*function Triggers When Sphere comp overlapping event ends */
	if (MOverlappingSphere)
		MOverlappingSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	if (MPopUpWidget)
	{
		if (UPopUpWidget* ItemWidget{Cast<UPopUpWidget>(MPopUpWidget->GetUserWidgetObject())})
		{
			/*set item name*/
			ItemWidget->SetItemName(MItemName);
			/*set item count*/
			ItemWidget->SetItemCount(MItemCount);
			/*sets active stars with bool in ref to item rarity*/
			SetActiveStars();
			/*set the owner of the pop up widget*/
			ItemWidget->SetItemActor(this);
			
			ItemWidget->SetStarIcon();

			
		}
	}
	/*init custom depth by disabling it*/
	InitCustomDepth();
	StartPulseTimer();
}

float AItem::SinTransform() const
{
	return MAmplitude*FMath::Sin(MRunningTime*MtimeConstant);
}

void AItem::EnableGlowMaterial() const
{
	if (MMaterialInstanceDynamic)
	{
		MMaterialInstanceDynamic->SetScalarParameterValue(FName{"GlowBlendAlpha"},0);
	}
}

void AItem::DisableGlowMaterial() const
{
	if (MMaterialInstanceDynamic)
	{
		MMaterialInstanceDynamic->SetScalarParameterValue(FName{"GlowBlendAlpha"},1);
	}
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                            const FHitResult& SweepResult)
{
	/*check for null pointer*/
	if (OtherActor)
	{
		/*cast and check if other actor is shooter char
			   init overlapping item to true*/
		if (AShooterCharacter* ShooterChar{Cast<AShooterCharacter>(OtherActor)})
			ShooterChar->CheckOverlappingItem(true);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*check for null pointer*/
	if (OtherActor)
	{
		/*cast and check if other actor is shooter char
			   init overlapping item to true*/
		if (AShooterCharacter* ShooterChar{Cast<AShooterCharacter>(OtherActor)})
			ShooterChar->CheckOverlappingItem(false);
	}
}

void AItem::StopInterping()
{
	//set is interping to false
	MisInterping = false;
	if (MShooterChar)
	{
		 //unhiglight slot after equipping
		MShooterChar->UnHighlightIconSlot();
		/*subtract from the interpolation location*/
		MShooterChar->IncreaseFInterpItemCount(MFinterpLocIndex, -1);
		//calls the pickup item from shooter char.
		MShooterChar->GetPickUpItem(this);
		PlayEquipSound();
		MCanEnableCustomDepth=true;
		DisableGlowMaterial();
		DisableCustomDepth();
	}

	//return item back to initial size
	SetActorScale3D(FVector{1.f,});
	/*for outline lights to be tripped of or on*/
	MCanEnableCustomDepth=true;
	
}

void AItem::ItemInterp(float DeltaTime)
{
	if (MShooterChar && MZCurve && MisInterping)
	{
		//get camera location item should interp to
		const FVector CameraLocation{GetSceneInterpLocation()};
		//get initial item location
		FVector ItemLocation{MItemInterpStartLoc};
		//init elapsed time since interpolation began           
		const float InterpElapsedTime{GetWorldTimerManager().GetTimerElapsed(MInterpTimerHandle)};
		// init the ZCurve Value At each point in time
		const float ZCurveValue{MZCurve->GetFloatValue(InterpElapsedTime)};
		//get the initial z axis distance of item to camera
		const FVector ItemToCam{FVector(0.f, 0.f, (CameraLocation - ItemLocation).Z)};
		//init deltaZ which is the variable the item ZCurve will be scaled by
		const double DeltaZ{ItemToCam.Size()};

		// get interpolation x and y values and set to item location x and y value 
		const float ItemInterpXValue = FMath::FInterpTo(GetActorLocation().X,
		                                                CameraLocation.X, DeltaTime, 30.f);

		const float ItemInterpYValue = FMath::FInterpTo(GetActorLocation().Y,
		                                                CameraLocation.Y, DeltaTime, 30.f);

		ItemLocation.X = ItemInterpXValue;
		ItemLocation.Y = ItemInterpYValue;

		ItemLocation.Z += ZCurveValue * DeltaZ;
		//set actor new location art runtime
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);
		//check for scale curve
		if (MItemScaleCurve)
		{
			//get float value at each point in time and init scale curve value
			const float ScaleCurveValue = MItemScaleCurve->GetFloatValue(InterpElapsedTime);
			//tweak actor mesh scale at runtime(delta) with float from scale curve
			SetActorScale3D(FVector{ScaleCurveValue, ScaleCurveValue, ScaleCurveValue});
		}
	}
}

void AItem::SetItemProperties(EItemState ItemState) const
{
	//TODO:Ensure to implement other states
	/*function sets item mesh box and sphere comp properties*/

	switch (ItemState)
	{
	case EItemState::EIS_PickUp: //item properties
		MItemMeshComponent->SetSimulatePhysics(false);
		MItemMeshComponent->SetEnableGravity(false);
		MItemMeshComponent->SetVisibility(true);
		MItemMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//sphere properties
		MOverlappingSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		MOverlappingSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//box comp properties
		MBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		MBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;

	case EItemState::EIS_Equipped: //item properties
		GetPopUpWidgetComponent()->SetVisibility(false);
		MItemMeshComponent->SetSimulatePhysics(false);
		MItemMeshComponent->SetEnableGravity(false);
		MItemMeshComponent->SetVisibility(true);
		MItemMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//sphere properties
		MOverlappingSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MOverlappingSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Box Comp Properties
		MBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Falling: //item properties
		MItemMeshComponent->SetSimulatePhysics(true);
		MItemMeshComponent->SetEnableGravity(true);
		MItemMeshComponent->SetVisibility(true);
		MItemMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MItemMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic,
		                                                  ECollisionResponse::ECR_Block);
		MItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//sphere properties
		MOverlappingSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MOverlappingSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Box Comp Properties
		MBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_EquipInterping: //item properties
		GetPopUpWidgetComponent()->SetVisibility(false);
		MItemMeshComponent->SetSimulatePhysics(false);
		MItemMeshComponent->SetEnableGravity(false);
		MItemMeshComponent->SetVisibility(true);
		MItemMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//sphere properties
		MOverlappingSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MOverlappingSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Box Comp Properties
		MBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_PickedUp:
		GetPopUpWidgetComponent()->SetVisibility(false);
		MItemMeshComponent->SetSimulatePhysics(false);
		MItemMeshComponent->SetEnableGravity(false);
		MItemMeshComponent->SetVisibility(false);
		MItemMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//sphere properties
		MOverlappingSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MOverlappingSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Box Comp Properties
		MBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	default: break;
	}
}

FVector AItem::GetSceneInterpLocation()
{
	if (MShooterChar)
	{
		switch (MItemType)
		{
		case EItemType::EIT_Ammo: return MShooterChar->GetFInterLocation(MFinterpLocIndex).MInterpSceneComponent->
		                                               GetComponentLocation();
		case EItemType::EIT_Weapon: return MShooterChar->GetFInterLocation(0).MInterpSceneComponent->
		                                                 GetComponentLocation();
		default: break;
		}
	}

	return FVector(0.f);
}

void AItem::SetItemName(FString& itemName)
{
	MItemName = itemName;
}

void AItem::SetActiveStars()
{
	/*0 index is not used*/
	for (int8 i = 0; i <= 5; i++)
	{
		MActiveStars.Add(false);
	}

	switch (MItemRarity)
	{
	case EItemRarity::EIR_Damaged: MActiveStars[1] = true;
		break;

	case EItemRarity::EIR_Common: MActiveStars[1] = true;
		MActiveStars[2] = true;
		break;

	case EItemRarity::EIR_UnCommon: MActiveStars[1] = true;
		MActiveStars[2] = true;
		MActiveStars[3] = true;
		break;

	case EItemRarity::EIR_Rare: MActiveStars[1] = true;
		MActiveStars[2] = true;
		MActiveStars[3] = true;
		MActiveStars[4] = true;

	case EItemRarity::EIR_Legendary: MActiveStars[1] = true;
		MActiveStars[2] = true;
		MActiveStars[3] = true;
		MActiveStars[4] = true;
		MActiveStars[5] = true;

	default: break;
	}
}

void AItem::SetItemState(EItemState ItemState)
{
	MItemState = ItemState;
	SetItemProperties(MItemState);
}

void AItem::StartItemCurve(AShooterCharacter* T_ShooterChar, bool T_bForcedSoundPlay)
{
	//init shooter char
	if (T_ShooterChar)
	{
		MShooterChar = T_ShooterChar;
		if (MShooterChar)
		{
			MFinterpLocIndex = MShooterChar->FinterLowestIndex();
			MShooterChar->IncreaseFInterpItemCount(MFinterpLocIndex, 1);
		}
	}

	//init location of item interpolation
	MItemInterpStartLoc = GetActorLocation();
	MisInterping = true;
	MCanEnableCustomDepth=false;
	SetItemState(EItemState::EIS_EquipInterping);
	PlayPickupSound(T_bForcedSoundPlay);
	GetWorldTimerManager().ClearTimer(MPulseTimerHandle);
	GetWorldTimerManager().SetTimer(MInterpTimerHandle, this, &AItem::StopInterping, MZCurveTime);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ItemInterp(DeltaTime);
	UpdatePulse();

	//oscillate only when item is in pickup state
	if (MItemState==EItemState::EIS_PickUp)
	{
		//init running time with delta time to give it an initial value every frame
		MRunningTime+=DeltaTime;
		//osccillate actor world offset by value return from SinTransform
		AddActorWorldOffset(FVector(0.f,0.f,SinTransform()));
		AddActorWorldRotation(FRotator(0.f,MRotationRate,0.f));	
	}
	
}

void AItem::PlayPickupSound(bool T_bForcedSoundPlay) const
{
	if (!MShooterChar)return;

	if (T_bForcedSoundPlay)
	{
		if (MPickUpSound)
			UGameplayStatics::PlaySound2D(this, MPickUpSound);
	}
	else if (MShooterChar->GetShouldPlayPickupSound() == true)
	{
		MShooterChar->StartPickupSoundTimer();
		if (MPickUpSound)
			UGameplayStatics::PlaySound2D(this, MPickUpSound);
	}
}

void AItem::PlayEquipSound(bool T_bForceSoundPlay) const
{
	if(!MShooterChar)return;

	if (T_bForceSoundPlay)
	{
		if (MEquipSound)
			UGameplayStatics::PlaySound2D(this, MEquipSound);
	}
	else if (MShooterChar->GetShouldPlayEquipSound() == true)
	{
		MShooterChar->StartEquipSoundTimer();
		if (MEquipSound)
			UGameplayStatics::PlaySound2D(this, MEquipSound);
	}
}

void AItem::EnableCustomDepth()
{
	if (MCanEnableCustomDepth)
	{
		MItemMeshComponent->SetRenderCustomDepth(true);	
	}
	
}

void AItem::DisableCustomDepth()
{
	if (MCanEnableCustomDepth)
	{
		MItemMeshComponent->SetRenderCustomDepth(false);	
	}
	
}


void AItem::InitCustomDepth()
{
	DisableCustomDepth();
}

void AItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	
	
    //init datatable with path to datatable on editor
	const FString ItemDataTablePath{
		"/Script/Engine.DataTable'/Game/Game/Character/Assets/Items/DataTable/ItemRarityDataTable.ItemRarityDataTable'"
	};
     //init the address to a pointer    
	UDataTable* DataTable{(Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(),
		nullptr, *ItemDataTablePath)))};
       //not null
	if (DataTable)
	{   //init a pointer to rarity table struct and init with nullptr pending switch cases
		const FItemRarityTable*ItemRarityTable{nullptr};

		switch (MItemRarity)
		{
		case EItemRarity::EIR_Damaged:
			ItemRarityTable=DataTable->FindRow<FItemRarityTable>(FName{"Damaged"},TEXT(""));
			break;

		case EItemRarity::EIR_Common:
			ItemRarityTable=DataTable->FindRow<FItemRarityTable>(FName{"Common"},TEXT(""));
			break;

		case EItemRarity::EIR_UnCommon:
			ItemRarityTable=DataTable->FindRow<FItemRarityTable>(FName{"UnCommon"},TEXT(""));
			break;
		case EItemRarity::EIR_Rare:
			ItemRarityTable=DataTable->FindRow<FItemRarityTable>(FName{"Rare"},TEXT(""));
			break;
		case EItemRarity::EIR_Legendary:
			ItemRarityTable=DataTable->FindRow<FItemRarityTable>(FName{"Legendary"},TEXT(""));
			break;

			default:break;
		}
           // not null init item properties with rarity table variables
		if (ItemRarityTable)
		{
			MGlowColor=ItemRarityTable->MGlowColor;
			MLightColor=ItemRarityTable->MLightColor;
			MDarkColor=ItemRarityTable->MDarkColor;
			MNumberOfStars=ItemRarityTable->MNumberOfStars;
			MItemBackground=ItemRarityTable->MItemBackground;
		}

		if (MMaterialInstance)
		{   //set item material
			MMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(MMaterialInstance, this);
			if (MMaterialInstanceDynamic&&MItemMeshComponent)
			{
				MItemMeshComponent->SetMaterial(MMaterialIndex, MMaterialInstanceDynamic);
				MMaterialInstanceDynamic->SetVectorParameterValue(FName{"FresnelColour"},MGlowColor);
			}
			//enable glow
			EnableGlowMaterial();
			
		}
	}
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::StartPulseTimer()
{
	if (MItemState==EItemState::EIS_PickUp)
	{
		GetWorldTimerManager().SetTimer(MPulseTimerHandle,this,&AItem::ResetPulseTimer,MPulseTimer);
	}
}

void AItem::UpdatePulse() const
{
	float ElapsedTime;
	FVector CurveVector;
	switch (MItemState)
	{
	case EItemState::EIS_PickUp:
		if (MVectorPulseCurve)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(MPulseTimerHandle);
			CurveVector = MVectorPulseCurve->GetVectorValue(ElapsedTime);
		}
		break;


	case EItemState::EIS_EquipInterping:
		if (MInterpingVectorCurve)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(MInterpTimerHandle);
			CurveVector = MInterpingVectorCurve->GetVectorValue(ElapsedTime);
		}
		break;
		
	default: break;
		
	}


	if (MMaterialInstanceDynamic)
	{
		MMaterialInstanceDynamic->SetScalarParameterValue(FName{"GlowAmount"}, CurveVector.X * MGlowAmount);
		MMaterialInstanceDynamic->SetScalarParameterValue(FName{"FresnelExponent"},
		                                                  CurveVector.Y * MFresnelExponent);
		MMaterialInstanceDynamic->SetScalarParameterValue(FName{"FresnelReflectFractionin"},
		                                                  CurveVector.Z * MFresnelFraction);
	}
}
