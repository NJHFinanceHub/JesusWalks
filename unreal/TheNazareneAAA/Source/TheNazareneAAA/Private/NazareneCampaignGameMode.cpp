#include "NazareneCampaignGameMode.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NazareneEnemyCharacter.h"
#include "NazareneGameInstance.h"
#include "NazareneHUD.h"
#include "NazarenePlayerCharacter.h"
#include "NazarenePrayerSite.h"
#include "NazareneSaveSubsystem.h"
#include "NazareneTravelGate.h"

ANazareneCampaignGameMode::ANazareneCampaignGameMode()
{
    DefaultPawnClass = ANazarenePlayerCharacter::StaticClass();
    HUDClass = ANazareneHUD::StaticClass();
    PlayerControllerClass = APlayerController::StaticClass();
}

void ANazareneCampaignGameMode::BeginPlay()
{
    Super::BeginPlay();

    Session = Cast<UNazareneGameInstance>(GetGameInstance());
    SaveSubsystem = Session ? Session->GetSubsystem<UNazareneSaveSubsystem>() : nullptr;
    BuildDefaultRegions();

    FNazareneSavePayload PendingPayload;
    bool bHasPendingPayload = Session ? Session->ConsumePendingPayload(PendingPayload) : false;

    if (bHasPendingPayload)
    {
        if (Session)
        {
            Session->GetMutableCampaignState() = PendingPayload.Campaign;
        }
        RegionIndex = FMath::Clamp(PendingPayload.Campaign.RegionIndex, 0, Regions.Num() - 1);
    }
    else if (Session)
    {
        RegionIndex = FMath::Clamp(Session->GetCampaignState().RegionIndex, 0, Regions.Num() - 1);
    }

    LoadRegion(RegionIndex);

    if (bHasPendingPayload)
    {
        ApplySavePayload(PendingPayload);
    }

    SaveCheckpoint();
}

