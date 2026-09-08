#include "Components/Health/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

    if(bHasArmor)
    {
        CurrentArmor = MaxArmor;
    }
    else
    {
        CurrentArmor = 0.0f;
    }

    TimeSinceLastDamage = ArmorRegenDelay;
}
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if(!bHasArmor || CurrentArmor >= MaxArmor) return;

    TimeSinceLastDamage += DeltaTime;

    if (TimeSinceLastDamage >= ArmorRegenDelay)
    {
        RegenerateArmor(DeltaTime);
    }

}
void UHealthComponent::RegenerateArmor(float DeltaTime)
{
    if(ArmorRegenRate <= 0.f || CurrentArmor >= MaxArmor) return;
    const float OldArmor = CurrentArmor;
    CurrentArmor = FMath::Clamp(CurrentArmor + ArmorRegenRate * DeltaTime, 0.0f, MaxArmor);

    if (!FMath::IsNearlyEqual(OldArmor, CurrentArmor))
    {
        OnArmorChanged.Broadcast(GetArmorPercent());
    }
}
void UHealthComponent::TakeDamage(float DamageAmount)
{
    if(DamageAmount <= 0.f || CurrentHealth <= 0.f) return;

    TimeSinceLastDamage = 0.0f;

    float RemainingDamage = DamageAmount;

    if(bHasArmor && CurrentArmor > 0.f)
    {
        const float ArmorDamage = FMath::Min(CurrentArmor, RemainingDamage);

        CurrentArmor -= ArmorDamage;
        RemainingDamage -= ArmorDamage;

        OnArmorChanged.Broadcast(GetArmorPercent());
    }

    if (RemainingDamage > 0.f)
    {
        CurrentHealth = FMath::Clamp(CurrentHealth - RemainingDamage, 0.0f, MaxHealth);

        OnHealthChanged.Broadcast(GetHealthPercent());
    }

    if (CurrentHealth <= 0.f)
    {
        OnDeath.Broadcast();
    }
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
float UHealthComponent::GetArmorPercent() const
{
    return CurrentArmor / MaxArmor;
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
float UHealthComponent::GetCurrentArmor() const
{
    return CurrentArmor;
}
float UHealthComponent::GetMaxArmor() const
{
    return MaxArmor;
}