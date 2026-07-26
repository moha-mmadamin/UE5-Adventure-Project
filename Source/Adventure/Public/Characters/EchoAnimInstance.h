#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "EchoAnimInstance.generated.h"


UCLASS()
class ADVENTURE_API UEchoAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	class AEcho* Echo;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	ECharacterState CharacterState;
};