void ANazareneCampaignGameMode::BuildDefaultRegions()
{
    if (Regions.Num() > 0)
    {
        return;
    }

    FNazareneRegionDefinition Galilee;
    Galilee.RegionId = FName(TEXT("galilee"));
    Galilee.RegionName = TEXT("Galilee Shores");
    Galilee.Chapter = 1;
    Galilee.Objective = TEXT("Redeem the Legion Sovereign of Gerasa.");
    Galilee.PlayerSpawn = FVector(0.0f, 0.0f, 220.0f);
    Galilee.PrayerSiteId = FName(TEXT("galilee_site_01"));
    Galilee.PrayerSiteName = TEXT("Prayer Site: Galilee Shores");
    Galilee.PrayerSiteLocation = FVector(0.0f, 0.0f, 20.0f);
    Galilee.TravelGatePrompt = TEXT("Press E to travel to Decapolis Ruins");
    Galilee.TravelGateLocation = FVector(0.0f, 1800.0f, 20.0f);
    Galilee.BossSpawnId = FName(TEXT("galilee_named_boss_01"));
    Galilee.RewardMiracle = FName(TEXT("blessing"));
    Galilee.RewardHealthBonus = 10.0f;
    Galilee.RewardStaminaBonus = 6.0f;
    Galilee.Enemies =
    {
        { FName(TEXT("galilee_shield_01")), TEXT("Roman Shieldbearer I"), ENazareneEnemyArchetype::MeleeShield, FVector(850.0f, 420.0f, 100.0f) },
        { FName(TEXT("galilee_shield_02")), TEXT("Roman Shieldbearer II"), ENazareneEnemyArchetype::MeleeShield, FVector(-900.0f, 450.0f, 100.0f) },
        { FName(TEXT("galilee_spear_01")), TEXT("Roman Spearman I"), ENazareneEnemyArchetype::Spear, FVector(1200.0f, -250.0f, 100.0f) },
        { FName(TEXT("galilee_spear_02")), TEXT("Roman Spearman II"), ENazareneEnemyArchetype::Spear, FVector(-1200.0f, -300.0f, 100.0f) },
        { FName(TEXT("galilee_ranged_01")), TEXT("Roman Slinger I"), ENazareneEnemyArchetype::Ranged, FVector(1300.0f, 950.0f, 100.0f) },
        { FName(TEXT("galilee_ranged_02")), TEXT("Roman Slinger II"), ENazareneEnemyArchetype::Ranged, FVector(-1300.0f, 980.0f, 100.0f) },
        { FName(TEXT("galilee_demon_01")), TEXT("Unclean Spirit I"), ENazareneEnemyArchetype::Demon, FVector(460.0f, -1100.0f, 100.0f) },
        { FName(TEXT("galilee_demon_02")), TEXT("Unclean Spirit II"), ENazareneEnemyArchetype::Demon, FVector(-460.0f, -1180.0f, 100.0f) },
        { FName(TEXT("galilee_named_boss_01")), TEXT("Legion Sovereign of Gerasa"), ENazareneEnemyArchetype::Boss, FVector(0.0f, -2200.0f, 100.0f) }
    };

    FNazareneRegionDefinition Decapolis;
    Decapolis.RegionId = FName(TEXT("decapolis"));
    Decapolis.RegionName = TEXT("Decapolis Ruins");
    Decapolis.Chapter = 2;
    Decapolis.Objective = TEXT("Redeem the Gadarene Warlord among the ruins.");
    Decapolis.PlayerSpawn = FVector(0.0f, 0.0f, 220.0f);
    Decapolis.PrayerSiteId = FName(TEXT("decapolis_site_01"));
    Decapolis.PrayerSiteName = TEXT("Prayer Site: Decapolis Ruins");
    Decapolis.PrayerSiteLocation = FVector(0.0f, 0.0f, 20.0f);
    Decapolis.TravelGatePrompt = TEXT("Press E to travel to Wilderness of Temptation");
    Decapolis.TravelGateLocation = FVector(0.0f, 1900.0f, 20.0f);
    Decapolis.BossSpawnId = FName(TEXT("decapolis_named_boss_01"));
    Decapolis.RewardMiracle = FName(TEXT("radiance"));
    Decapolis.RewardHealthBonus = 10.0f;
    Decapolis.RewardStaminaBonus = 6.0f;
    Decapolis.Enemies =
    {
        { FName(TEXT("decapolis_shield_01")), TEXT("Roman Shieldbearer"), ENazareneEnemyArchetype::MeleeShield, FVector(700.0f, 350.0f, 100.0f) },
        { FName(TEXT("decapolis_shield_02")), TEXT("Roman Shieldbearer"), ENazareneEnemyArchetype::MeleeShield, FVector(-700.0f, 420.0f, 100.0f) },
        { FName(TEXT("decapolis_spear_01")), TEXT("Roman Spearman"), ENazareneEnemyArchetype::Spear, FVector(1150.0f, -250.0f, 100.0f) },
        { FName(TEXT("decapolis_spear_02")), TEXT("Roman Spearman"), ENazareneEnemyArchetype::Spear, FVector(-1150.0f, -250.0f, 100.0f) },
        { FName(TEXT("decapolis_ranged_01")), TEXT("Roman Slinger"), ENazareneEnemyArchetype::Ranged, FVector(1300.0f, 800.0f, 100.0f) },
        { FName(TEXT("decapolis_ranged_02")), TEXT("Roman Slinger"), ENazareneEnemyArchetype::Ranged, FVector(-1300.0f, 820.0f, 100.0f) },
        { FName(TEXT("decapolis_demon_01")), TEXT("Unclean Spirit"), ENazareneEnemyArchetype::Demon, FVector(350.0f, -1400.0f, 100.0f) },
        { FName(TEXT("decapolis_demon_02")), TEXT("Unclean Spirit"), ENazareneEnemyArchetype::Demon, FVector(-350.0f, -1450.0f, 100.0f) },
        { FName(TEXT("decapolis_named_boss_01")), TEXT("Gadarene Warlord"), ENazareneEnemyArchetype::Boss, FVector(0.0f, -2300.0f, 100.0f) }
    };

    FNazareneRegionDefinition Wilderness;
    Wilderness.RegionId = FName(TEXT("wilderness"));
    Wilderness.RegionName = TEXT("Wilderness of Temptation");
    Wilderness.Chapter = 3;
    Wilderness.Objective = TEXT("Resist the Adversary of the Desert.");
    Wilderness.PlayerSpawn = FVector(0.0f, 0.0f, 220.0f);
    Wilderness.PrayerSiteId = FName(TEXT("wilderness_site_01"));
    Wilderness.PrayerSiteName = TEXT("Prayer Site: Wilderness Ridge");
    Wilderness.PrayerSiteLocation = FVector(0.0f, 0.0f, 20.0f);
    Wilderness.TravelGatePrompt = TEXT("Press E to travel to Jerusalem Approach");
    Wilderness.TravelGateLocation = FVector(0.0f, 1900.0f, 20.0f);
    Wilderness.BossSpawnId = FName(TEXT("wilderness_named_boss_01"));
    Wilderness.RewardHealthBonus = 12.0f;
    Wilderness.RewardStaminaBonus = 8.0f;
    Wilderness.Enemies =
    {
        { FName(TEXT("wilderness_ranged_01")), TEXT("Desert Slinger"), ENazareneEnemyArchetype::Ranged, FVector(1200.0f, 800.0f, 100.0f) },
        { FName(TEXT("wilderness_ranged_02")), TEXT("Desert Slinger"), ENazareneEnemyArchetype::Ranged, FVector(-1200.0f, 850.0f, 100.0f) },
        { FName(TEXT("wilderness_spear_01")), TEXT("Desert Spearman"), ENazareneEnemyArchetype::Spear, FVector(850.0f, -350.0f, 100.0f) },
        { FName(TEXT("wilderness_spear_02")), TEXT("Desert Spearman"), ENazareneEnemyArchetype::Spear, FVector(-850.0f, -420.0f, 100.0f) },
        { FName(TEXT("wilderness_demon_01")), TEXT("Tempter Spirit"), ENazareneEnemyArchetype::Demon, FVector(260.0f, -1450.0f, 100.0f) },
        { FName(TEXT("wilderness_demon_02")), TEXT("Tempter Spirit"), ENazareneEnemyArchetype::Demon, FVector(-260.0f, -1480.0f, 100.0f) },
        { FName(TEXT("wilderness_demon_03")), TEXT("Tempter Spirit"), ENazareneEnemyArchetype::Demon, FVector(420.0f, -1900.0f, 100.0f) },
        { FName(TEXT("wilderness_named_boss_01")), TEXT("Adversary of the Desert"), ENazareneEnemyArchetype::Boss, FVector(0.0f, -2400.0f, 100.0f) }
    };

    FNazareneRegionDefinition Jerusalem;
    Jerusalem.RegionId = FName(TEXT("jerusalem"));
    Jerusalem.RegionName = TEXT("Jerusalem Approach");
    Jerusalem.Chapter = 4;
    Jerusalem.Objective = TEXT("Face the Temple Warden at the city's gate.");
    Jerusalem.PlayerSpawn = FVector(0.0f, 0.0f, 220.0f);
    Jerusalem.PrayerSiteId = FName(TEXT("jerusalem_site_01"));
    Jerusalem.PrayerSiteName = TEXT("Prayer Site: Jerusalem Approach");
    Jerusalem.PrayerSiteLocation = FVector(0.0f, 0.0f, 20.0f);
    Jerusalem.TravelGatePrompt = TEXT("Press E to conclude the pilgrimage");
    Jerusalem.TravelGateLocation = FVector(0.0f, 1900.0f, 20.0f);
    Jerusalem.BossSpawnId = FName(TEXT("jerusalem_named_boss_01"));
    Jerusalem.Enemies =
    {
        { FName(TEXT("jerusalem_shield_01")), TEXT("Temple Shieldbearer"), ENazareneEnemyArchetype::MeleeShield, FVector(900.0f, 450.0f, 100.0f) },
        { FName(TEXT("jerusalem_shield_02")), TEXT("Temple Shieldbearer"), ENazareneEnemyArchetype::MeleeShield, FVector(-900.0f, 450.0f, 100.0f) },
        { FName(TEXT("jerusalem_spear_01")), TEXT("Temple Spearman"), ENazareneEnemyArchetype::Spear, FVector(1200.0f, -250.0f, 100.0f) },
        { FName(TEXT("jerusalem_spear_02")), TEXT("Temple Spearman"), ENazareneEnemyArchetype::Spear, FVector(-1200.0f, -250.0f, 100.0f) },
        { FName(TEXT("jerusalem_ranged_01")), TEXT("Temple Slinger"), ENazareneEnemyArchetype::Ranged, FVector(1400.0f, 900.0f, 100.0f) },
        { FName(TEXT("jerusalem_ranged_02")), TEXT("Temple Slinger"), ENazareneEnemyArchetype::Ranged, FVector(-1400.0f, 920.0f, 100.0f) },
        { FName(TEXT("jerusalem_demon_01")), TEXT("Unclean Spirit"), ENazareneEnemyArchetype::Demon, FVector(350.0f, -1600.0f, 100.0f) },
        { FName(TEXT("jerusalem_demon_02")), TEXT("Unclean Spirit"), ENazareneEnemyArchetype::Demon, FVector(-350.0f, -1650.0f, 100.0f) },
        { FName(TEXT("jerusalem_named_boss_01")), TEXT("Temple Warden"), ENazareneEnemyArchetype::Boss, FVector(0.0f, -2450.0f, 100.0f) }
    };

    Regions = { Galilee, Decapolis, Wilderness, Jerusalem };
}

