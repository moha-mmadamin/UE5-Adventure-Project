#include "Characters/EchoAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/Echo.h"

void UEchoAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Echo = Cast<AEcho>(TryGetPawnOwner());
}

void UEchoAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (Echo)
	{
		WeaponState = Echo->GetWeaponState();
	}
}
