#include "Components/HealthComponent.h"

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
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(GetHealthPercent());
}
void UHealthComponent::Heal(float HealAmount)
{
    CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(GetHealthPercent());
}

float UHealthComponent::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}