void ANazareneCampaignGameMode::LoadRegion(int32 TargetRegionIndex)
{
    if (Regions.Num() == 0)
    {
        return;
    }

    ClearRegionActors();
    EnemyBySpawnId.Empty();
    BossEnemy = nullptr;
    TravelGate = nullptr;

    RegionIndex = FMath::Clamp(TargetRegionIndex, 0, Regions.Num() - 1);
    if (Session)
    {
        Session->GetMutableCampaignState().RegionIndex = RegionIndex;
    }

    const FNazareneRegionDefinition& Region = Regions[RegionIndex];
    SpawnRegionEnvironment(Region);
    SpawnRegionActors(Region);

    PlayerCharacter = Cast<ANazarenePlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (PlayerCharacter == nullptr)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC != nullptr)
        {
            PlayerCharacter = GetWorld()->SpawnActor<ANazarenePlayerCharacter>(ANazarenePlayerCharacter::StaticClass(), Region.PlayerSpawn, FRotator::ZeroRotator);
            if (PlayerCharacter != nullptr)
            {
                PC->Possess(PlayerCharacter);
            }
        }
    }

    if (PlayerCharacter != nullptr)
    {
        PlayerCharacter->SetActorLocation(Region.PlayerSpawn);
        PlayerCharacter->SetCampaignGameMode(this);

        if (Session)
        {
            const FNazareneCampaignState& State = Session->GetCampaignState();
            PlayerCharacter->MaxHealth = 120.0f + State.MaxHealthBonus;
            PlayerCharacter->MaxStamina = 100.0f + State.MaxStaminaBonus;
            PlayerCharacter->SetUnlockedMiracles(State.UnlockedMiracles);
        }
    }

    bRegionCompleted = false;
    SyncCompletionState();
    UpdateHUDForRegion(Region, bRegionCompleted);
}

