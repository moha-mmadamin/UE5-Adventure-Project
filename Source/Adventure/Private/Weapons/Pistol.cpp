#include "Weapons/Pistol.h"

APistol::APistol()
{
    MagazineCapacity = 15;
    CurrentAmmo = 15;
    ReserveAmmo = 45;
    FireRate = 0.5f;
}
float APistol::GetDamageForBone(FName BoneName) const
{
    if (BoneName == TEXT("head"))
    {
        return FMath::RandRange(35.f, 50.f);
    }

    if (BoneName == TEXT("spine_01") ||
        BoneName == TEXT("spine_02") ||
        BoneName == TEXT("spine_03") ||
        BoneName == TEXT("spine_04") ||
        BoneName == TEXT("spine_05"))
    {
        return FMath::RandRange(15.f, 30.f);
    }

    return 15.f;
}
