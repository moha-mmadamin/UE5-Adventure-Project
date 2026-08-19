#pragma once

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    EES_Idle UMETA(DisplayName = "Idle"),
    EES_Patrol UMETA(DisplayName = "Patrol"),
    EES_Investigating UMETA(DisplayName = "Investigating"),
    EES_Searching UMETA(DisplayName = "Searching"),
    EES_Chasing UMETA(DisplayName = "Chasing"),
    EES_Combat UMETA(DisplayName = "Combat"),
    EES_TakingCover UMETA(DisplayName = "TakingCover"),
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