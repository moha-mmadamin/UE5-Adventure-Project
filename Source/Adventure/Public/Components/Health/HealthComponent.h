#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnHealthChanged,
    float,
    HealthPercent
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnArmorChanged,
    float,
    ArmorPercent
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
	float GetArmorPercent() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetCurrentHealth() const;

    UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const;

    UFUNCTION(BlueprintPure, Category = "Armor")
    float GetCurrentArmor() const;

    UFUNCTION(BlueprintPure, Category = "Armor")
    float GetMaxArmor() const;
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float NewHealth);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
    bool bHasArmor = false;

    UPROPERTY(BlueprintAssignable)
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FOnArmorChanged OnArmorChanged;

    UPROPERTY(BlueprintAssignable)
    FOnDeath OnDeath;

protected:
	virtual void BeginPlay() override;

private:
    void RegenerateArmor(float DeltaTime);
    float TimeSinceLastDamage = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Health")
    float MaxHealth = 200.0f;

	UPROPERTY(VisibleAnywhere, Category = "Health")
    float CurrentHealth = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Armor")
    float MaxArmor = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Armor")
    float CurrentArmor = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Armor")
    float ArmorRegenDelay = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Armor")
    float ArmorRegenRate = 5.0f;
};
