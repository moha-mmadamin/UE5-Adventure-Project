// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

class UTextBlock;
class AWeapon;

UCLASS()
class ADVENTURE_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindWeapon(AWeapon* Weapon);

	UFUNCTION()
	void UpdateAmmo(int CurrentAmmo, int MaxAmmo);

private:
	UPROPERTY()
	AWeapon* CurrentWeapon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AmmoText;
};
