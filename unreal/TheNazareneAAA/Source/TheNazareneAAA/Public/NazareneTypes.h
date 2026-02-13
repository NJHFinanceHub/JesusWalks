#pragma once

#include "CoreMinimal.h"
#include "NazareneTypes.generated.h"

UENUM(BlueprintType)
enum class ENazareneEnemyArchetype : uint8
{
    MeleeShield = 0 UMETA(DisplayName = "Melee Shield"),
    Spear = 1 UMETA(DisplayName = "Spear"),
    Ranged = 2 UMETA(DisplayName = "Ranged"),
    Demon = 3 UMETA(DisplayName = "Demon"),
    Boss = 4 UMETA(DisplayName = "Boss")
};

UENUM(BlueprintType)
enum class ENazareneEnemyState : uint8
{
    Idle = 0,
    Chase = 1,
    Retreat = 2,
    Strafe = 3,
    Windup = 4,
    Casting = 5,
    Blocking = 6,
    Recover = 7,
    Staggered = 8,
    Parried = 9,
    Redeemed = 10
};

USTRUCT(BlueprintType)
struct FNazarenePlayerSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Position = FVector(0.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Health = 120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Stamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Faith = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName LastRestSiteId = NAME_None;
};

USTRUCT(BlueprintType)
struct FNazareneEnemySnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SpawnId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString EnemyName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ENazareneEnemyArchetype Archetype = ENazareneEnemyArchetype::MeleeShield;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Position = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Health = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Poise = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRedeemed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BossPhase = 1;
};

USTRUCT(BlueprintType)
struct FNazareneCampaignState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RegionIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> UnlockedMiracles = { FName(TEXT("heal")) };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHealthBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxStaminaBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> RegionRetryCounts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> Flags;
};

USTRUCT(BlueprintType)
struct FNazareneSavePayload
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FNazarenePlayerSnapshot Player;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FNazareneEnemySnapshot> Enemies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FNazareneCampaignState Campaign;
};

USTRUCT(BlueprintType)
struct FNazareneEnemySpawnDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SpawnId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString EnemyName = TEXT("Enemy");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ENazareneEnemyArchetype Archetype = ENazareneEnemyArchetype::MeleeShield;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Location = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FNazareneRegionDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RegionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RegionName = TEXT("Galilee Shores");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Chapter = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Objective = TEXT("Redeem the guardian.");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector PlayerSpawn = FVector(0.0f, 0.0f, 200.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName PrayerSiteId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString PrayerSiteName = TEXT("Prayer Site");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector PrayerSiteLocation = FVector(0.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TravelGatePrompt = TEXT("Press E to travel onward");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector TravelGateLocation = FVector(0.0f, 0.0f, 3000.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FNazareneEnemySpawnDefinition> Enemies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BossSpawnId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RewardMiracle = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RewardHealthBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RewardStaminaBonus = 0.0f;
};

