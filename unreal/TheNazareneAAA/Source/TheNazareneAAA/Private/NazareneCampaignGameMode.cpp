#include "NazareneCampaignGameMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NazareneEnemyCharacter.h"
#include "NazareneGameInstance.h"
#include "NazarenePlayerCharacter.h"
#include "NazarenePrayerSite.h"
#include "NazareneSaveSubsystem.h"
#include "NazareneTravelGate.h"

ANazareneCampaignGameMode::ANazareneCampaignGameMode()
{
    PlayerCharacterClass = ANazarenePlayerCharacter::StaticClass();
    EnemyClass = ANazareneEnemyCharacter::StaticClass();
    PrayerSiteClass = ANazarenePrayerSite::StaticClass();
    TravelGateClass = ANazareneTravelGate::StaticClass();
    DefaultPawnClass = nullptr;
}

void ANazareneCampaignGameMode::BeginPlay()
{
    Super::BeginPlay();

    NazareneGameInstance = Cast<UNazareneGameInstance>(GetGameInstance());
    InitializeDefaultRegions();

    FNazareneSavePayload PendingPayload;
    if (NazareneGameInstance != nullptr && NazareneGameInstance->ConsumePendingPayload(PendingPayload))
    {
        CurrentRegionIndex = FMath::Clamp(PendingPayload.Campaign.RegionIndex, 0, Regions.Num() - 1);
        LoadRegion(CurrentRegionIndex);
        ApplyLoadedPayload(PendingPayload);
    }
    else
    {
        if (NazareneGameInstance != nullptr)
        {
            CurrentRegionIndex = FMath::Clamp(NazareneGameInstance->GetCampaignState().RegionIndex, 0, Regions.Num() - 1);
        }
        LoadRegion(CurrentRegionIndex);
    }
}

void ANazareneCampaignGameMode::RequestTravel(int32 TargetRegionIndex)
{
    if (!bRegionCompleted)
    {
        if (PlayerCharacter != nullptr)
        {
            PlayerCharacter->SetContextHint(TEXT("Redeem the region's guardian to proceed."));
        }
        return;
    }

    if (TargetRegionIndex >= Regions.Num())
    {
        SaveCheckpoint();
        if (PlayerCharacter != nullptr)
        {
            PlayerCharacter->SetContextHint(TEXT("Pilgrimage complete."));
        }
        return;
    }

    LoadRegion(TargetRegionIndex);
    SaveCheckpoint();
}

int32 ANazareneCampaignGameMode::GetCurrentRegionIndex() const
{
    return CurrentRegionIndex;
}

FNazareneSavePayload ANazareneCampaignGameMode::BuildSavePayload() const
{
    FNazareneSavePayload Payload;
    if (PlayerCharacter != nullptr)
    {
        Payload.Player = PlayerCharacter->BuildSnapshot();
    }

    for (TActorIterator<ANazareneEnemyCharacter> It(GetWorld()); It; ++It)
    {
        Payload.Enemies.Add(It->BuildSnapshot());
    }

    if (NazareneGameInstance != nullptr)
    {
        Payload.Campaign = NazareneGameInstance->GetCampaignState();
        Payload.Campaign.RegionIndex = CurrentRegionIndex;
    }
    else
    {
        Payload.Campaign.RegionIndex = CurrentRegionIndex;
    }

    return Payload;
}

