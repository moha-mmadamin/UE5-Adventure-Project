// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

class UTextBlock;
class ABaseWeapon;

UCLASS()
class ADVENTURE_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindWeapon(ABaseWeapon* Weapon);

	UFUNCTION()
	void UpdateAmmo(int CurrentAmmo, int MaxAmmo);

private:
	UPROPERTY()
	ABaseWeapon* CurrentWeapon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AmmoText;
};