void ANazareneCampaignGameMode::ClearRegionActors()
{
    for (AActor* Actor : RegionActors)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    RegionActors.Empty();
}

void ANazareneCampaignGameMode::SpawnRegionEnvironment(const FNazareneRegionDefinition& Region)
{
    ADirectionalLight* Sun = GetWorld()->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), FVector(0.0f, 0.0f, 600.0f), FRotator(-38.0f, -32.0f, 0.0f));
    if (Sun != nullptr)
    {
        Sun->GetLightComponent()->SetIntensity(10.0f);
        RegionActors.Add(Sun);
    }

    AStaticMeshActor* Ground = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(0.0f, 0.0f, -50.0f), FRotator::ZeroRotator);
    if (Ground != nullptr)
    {
        UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
        if (CubeMesh != nullptr)
        {
            Ground->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
            Ground->GetStaticMeshComponent()->SetWorldScale3D(FVector(120.0f, 120.0f, 1.0f));
        }
        RegionActors.Add(Ground);
    }

    AStaticMeshActor* Arena = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(0.0f, -2200.0f, -45.0f), FRotator::ZeroRotator);
    if (Arena != nullptr)
    {
        UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
        if (CylinderMesh != nullptr)
        {
            Arena->GetStaticMeshComponent()->SetStaticMesh(CylinderMesh);
            Arena->GetStaticMeshComponent()->SetWorldScale3D(FVector(10.0f, 10.0f, 0.2f));
        }
        RegionActors.Add(Arena);
    }
}

