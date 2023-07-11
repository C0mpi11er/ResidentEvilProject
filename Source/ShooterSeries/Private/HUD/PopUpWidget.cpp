// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PopUpWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/Item.h"

enum class EItemRarity:uint8;


void UPopUpWidget::NativeConstruct()
{
	Super::NativeConstruct();
    /*check if inventory is full and changes pick up text*/
	if (MItem&&MItem->GetisCharInventoryFull())
	{
		FString PickUP{"Swap"};
		SetPickUpText(PickUP);
	}
}

void UPopUpWidget::SetItemName(FString& itemName) const
{
	const FText TextItemName{FText::FromString(itemName)};

	if (MItemName)
	{
		MItemName->SetText(TextItemName);
	}
}

void UPopUpWidget::SetItemCount(int32 ItemCount) const
{
	const FString StringItemCount{FString::Printf(TEXT("%d"), ItemCount)};
	const FText TextItemCount{FText::FromString(StringItemCount)};
	if (MItemCount)
	{
		MItemCount->SetText(TextItemCount);
	}
}

void UPopUpWidget::SetStarIcon() const
{
	/*set star image visibility according to item rarity*/
	if (MItem)
	{
		switch (MItem->GetItemRarity())
		{
		case EItemRarity::EIR_Damaged:MStarIcon1->SetVisibility(ESlateVisibility::Visible);
			MStarIcon2->SetVisibility(ESlateVisibility::Hidden);
			MStarIcon3->SetVisibility(ESlateVisibility::Hidden);
			MStarIcon4->SetVisibility(ESlateVisibility::Hidden);
			MStarIcon5->SetVisibility(ESlateVisibility::Hidden);break;
			
		case EItemRarity::EIR_Common:MStarIcon1->SetVisibility(ESlateVisibility::Visible);
			MStarIcon2->SetVisibility(ESlateVisibility::Visible);
			MStarIcon3->SetVisibility(ESlateVisibility::Hidden);
			MStarIcon4->SetVisibility(ESlateVisibility::Hidden);
			MStarIcon5->SetVisibility(ESlateVisibility::Hidden);break;

		case EItemRarity::EIR_UnCommon:MStarIcon1->SetVisibility(ESlateVisibility::Visible);
			MStarIcon2->SetVisibility(ESlateVisibility::Visible);
			MStarIcon3->SetVisibility(ESlateVisibility::Visible);
			MStarIcon4->SetVisibility(ESlateVisibility::Hidden);
			MStarIcon5->SetVisibility(ESlateVisibility::Hidden);break;

		case EItemRarity::EIR_Rare: MStarIcon1->SetVisibility(ESlateVisibility::Visible);
			MStarIcon2->SetVisibility(ESlateVisibility::Visible);
			MStarIcon3->SetVisibility(ESlateVisibility::Visible);
			MStarIcon4->SetVisibility(ESlateVisibility::Visible);
			MStarIcon5->SetVisibility(ESlateVisibility::Hidden);break;

		case EItemRarity::EIR_Legendary:MStarIcon1->SetVisibility(ESlateVisibility::Visible);
			MStarIcon2->SetVisibility(ESlateVisibility::Visible);
			MStarIcon3->SetVisibility(ESlateVisibility::Visible);
			MStarIcon4->SetVisibility(ESlateVisibility::Visible);
			MStarIcon5->SetVisibility(ESlateVisibility::Visible);break;

			default:break;
		}
	}
}

void UPopUpWidget::SetItemActor(AItem* ItemActor)
{
	MItem = ItemActor;
}

void UPopUpWidget::SetPickUpText(FString& T_Text) const
{
	const FText TextItemName{FText::FromString(T_Text)};

	if (MPickUpText)
	{
		MPickUpText->SetText(TextItemName);
	}
}
