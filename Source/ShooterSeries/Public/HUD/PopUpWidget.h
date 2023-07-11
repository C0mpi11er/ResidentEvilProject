// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopUpWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSERIES_API UPopUpWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

private:
	/*bind item name from bp widget*/
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> MItemName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock>MPickUpText;
	
	/*bind item count from bp widget*/
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MItemCount;
    /*binding all star images in bp widget*/
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> MStarIcon1;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> MStarIcon2;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> MStarIcon3;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> MStarIcon4;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> MStarIcon5;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess))
	TObjectPtr<class AItem> MItem;
	

public:
	FORCEINLINE UTextBlock* GetItemName() const { return MItemName; }
	void SetItemName(FString& itemName) const;
	FORCEINLINE UTextBlock* GetItemCount() const { return MItemCount; }
	void SetItemCount(int32 ItemCount) const;

	void SetStarIcon() const;

	void SetItemActor(AItem* ItemActor);

	void SetPickUpText(FString&T_Text) const;
};