void ANazareneCampaignGameMode::SpawnRegionActors(const FNazareneRegionDefinition& Region)
{
    ANazarenePrayerSite* Site = GetWorld()->SpawnActor<ANazarenePrayerSite>(ANazarenePrayerSite::StaticClass(), Region.PrayerSiteLocation, FRotator::ZeroRotator);
    if (Site != nullptr)
    {
        Site->SiteId = Region.PrayerSiteId;
        Site->SiteName = Region.PrayerSiteName;
        Site->PromptText = TEXT("Press E to pray and rest");
        RegionActors.Add(Site);
    }

    TravelGate = GetWorld()->SpawnActor<ANazareneTravelGate>(ANazareneTravelGate::StaticClass(), Region.TravelGateLocation, FRotator::ZeroRotator);
    if (TravelGate != nullptr)
    {
        TravelGate->TargetRegionIndex = RegionIndex + 1;
        TravelGate->PromptText = Region.TravelGatePrompt;
        TravelGate->SetGateEnabled(false);
        RegionActors.Add(TravelGate);
    }

    for (const FNazareneEnemySpawnDefinition& Spec : Region.Enemies)
    {
        ANazareneEnemyCharacter* Enemy = GetWorld()->SpawnActor<ANazareneEnemyCharacter>(ANazareneEnemyCharacter::StaticClass(), Spec.Location, FRotator::ZeroRotator);
        if (Enemy == nullptr)
        {
            continue;
        }

        Enemy->SpawnId = Spec.SpawnId;
        Enemy->EnemyName = Spec.EnemyName;
        Enemy->Archetype = Spec.Archetype;
        Enemy->ConfigureFromArchetype();
        Enemy->OnRedeemed.AddDynamic(this, &ANazareneCampaignGameMode::HandleEnemyRedeemed);

        RegionActors.Add(Enemy);
        EnemyBySpawnId.Add(Spec.SpawnId, Enemy);
        if (Spec.SpawnId == Region.BossSpawnId)
        {
            BossEnemy = Enemy;
        }
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
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
            {
                HUD->ShowMessage(TEXT("Pilgrimage complete. Roll credits scene from your Unreal level blueprint."), 6.0f);
            }
        }
        SaveCheckpoint();
        return;
    }

    LoadRegion(TargetRegionIndex);
    SaveCheckpoint();
}

bool ANazareneCampaignGameMode::SaveToSlot(int32 SlotId)
{
    if (SaveSubsystem == nullptr || SlotId < 1)
    {
        return false;
    }
    return SaveSubsystem->SavePayloadToSlot(SlotId, BuildSavePayload());
}

