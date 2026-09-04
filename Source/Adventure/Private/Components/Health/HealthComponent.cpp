#include "Components/Health/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
void UHealthComponent::TakeDamage(float DamageAmount)
{
    if(DamageAmount <= 0.f || CurrentHealth <= 0.f) return;
    
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(GetHealthPercent());
}
void UHealthComponent::Heal(float HealAmount)
{
    if(HealAmount <= 0.f || CurrentHealth <= 0.f) return;
    
    CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(GetHealthPercent());
}
float UHealthComponent::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}
float UHealthComponent::GetCurrentHealth() const
{
    return CurrentHealth;
}
void UHealthComponent::SetCurrentHealth(float NewHealth)
{
    CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);

    OnHealthChanged.Broadcast(GetHealthPercent());
}
float UHealthComponent::GetMaxHealth() const
{
    return MaxHealth;
}
