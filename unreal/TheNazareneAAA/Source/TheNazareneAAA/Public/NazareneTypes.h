#pragma once

#include "CoreMinimal.h"
#include "NazareneTypes.generated.h"

class USoundBase;

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

UENUM(BlueprintType)
enum class ENazareneDamageNumberType : uint8
{
    Normal = 0,
    Critical = 1,
    Heal = 2,
    PoiseBreak = 3,
    Blocked = 4
};

UENUM(BlueprintType)
enum class ENazareneItemType : uint8
{
    Quest = 0 UMETA(DisplayName = "Quest"),
    Consumable = 1 UMETA(DisplayName = "Consumable"),
    Relic = 2 UMETA(DisplayName = "Relic"),
    Key = 3 UMETA(DisplayName = "Key")
};

UENUM(BlueprintType)
enum class ENazareneItemRarity : uint8
{
    Common = 0 UMETA(DisplayName = "Common"),
    Rare = 1 UMETA(DisplayName = "Rare"),
    Epic = 2 UMETA(DisplayName = "Epic"),
    Legendary = 3 UMETA(DisplayName = "Legendary")
};

USTRUCT(BlueprintType)
struct FNazareneInventoryItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ItemName = TEXT("Unnamed Item");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ENazareneItemType ItemType = ENazareneItemType::Quest;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ENazareneItemRarity Rarity = ENazareneItemRarity::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity = 1;
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
    int32 TotalXP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PlayerLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SkillPoints = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> UnlockedSkills;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FNazareneInventoryItem> Inventory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> RegionRetryCounts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> Flags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<uint8> ChapterStagePerRegion;
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
struct FNazareneNPCDialogueLine
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DisplayDuration = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<USoundBase> VoiceClip;
};

USTRUCT(BlueprintType)
struct FNazareneNPCSpawnDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString NPCName = TEXT("Villager");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CharacterSlug;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Location = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString IdleGreeting;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FNazareneNPCDialogueLine> DialogueLines;
};

UENUM(BlueprintType)
enum class ENazareneSpawnTrigger : uint8
{
    OnRegionLoad = 0,
    OnPrayerSiteConsecrated = 1,
    OnBossPhaseChange = 2,
    OnAreaEnter = 3
};

USTRUCT(BlueprintType)
struct FNazareneEncounterWave
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ENazareneSpawnTrigger Trigger = ENazareneSpawnTrigger::OnRegionLoad;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TriggerParam = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FNazareneEnemySpawnDefinition> Enemies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DelaySeconds = 0.0f;
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
    FString ChapterTitle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName StreamedLevelPackage = NAME_None;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString LoreText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FNazareneNPCSpawnDefinition> NPCs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FNazareneEncounterWave> EncounterWaves;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FNazareneInventoryItem> RewardItems;
};

UENUM(BlueprintType)
enum class ENazareneVFXType : uint8
{
    HealBurst = 0,
    BlessingAura = 1,
    RadiancePulse = 2,
    LightSlash = 3,
    HeavySlash = 4,
    EnemyHitReact = 5,
    EnemyRedeemed = 6,
    BossArenaHazard = 7,
    BossPhaseTransition = 8,
    DodgeTrail = 9,
    ParryFlash = 10
};