bool ANazareneCampaignGameMode::ApplyLoadedPayload(const FNazareneSavePayload& Payload)
{
    if (NazareneGameInstance != nullptr)
    {
        FNazareneCampaignState& MutableCampaign = NazareneGameInstance->GetMutableCampaignState();
        MutableCampaign = Payload.Campaign;
    }

    const int32 TargetRegion = FMath::Clamp(Payload.Campaign.RegionIndex, 0, Regions.Num() - 1);
    if (TargetRegion != CurrentRegionIndex)
    {
        LoadRegion(TargetRegion);
    }

    if (PlayerCharacter != nullptr)
    {
        PlayerCharacter->ApplySnapshot(Payload.Player);
    }

    TMap<FName, FNazareneEnemySnapshot> EnemyMap;
    for (const FNazareneEnemySnapshot& Snapshot : Payload.Enemies)
    {
        EnemyMap.Add(Snapshot.SpawnId, Snapshot);
    }

    bSuppressRedeemEvents = true;
    for (TActorIterator<ANazareneEnemyCharacter> It(GetWorld()); It; ++It)
    {
        ANazareneEnemyCharacter* Enemy = *It;
        if (EnemyMap.Contains(Enemy->SpawnId))
        {
            Enemy->ApplySnapshot(EnemyMap[Enemy->SpawnId]);
        }
        else
        {
            Enemy->ResetToSpawn();
        }
    }
    bSuppressRedeemEvents = false;

    bRegionCompleted = (BossEnemy != nullptr && BossEnemy->IsRedeemed());
    if (TravelGate != nullptr)
    {
        TravelGate->SetGateEnabled(bRegionCompleted);
    }
    return true;
}