bool ANazareneCampaignGameMode::LoadFromSlot(int32 SlotId)
{
    if (SaveSubsystem == nullptr || SlotId < 1)
    {
        return false;
    }

    FNazareneSavePayload Payload;
    if (!SaveSubsystem->LoadPayloadFromSlot(SlotId, Payload))
    {
        return false;
    }

    if (Session)
    {
        Session->GetMutableCampaignState() = Payload.Campaign;
    }

    if (Payload.Campaign.RegionIndex != RegionIndex)
    {
        LoadRegion(Payload.Campaign.RegionIndex);
    }

    ApplySavePayload(Payload);
    return true;
}

bool ANazareneCampaignGameMode::SaveCheckpoint()
{
    if (SaveSubsystem == nullptr)
    {
        return false;
    }
    return SaveSubsystem->SaveCheckpoint(BuildSavePayload());
}

bool ANazareneCampaignGameMode::LoadCheckpoint()
{
    if (SaveSubsystem == nullptr)
    {
        return false;
    }
    FNazareneSavePayload Payload;
    if (!SaveSubsystem->LoadCheckpoint(Payload))
    {
        return false;
    }
    ApplySavePayload(Payload);
    return true;
}

FNazareneSavePayload ANazareneCampaignGameMode::BuildSavePayload() const
{
    FNazareneSavePayload Payload;

    if (PlayerCharacter != nullptr)
    {
        Payload.Player = PlayerCharacter->BuildSnapshot();
    }

    for (const TPair<FName, TObjectPtr<ANazareneEnemyCharacter>>& Pair : EnemyBySpawnId)
    {
        if (IsValid(Pair.Value))
        {
            Payload.Enemies.Add(Pair.Value->BuildSnapshot());
        }
    }

    if (Session)
    {
        Payload.Campaign = Session->GetCampaignState();
    }
    Payload.Campaign.RegionIndex = RegionIndex;
    return Payload;
}

void ANazareneCampaignGameMode::ApplySavePayload(const FNazareneSavePayload& Payload)
{
    if (Session)
    {
        Session->GetMutableCampaignState() = Payload.Campaign;
    }

    if (Payload.Campaign.RegionIndex != RegionIndex)
    {
        LoadRegion(Payload.Campaign.RegionIndex);
    }

    if (PlayerCharacter != nullptr)
    {
        PlayerCharacter->MaxHealth = 120.0f + Payload.Campaign.MaxHealthBonus;
        PlayerCharacter->MaxStamina = 100.0f + Payload.Campaign.MaxStaminaBonus;
        PlayerCharacter->SetUnlockedMiracles(Payload.Campaign.UnlockedMiracles);
        PlayerCharacter->ApplySnapshot(Payload.Player);
    }

    TMap<FName, FNazareneEnemySnapshot> Snapshots;
    for (const FNazareneEnemySnapshot& Snapshot : Payload.Enemies)
    {
        Snapshots.Add(Snapshot.SpawnId, Snapshot);
    }

    bSuppressRedeemedCallbacks = true;
    for (const TPair<FName, TObjectPtr<ANazareneEnemyCharacter>>& Pair : EnemyBySpawnId)
    {
        ANazareneEnemyCharacter* Enemy = Pair.Value;
        if (!IsValid(Enemy))
        {
            continue;
        }
        const FNazareneEnemySnapshot* Snapshot = Snapshots.Find(Pair.Key);
        if (Snapshot != nullptr)
        {
            Enemy->ApplySnapshot(*Snapshot);
        }
        else
        {
            Enemy->ResetToSpawn();
        }
    }
    bSuppressRedeemedCallbacks = false;

    SyncCompletionState();
    if (Regions.IsValidIndex(RegionIndex))
    {
        UpdateHUDForRegion(Regions[RegionIndex], bRegionCompleted);
    }
}

