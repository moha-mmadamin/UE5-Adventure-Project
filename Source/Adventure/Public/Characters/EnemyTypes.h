#pragma once

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    EES_Idle UMETA(DisplayName = "Idle"),
    EES_Patrol UMETA(DisplayName = "Patrol"),
    EES_Investigate UMETA(DisplayName = "Investigate"),
    EES_Chase UMETA(DisplayName = "Chase"),
    EES_Attack UMETA(DisplayName = "Attack"),
    EES_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EEnemyDetectionType : uint8
{
    EDT_None UMETA(DisplayName = "None"),
    EDT_Sight UMETA(DisplayName = "Sight"),
    EDT_Hearing UMETA(DisplayName = "Hearing"),
    EDT_Damage UMETA(DisplayName = "Damage")
};