void ANazareneCampaignGameMode::InitializeDefaultRegions()
{
    if (Regions.Num() > 0)
    {
        return;
    }

    auto M = [](double X, double Y, double Z) -> FVector
    {
        return FVector(float(X * 100.0), float(Y * 100.0), float(Z * 100.0));
    };

    FNazareneRegionDefinition Galilee;
    Galilee.RegionId = FName(TEXT("galilee"));
    Galilee.RegionName = TEXT("Galilee Shores");
    Galilee.Chapter = 1;
    Galilee.Objective = TEXT("Redeem the Legion Sovereign of Gerasa.");
    Galilee.PlayerSpawn = M(0.0, 0.0, 1.8);
    Galilee.PrayerSiteId = FName(TEXT("galilee_site_01"));
    Galilee.PrayerSiteName = TEXT("Prayer Site: Galilee Shores");
    Galilee.PrayerSiteLocation = M(0.0, 0.0, 0.2);
    Galilee.TravelGatePrompt = TEXT("Press E to travel to Decapolis Ruins");
    Galilee.TravelGateLocation = M(0.0, 32.0, 0.2);
    Galilee.BossSpawnId = FName(TEXT("galilee_named_boss_01"));
    Galilee.RewardMiracle = FName(TEXT("blessing"));
    Galilee.RewardHealthBonus = 10.0f;
    Galilee.RewardStaminaBonus = 6.0f;
    Galilee.Enemies = {
        {FName(TEXT("galilee_shield_01")), TEXT("Roman Shieldbearer I"), ENazareneEnemyArchetype::MeleeShield, M(9.0, -3.0, 1.0)},
        {FName(TEXT("galilee_shield_02")), TEXT("Roman Shieldbearer II"), ENazareneEnemyArchetype::MeleeShield, M(-9.0, -5.0, 1.0)},
        {FName(TEXT("galilee_spear_01")), TEXT("Roman Spearman I"), ENazareneEnemyArchetype::Spear, M(15.0, 11.0, 1.0)},
        {FName(TEXT("galilee_spear_02")), TEXT("Roman Spearman II"), ENazareneEnemyArchetype::Spear, M(-14.0, 12.0, 1.0)},
        {FName(TEXT("galilee_ranged_01")), TEXT("Roman Slinger I"), ENazareneEnemyArchetype::Ranged, M(18.0, -14.0, 1.0)},
        {FName(TEXT("galilee_ranged_02")), TEXT("Roman Slinger II"), ENazareneEnemyArchetype::Ranged, M(-18.0, -15.0, 1.0)},
        {FName(TEXT("galilee_demon_01")), TEXT("Unclean Spirit I"), ENazareneEnemyArchetype::Demon, M(5.0, -20.0, 1.0)},
        {FName(TEXT("galilee_demon_02")), TEXT("Unclean Spirit II"), ENazareneEnemyArchetype::Demon, M(-6.0, -22.0, 1.0)},
        {FName(TEXT("galilee_named_boss_01")), TEXT("Legion Sovereign of Gerasa"), ENazareneEnemyArchetype::Boss, M(0.0, -34.0, 1.0)}
    };
    Regions.Add(Galilee);

    FNazareneRegionDefinition Decapolis;
    Decapolis.RegionId = FName(TEXT("decapolis"));
    Decapolis.RegionName = TEXT("Decapolis Ruins");
    Decapolis.Chapter = 2;
    Decapolis.Objective = TEXT("Redeem the Gadarene Warlord among the ruins.");
    Decapolis.PlayerSpawn = M(0.0, 0.0, 1.8);
    Decapolis.PrayerSiteId = FName(TEXT("decapolis_site_01"));
    Decapolis.PrayerSiteName = TEXT("Prayer Site: Decapolis Ruins");
    Decapolis.PrayerSiteLocation = M(0.0, 0.0, 0.2);
    Decapolis.TravelGatePrompt = TEXT("Press E to travel to Wilderness of Temptation");
    Decapolis.TravelGateLocation = M(0.0, 34.0, 0.2);
    Decapolis.BossSpawnId = FName(TEXT("decapolis_named_boss_01"));
    Decapolis.RewardMiracle = FName(TEXT("radiance"));
    Decapolis.RewardHealthBonus = 10.0f;
    Decapolis.RewardStaminaBonus = 6.0f;
    Decapolis.Enemies = {
        {FName(TEXT("decapolis_shield_01")), TEXT("Roman Shieldbearer"), ENazareneEnemyArchetype::MeleeShield, M(8.0, -4.0, 1.0)},
        {FName(TEXT("decapolis_shield_02")), TEXT("Roman Shieldbearer"), ENazareneEnemyArchetype::MeleeShield, M(-9.0, -6.0, 1.0)},
        {FName(TEXT("decapolis_spear_01")), TEXT("Roman Spearman"), ENazareneEnemyArchetype::Spear, M(12.0, 9.0, 1.0)},
        {FName(TEXT("decapolis_spear_02")), TEXT("Roman Spearman"), ENazareneEnemyArchetype::Spear, M(-12.0, 10.0, 1.0)},
        {FName(TEXT("decapolis_ranged_01")), TEXT("Roman Slinger"), ENazareneEnemyArchetype::Ranged, M(16.0, -10.0, 1.0)},
        {FName(TEXT("decapolis_ranged_02")), TEXT("Roman Slinger"), ENazareneEnemyArchetype::Ranged, M(-16.0, -12.0, 1.0)},
        {FName(TEXT("decapolis_demon_01")), TEXT("Unclean Spirit"), ENazareneEnemyArchetype::Demon, M(6.0, -18.0, 1.0)},
        {FName(TEXT("decapolis_demon_02")), TEXT("Unclean Spirit"), ENazareneEnemyArchetype::Demon, M(-5.0, -20.0, 1.0)},
        {FName(TEXT("decapolis_demon_03")), TEXT("Unclean Spirit"), ENazareneEnemyArchetype::Demon, M(3.0, -24.0, 1.0)},
        {FName(TEXT("decapolis_named_boss_01")), TEXT("Gadarene Warlord"), ENazareneEnemyArchetype::Boss, M(0.0, -30.0, 1.0)}
    };
    Regions.Add(Decapolis);

    FNazareneRegionDefinition Wilderness;
    Wilderness.RegionId = FName(TEXT("wilderness"));
    Wilderness.RegionName = TEXT("Wilderness of Temptation");
    Wilderness.Chapter = 3;
    Wilderness.Objective = TEXT("Resist the Adversary of the Desert.");
    Wilderness.PlayerSpawn = M(0.0, 0.0, 1.8);
    Wilderness.PrayerSiteId = FName(TEXT("wilderness_site_01"));
    Wilderness.PrayerSiteName = TEXT("Prayer Site: Wilderness Ridge");
    Wilderness.PrayerSiteLocation = M(0.0, 0.0, 0.2);
    Wilderness.TravelGatePrompt = TEXT("Press E to travel to Jerusalem Approach");
    Wilderness.TravelGateLocation = M(0.0, 36.0, 0.2);
    Wilderness.BossSpawnId = FName(TEXT("wilderness_named_boss_01"));
    Wilderness.RewardHealthBonus = 12.0f;
    Wilderness.RewardStaminaBonus = 8.0f;
    Wilderness.Enemies = {
        {FName(TEXT("wilderness_ranged_01")), TEXT("Desert Slinger"), ENazareneEnemyArchetype::Ranged, M(18.0, -10.0, 1.0)},
        {FName(TEXT("wilderness_ranged_02")), TEXT("Desert Slinger"), ENazareneEnemyArchetype::Ranged, M(-18.0, -12.0, 1.0)},
        {FName(TEXT("wilderness_spear_01")), TEXT("Desert Spearman"), ENazareneEnemyArchetype::Spear, M(12.0, 10.0, 1.0)},
        {FName(TEXT("wilderness_spear_02")), TEXT("Desert Spearman"), ENazareneEnemyArchetype::Spear, M(-12.0, 12.0, 1.0)},
        {FName(TEXT("wilderness_demon_01")), TEXT("Tempter Spirit"), ENazareneEnemyArchetype::Demon, M(6.0, -18.0, 1.0)},
        {FName(TEXT("wilderness_demon_02")), TEXT("Tempter Spirit"), ENazareneEnemyArchetype::Demon, M(-6.0, -19.0, 1.0)},
        {FName(TEXT("wilderness_demon_03")), TEXT("Tempter Spirit"), ENazareneEnemyArchetype::Demon, M(4.0, -26.0, 1.0)},
        {FName(TEXT("wilderness_demon_04")), TEXT("Tempter Spirit"), ENazareneEnemyArchetype::Demon, M(-4.0, -27.0, 1.0)},
        {FName(TEXT("wilderness_named_boss_01")), TEXT("Adversary of the Desert"), ENazareneEnemyArchetype::Boss, M(0.0, -32.0, 1.0)}
    };
    Regions.Add(Wilderness);

    FNazareneRegionDefinition Jerusalem;
    Jerusalem.RegionId = FName(TEXT("jerusalem"));
    Jerusalem.RegionName = TEXT("Jerusalem Approach");
    Jerusalem.Chapter = 4;
    Jerusalem.Objective = TEXT("Face the Temple Warden at the city's gate.");
    Jerusalem.PlayerSpawn = M(0.0, 0.0, 1.8);
    Jerusalem.PrayerSiteId = FName(TEXT("jerusalem_site_01"));
    Jerusalem.PrayerSiteName = TEXT("Prayer Site: Jerusalem Approach");
    Jerusalem.PrayerSiteLocation = M(0.0, 0.0, 0.2);
    Jerusalem.TravelGatePrompt = TEXT("Press E to conclude the pilgrimage");
    Jerusalem.TravelGateLocation = M(0.0, 32.0, 0.2);
    Jerusalem.BossSpawnId = FName(TEXT("jerusalem_named_boss_01"));
    Jerusalem.Enemies = {
        {FName(TEXT("jerusalem_shield_01")), TEXT("Temple Shieldbearer"), ENazareneEnemyArchetype::MeleeShield, M(8.0, -4.0, 1.0)},
        {FName(TEXT("jerusalem_shield_02")), TEXT("Temple Shieldbearer"), ENazareneEnemyArchetype::MeleeShield, M(-8.0, -6.0, 1.0)},
        {FName(TEXT("jerusalem_spear_01")), TEXT("Temple Spearman"), ENazareneEnemyArchetype::Spear, M(12.0, 8.0, 1.0)},
        {FName(TEXT("jerusalem_spear_02")), TEXT("Temple Spearman"), ENazareneEnemyArchetype::Spear, M(-12.0, 10.0, 1.0)},
        {FName(TEXT("jerusalem_ranged_01")), TEXT("Temple Slinger"), ENazareneEnemyArchetype::Ranged, M(16.0, -12.0, 1.0)},
        {FName(TEXT("jerusalem_ranged_02")), TEXT("Temple Slinger"), ENazareneEnemyArchetype::Ranged, M(-16.0, -14.0, 1.0)},
        {FName(TEXT("jerusalem_demon_01")), TEXT("Unclean Spirit"), ENazareneEnemyArchetype::Demon, M(5.0, -20.0, 1.0)},
        {FName(TEXT("jerusalem_demon_02")), TEXT("Unclean Spirit"), ENazareneEnemyArchetype::Demon, M(-5.0, -21.0, 1.0)},
        {FName(TEXT("jerusalem_named_boss_01")), TEXT("Temple Warden"), ENazareneEnemyArchetype::Boss, M(0.0, -28.0, 1.0)}
    };
    Regions.Add(Jerusalem);
}

