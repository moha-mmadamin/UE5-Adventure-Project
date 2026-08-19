#pragma once

#include "CoreMinimal.h"
#include "CharacterAnimInstance.generated.h"

class ABaseCharacter;

UCLASS()
class ADVENTURE_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;



private:

	
};