void ANazareneCampaignGameMode::SyncCompletionState()
{
    bRegionCompleted = (BossEnemy != nullptr && BossEnemy->IsRedeemed());
    EnableTravelGate(bRegionCompleted);
}

void ANazareneCampaignGameMode::HandleEnemyRedeemed(ANazareneEnemyCharacter* Enemy, float FaithReward)
{
    if (bSuppressRedeemedCallbacks)
    {
        return;
    }
    if (Enemy != BossEnemy)
    {
        return;
    }
    OnBossRedeemed();
}

void ANazareneCampaignGameMode::OnBossRedeemed()
{
    if (bRegionCompleted || !Regions.IsValidIndex(RegionIndex))
    {
        return;
    }

    bRegionCompleted = true;
    const FNazareneRegionDefinition& Region = Regions[RegionIndex];
    const bool bRewardApplied = ApplyRegionReward(Region);
    EnableTravelGate(true);
    UpdateHUDForRegion(Region, true);

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
        {
            HUD->ShowMessage(bRewardApplied ? TEXT("Blessings strengthened. The way forward is open.") : TEXT("The way forward is open."));
        }
    }

    SaveCheckpoint();
}

bool ANazareneCampaignGameMode::ApplyRegionReward(const FNazareneRegionDefinition& Region)
{
    if (Session == nullptr)
    {
        return false;
    }

    const FName RewardFlag(*FString::Printf(TEXT("boss_%s"), *Region.RegionId.ToString()));
    if (Session->IsFlagSet(RewardFlag))
    {
        return false;
    }

    Session->MarkFlag(RewardFlag);
    FNazareneCampaignState& State = Session->GetMutableCampaignState();

    bool bAnyReward = false;
    if (!Region.RewardMiracle.IsNone())
    {
        bAnyReward |= Session->EnsureMiracleUnlocked(Region.RewardMiracle);
    }
    if (Region.RewardHealthBonus > 0.0f)
    {
        State.MaxHealthBonus += Region.RewardHealthBonus;
        bAnyReward = true;
    }
    if (Region.RewardStaminaBonus > 0.0f)
    {
        State.MaxStaminaBonus += Region.RewardStaminaBonus;
        bAnyReward = true;
    }

    if (PlayerCharacter != nullptr)
    {
        PlayerCharacter->MaxHealth = 120.0f + State.MaxHealthBonus;
        PlayerCharacter->MaxStamina = 100.0f + State.MaxStaminaBonus;
        PlayerCharacter->SetUnlockedMiracles(State.UnlockedMiracles);

        FNazarenePlayerSnapshot Snapshot = PlayerCharacter->BuildSnapshot();
        Snapshot.Health = PlayerCharacter->MaxHealth;
        Snapshot.Stamina = PlayerCharacter->MaxStamina;
        PlayerCharacter->ApplySnapshot(Snapshot);
    }

    return bAnyReward;
}

void ANazareneCampaignGameMode::EnableTravelGate(bool bEnabled)
{
    if (TravelGate != nullptr)
    {
        TravelGate->SetGateEnabled(bEnabled);
    }
}

void ANazareneCampaignGameMode::UpdateHUDForRegion(const FNazareneRegionDefinition& Region, bool bCompleted) const
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC == nullptr)
    {
        return;
    }

    ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD());
    if (HUD == nullptr)
    {
        return;
    }

    HUD->SetRegionName(FString::Printf(TEXT("Chapter %d: %s"), Region.Chapter, *Region.RegionName));
    if (!bCompleted)
    {
        HUD->SetObjective(Region.Objective);
    }
    else if (RegionIndex >= Regions.Num() - 1)
    {
        HUD->SetObjective(TEXT("Step into the light to conclude the pilgrimage."));
    }
    else
    {
        HUD->SetObjective(FString::Printf(TEXT("Journey marker open: travel to %s."), *Regions[RegionIndex + 1].RegionName));
    }
}