void ANazareneCampaignGameMode::LoadRegion(int32 RegionIndex)
{
    if (Regions.Num() == 0)
    {
        return;
    }

    ClearRegionActors();
    CurrentRegionIndex = FMath::Clamp(RegionIndex, 0, Regions.Num() - 1);
    bRegionCompleted = false;
    BossEnemy = nullptr;
    TravelGate = nullptr;

    if (NazareneGameInstance != nullptr)
    {
        FNazareneCampaignState& Campaign = NazareneGameInstance->GetMutableCampaignState();
        Campaign.RegionIndex = CurrentRegionIndex;
    }

    SpawnRegionActors(Regions[CurrentRegionIndex]);
    SaveCheckpoint();
}

void ANazareneCampaignGameMode::ClearRegionActors()
{
    for (AActor* Actor : SpawnedRegionActors)
    {
        if (Actor != nullptr && IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    SpawnedRegionActors.Empty();
    PlayerCharacter = nullptr;
    BossEnemy = nullptr;
    TravelGate = nullptr;
}

void ANazareneCampaignGameMode::SpawnRegionActors(const FNazareneRegionDefinition& Region)
{
    UWorld* World = GetWorld();
    if (World == nullptr)
    {
        return;
    }

    if (PlayerCharacterClass != nullptr)
    {
        FActorSpawnParameters Params;
        PlayerCharacter = World->SpawnActor<ANazarenePlayerCharacter>(PlayerCharacterClass, Region.PlayerSpawn, FRotator::ZeroRotator, Params);
        if (PlayerCharacter != nullptr)
        {
            SpawnedRegionActors.Add(PlayerCharacter);
            PlayerCharacter->SetCampaignGameMode(this);
        }
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (PlayerCharacter != nullptr)
        {
            PC->Possess(PlayerCharacter);
            PC->SetViewTarget(PlayerCharacter);
            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false;
        }
    }

    if (PrayerSiteClass != nullptr)
    {
        ANazarenePrayerSite* PrayerSite = World->SpawnActor<ANazarenePrayerSite>(PrayerSiteClass, Region.PrayerSiteLocation, FRotator::ZeroRotator);
        if (PrayerSite != nullptr)
        {
            PrayerSite->SiteId = Region.PrayerSiteId;
            PrayerSite->SiteName = Region.PrayerSiteName;
            SpawnedRegionActors.Add(PrayerSite);
        }
    }

    if (EnemyClass != nullptr)
    {
        for (const FNazareneEnemySpawnDefinition& Def : Region.Enemies)
        {
            ANazareneEnemyCharacter* Enemy = World->SpawnActor<ANazareneEnemyCharacter>(EnemyClass, Def.Location, FRotator::ZeroRotator);
            if (Enemy == nullptr)
            {
                continue;
            }

            Enemy->SpawnId = Def.SpawnId;
            Enemy->EnemyName = Def.EnemyName;
            Enemy->Archetype = Def.Archetype;
            Enemy->ConfigureFromArchetype();
            Enemy->OnRedeemed.AddDynamic(this, &ANazareneCampaignGameMode::HandleEnemyRedeemed);

            if (Def.SpawnId == Region.BossSpawnId)
            {
                BossEnemy = Enemy;
            }

            SpawnedRegionActors.Add(Enemy);
        }
    }

    if (TravelGateClass != nullptr)
    {
        TravelGate = World->SpawnActor<ANazareneTravelGate>(TravelGateClass, Region.TravelGateLocation, FRotator::ZeroRotator);
        if (TravelGate != nullptr)
        {
            TravelGate->PromptText = Region.TravelGatePrompt;
            TravelGate->TargetRegionIndex = CurrentRegionIndex + 1;
            TravelGate->SetGateEnabled(false);
            SpawnedRegionActors.Add(TravelGate);
        }
    }

    UpdatePlayerProgressionStats();
}

void ANazareneCampaignGameMode::SaveCheckpoint()
{
    UNazareneSaveSubsystem* SaveSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UNazareneSaveSubsystem>() : nullptr;
    if (SaveSubsystem == nullptr)
    {
        return;
    }
    SaveSubsystem->SaveCheckpoint(BuildSavePayload());
}

void ANazareneCampaignGameMode::ApplyRegionReward(const FNazareneRegionDefinition& Region)
{
    if (NazareneGameInstance == nullptr)
    {
        return;
    }

    const FName FlagId(*FString::Printf(TEXT("boss_%s"), *Region.RegionId.ToString()));
    if (NazareneGameInstance->IsFlagSet(FlagId))
    {
        return;
    }
    NazareneGameInstance->MarkFlag(FlagId);

    FNazareneCampaignState& Campaign = NazareneGameInstance->GetMutableCampaignState();
    Campaign.MaxHealthBonus += Region.RewardHealthBonus;
    Campaign.MaxStaminaBonus += Region.RewardStaminaBonus;
    if (!Region.RewardMiracle.IsNone())
    {
        NazareneGameInstance->EnsureMiracleUnlocked(Region.RewardMiracle);
    }

    UpdatePlayerProgressionStats();
}

void ANazareneCampaignGameMode::UpdatePlayerProgressionStats()
{
    if (PlayerCharacter == nullptr || NazareneGameInstance == nullptr)
    {
        return;
    }

    const FNazareneCampaignState& Campaign = NazareneGameInstance->GetCampaignState();
    PlayerCharacter->MaxHealth = 120.0f + Campaign.MaxHealthBonus;
    PlayerCharacter->MaxStamina = 100.0f + Campaign.MaxStaminaBonus;
    PlayerCharacter->SetUnlockedMiracles(Campaign.UnlockedMiracles);
    FNazarenePlayerSnapshot Snapshot = PlayerCharacter->BuildSnapshot();
    Snapshot.Health = PlayerCharacter->MaxHealth;
    Snapshot.Stamina = PlayerCharacter->MaxStamina;
    PlayerCharacter->ApplySnapshot(Snapshot);
}

void ANazareneCampaignGameMode::HandleEnemyRedeemed(ANazareneEnemyCharacter* Enemy, float FaithReward)
{
    if (bSuppressRedeemEvents || Enemy == nullptr || Enemy != BossEnemy || bRegionCompleted)
    {
        return;
    }

    bRegionCompleted = true;
    if (TravelGate != nullptr)
    {
        TravelGate->SetGateEnabled(true);
    }

    if (Regions.IsValidIndex(CurrentRegionIndex))
    {
        ApplyRegionReward(Regions[CurrentRegionIndex]);
    }

    SaveCheckpoint();
}
