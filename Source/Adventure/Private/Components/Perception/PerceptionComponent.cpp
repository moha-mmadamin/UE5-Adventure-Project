#include "Components/Perception/PerceptionComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "Components/AI/AIComponent.h"
#include "Characters/Enemy.h"

UPerceptionComponent::UPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

    OwnerEnemy = Cast<AEnemy>(GetOwner());

    if(!OwnerEnemy) return;

    AIComponent = OwnerEnemy->GetAIComponent();
    AIPerceptionComponent = OwnerEnemy->GetAIPerceptionComponent();

    if(!AIPerceptionComponent) return;

    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &UPerceptionComponent::OnTargetPerceptionUpdated
    );
}
void UPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
void UPerceptionComponent::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if(!Actor) return;

    if(Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        if(Stimulus.WasSuccessfullySensed())
        {
            HandleSight(Actor);
        }
        else
        {
            HandleLostSight(Actor);
        }
    }
    else if(Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
        HandleHearing(Stimulus.StimulusLocation);
    }
    else if(Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
    {
        HandleDamage(Actor);
    }
}
void UPerceptionComponent::HandleSight(AActor* DetectedActor)
{
    if(!DetectedActor) return;

    OnSightDetected.Broadcast(DetectedActor);
}
void UPerceptionComponent::HandleLostSight(AActor* DetectedActor)
{
    if(!DetectedActor) return;

    OnSightLost.Broadcast(DetectedActor);
}
void UPerceptionComponent::HandleHearing(const FVector& Location)
{
    OnNoiseHeard.Broadcast(Location);
}
void UPerceptionComponent::HandleDamage(AActor* DamageCauser)
{
    if(!DamageCauser) return;

    OnDamaged.Broadcast(DamageCauser);
}