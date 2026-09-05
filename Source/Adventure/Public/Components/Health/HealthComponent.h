// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnHealthChanged, 
	float, 
	HealthPercent
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTURE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float HealAmount);

	float GetHealthPercent() const;

    UPROPERTY(BlueprintAssignable)
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FOnDeath OnDeath;

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float NewHealth);

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetMaxHealth() const;

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Health")
    float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Health")
    float CurrentHealth = 100.0f;

public:
	//FORCEINLINE float GetCurrentHealth() const{ return CurrentHealth; };
    //FORCEINLINE float GetMaxHealth() const{ return MaxHealth; };
		
};
