#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "Pistol.generated.h"

UCLASS()
class ADVENTURE_API APistol : public ABaseWeapon
{
	GENERATED_BODY()

public:
	APistol();

protected:
	virtual float GetDamageForBone(FName BoneName) const override;
};
