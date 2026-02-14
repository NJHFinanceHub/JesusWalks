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
    ParryFlash = 10,
    // Ambient atmospheric VFX (Dark Souls-inspired)
    AmbientDustMotes = 11 UMETA(DisplayName = "Dust Motes"),
    AmbientEmbers = 12 UMETA(DisplayName = "Floating Embers"),
    AmbientGodRays = 13 UMETA(DisplayName = "God Rays"),
    AmbientMistWisps = 14 UMETA(DisplayName = "Mist Wisps"),
    AmbientFallingLeaves = 15 UMETA(DisplayName = "Falling Leaves"),
    AmbientSandParticles = 16 UMETA(DisplayName = "Sand Particles"),
    AmbientAshFall = 17 UMETA(DisplayName = "Ash Fall"),
    AmbientDawnRays = 18 UMETA(DisplayName = "Dawn Rays"),
    AmbientHolyGlow = 19 UMETA(DisplayName = "Holy Glow"),
    AmbientMoonlitHaze = 20 UMETA(DisplayName = "Moonlit Haze"),
    AmbientTorchSparks = 21 UMETA(DisplayName = "Torch Sparks"),
    AmbientCrowdDust = 22 UMETA(DisplayName = "Crowd Dust")
};

/** Atmosphere preset for Dark Souls-quality per-region lighting and post-processing. */
USTRUCT(BlueprintType)
struct FNazareneAtmospherePreset
{
    GENERATED_BODY()

    // Directional light (sun/moon)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Sun")
    FLinearColor SunColor = FLinearColor(1.0f, 0.95f, 0.82f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Sun")
    float SunIntensity = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Sun")
    float SunTemperature = 6000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Sun")
    FRotator SunRotation = FRotator(-38.0f, -32.0f, 0.0f);

    // Sky light
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Sky")
    float SkyIntensity = 0.65f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Sky")
    FLinearColor SkyTint = FLinearColor(0.42f, 0.44f, 0.52f);

    // Exponential height fog
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Fog")
    float FogDensity = 0.018f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Fog")
    float FogHeightFalloff = 0.22f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Fog")
    FLinearColor FogInscatteringColor = FLinearColor(0.28f, 0.24f, 0.18f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Fog")
    float FogMaxOpacity = 0.92f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Fog")
    float FogStartDistance = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Fog")
    bool bVolumetricFog = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Fog")
    float VolumetricFogScatteringDistribution = 0.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Fog")
    float VolumetricFogExtinctionScale = 1.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Fog")
    float SecondFogDensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Fog")
    FLinearColor SecondFogColor = FLinearColor::Black;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Fog")
    float SecondFogHeightOffset = -500.0f;

    // Post-process
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    float BlendWeight = 0.88f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    FVector4 ColorSaturation = FVector4(0.88f, 0.86f, 0.84f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    FVector4 ColorContrast = FVector4(1.12f, 1.12f, 1.10f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    FVector4 ColorGamma = FVector4(1.0f, 1.0f, 1.0f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    FVector4 ColorGain = FVector4(1.0f, 1.0f, 1.0f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    float AutoExposureBias = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    float BloomIntensity = 0.72f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    float BloomThreshold = 1.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    float VignetteIntensity = 0.42f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    float ChromaticAberrationIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    float AmbientOcclusionIntensity = 0.65f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|PostProcess")
    float AmbientOcclusionRadius = 180.0f;

    // Material tints
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Materials")
    FLinearColor GroundTint = FLinearColor(0.28f, 0.22f, 0.16f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Materials")
    FLinearColor AccentTint = FLinearColor(0.58f, 0.50f, 0.38f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Materials")
    FLinearColor WallTint = FLinearColor(0.42f, 0.36f, 0.30f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|Materials")
    float HeightJitter = 80.0f;

    // Ambient VFX types for this region
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere|VFX")
    TArray<ENazareneVFXType> AmbientVFXTypes;
};
