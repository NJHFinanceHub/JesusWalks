#include "NazareneCampaignGameMode.h"

#include "BehaviorTree/BehaviorTree.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PointLight.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NazareneMenuCameraActor.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "NazareneEnemyCharacter.h"
#include "NazareneGameInstance.h"
#include "NazareneNPC.h"
#include "NazareneRegionDataAsset.h"
#include "NazareneHUD.h"
#include "NazarenePlayerCharacter.h"
#include "NazarenePrayerSite.h"
#include "NazareneSaveSubsystem.h"
#include "NazareneTravelGate.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

ANazareneCampaignGameMode::ANazareneCampaignGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    DefaultPawnClass = ANazarenePlayerCharacter::StaticClass();
    HUDClass = ANazareneHUD::StaticClass();
    PlayerControllerClass = APlayerController::StaticClass();

    GalileeMusic = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/Game/Audio/Music/S_Music_Galilee.S_Music_Galilee")));
    DecapolisMusic = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/Game/Audio/Music/S_Music_Decapolis.S_Music_Decapolis")));
    WildernessMusic = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/Game/Audio/Music/S_Music_Wilderness.S_Music_Wilderness")));
    JerusalemMusic = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/Game/Audio/Music/S_Music_Jerusalem.S_Music_Jerusalem")));
    GethsemaneMusic = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/Game/Audio/Music/S_Music_Gethsemane.S_Music_Gethsemane")));
    ViaDolorosaMusic = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/Game/Audio/Music/S_Music_ViaDolorosa.S_Music_ViaDolorosa")));
    EmptyTombMusic = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/Game/Audio/Music/S_Music_EmptyTomb.S_Music_EmptyTomb")));

    BTMeleeShieldAsset = TSoftObjectPtr<UBehaviorTree>(FSoftObjectPath(TEXT("/Game/AI/BehaviorTrees/BT_MeleeShield.BT_MeleeShield")));
    BTSpearAsset = TSoftObjectPtr<UBehaviorTree>(FSoftObjectPath(TEXT("/Game/AI/BehaviorTrees/BT_Spear.BT_Spear")));
    BTRangedAsset = TSoftObjectPtr<UBehaviorTree>(FSoftObjectPath(TEXT("/Game/AI/BehaviorTrees/BT_Ranged.BT_Ranged")));
    BTDemonAsset = TSoftObjectPtr<UBehaviorTree>(FSoftObjectPath(TEXT("/Game/AI/BehaviorTrees/BT_Demon.BT_Demon")));
    BTBossAsset = TSoftObjectPtr<UBehaviorTree>(FSoftObjectPath(TEXT("/Game/AI/BehaviorTrees/BT_Boss.BT_Boss")));
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
    QueueIntroStoryIfNeeded();

    // Task 7: Spawn orbiting menu camera while the start menu is visible
    SpawnMenuCamera();
}

void ANazareneCampaignGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    TickCrossfade(DeltaSeconds);
}

void ANazareneCampaignGameMode::BuildDefaultRegions()
{
    if (Regions.Num() > 0)
    {
        return;
    }

    if (RegionDataAsset != nullptr)
    {
        Regions = RegionDataAsset->Regions;
        return;
    }

    FNazareneRegionDefinition Galilee;
    Galilee.RegionId = FName(TEXT("galilee"));
    Galilee.RegionName = TEXT("Galilee Shores");
    Galilee.Chapter = 1;
    Galilee.Objective = TEXT("Redeem the Legion Sovereign of Gerasa.");
    Galilee.StreamedLevelPackage = FName(TEXT("/Game/Maps/Regions/Galilee/L_GalileeShores"));
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
    Galilee.LoreText = TEXT("By these shores the Teacher called his first followers, saying: Come, and I will make you fishers of men.");
    Galilee.NPCs =
    {
        { TEXT("Peter"), TEXT("peter"), FVector(-350.0f, 250.0f, 100.0f), TEXT("Press E to speak with Peter"), { { TEXT("Follow me, and I will make you fishers of men."), 4.0f }, { TEXT("The nets are full, Lord."), 4.0f } } },
        { TEXT("Mary"), TEXT("mary"), FVector(350.0f, 280.0f, 100.0f), TEXT("Press E to speak with Mary"), { { TEXT("He has done great things for us."), 4.0f } } }
    };

    FNazareneRegionDefinition Decapolis;
    Decapolis.RegionId = FName(TEXT("decapolis"));
    Decapolis.RegionName = TEXT("Decapolis Ruins");
    Decapolis.Chapter = 2;
    Decapolis.Objective = TEXT("Redeem the Gadarene Warlord among the ruins.");
    Decapolis.StreamedLevelPackage = FName(TEXT("/Game/Maps/Regions/Decapolis/L_DecapolisRuins"));
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
    Decapolis.LoreText = TEXT("Among the ruins of Gentile cities, mercy crossed every boundary.");
    Decapolis.NPCs =
    {
        { TEXT("Andrew"), TEXT("andrew"), FVector(-350.0f, 250.0f, 100.0f), TEXT("Press E to speak with Andrew"), { { TEXT("We have found the Messiah."), 4.0f } } },
        { TEXT("John the Baptist"), TEXT("john_baptist"), FVector(350.0f, 280.0f, 100.0f), TEXT("Press E to speak with John the Baptist"), { { TEXT("He must increase, but I must decrease."), 4.0f }, { TEXT("Behold, the Lamb of God."), 4.0f } } }
    };

    FNazareneRegionDefinition Wilderness;
    Wilderness.RegionId = FName(TEXT("wilderness"));
    Wilderness.RegionName = TEXT("Wilderness of Temptation");
    Wilderness.Chapter = 3;
    Wilderness.Objective = TEXT("Resist the Adversary of the Desert.");
    Wilderness.StreamedLevelPackage = FName(TEXT("/Game/Maps/Regions/Wilderness/L_WildernessTemptation"));
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
    Wilderness.LoreText = TEXT("Forty days in solitude. Every temptation met with scripture and obedience.");
    Wilderness.NPCs =
    {
        { TEXT("Moses"), TEXT("moses"), FVector(-350.0f, 250.0f, 100.0f), TEXT("Press E to speak with Moses"), { { TEXT("Be strong and courageous. The Lord your God goes with you."), 4.0f } } },
        { TEXT("Elijah"), TEXT("elijah"), FVector(350.0f, 280.0f, 100.0f), TEXT("Press E to speak with Elijah"), { { TEXT("The Lord is God. The Lord is God."), 4.0f }, { TEXT("I alone am left, yet the Lord preserves."), 4.0f } } }
    };
    Wilderness.EncounterWaves =
    {
        { ENazareneSpawnTrigger::OnPrayerSiteConsecrated, 0, { { FName(TEXT("wilderness_wave_demon_01")), TEXT("Tempter Spirit"), ENazareneEnemyArchetype::Demon, FVector(600.0f, -800.0f, 100.0f) }, { FName(TEXT("wilderness_wave_demon_02")), TEXT("Tempter Spirit"), ENazareneEnemyArchetype::Demon, FVector(-600.0f, -800.0f, 100.0f) } }, 1.5f }
    };

    FNazareneRegionDefinition Jerusalem;
    Jerusalem.RegionId = FName(TEXT("jerusalem"));
    Jerusalem.RegionName = TEXT("Jerusalem Approach");
    Jerusalem.Chapter = 4;
    Jerusalem.Objective = TEXT("Face the Temple Warden at the city's gate.");
    Jerusalem.StreamedLevelPackage = FName(TEXT("/Game/Maps/Regions/Jerusalem/L_JerusalemApproach"));
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
    Jerusalem.LoreText = TEXT("The city on the hill, where prophets are received and rejected alike.");
    Jerusalem.NPCs =
    {
        { TEXT("Isaiah"), TEXT("isaiah"), FVector(-350.0f, 250.0f, 100.0f), TEXT("Press E to speak with Isaiah"), { { TEXT("Though your sins are like scarlet, they shall be white as snow."), 4.0f } } },
        { TEXT("Jeremiah"), TEXT("jeremiah"), FVector(350.0f, 280.0f, 100.0f), TEXT("Press E to speak with Jeremiah"), { { TEXT("I will put my law within them and write it on their hearts."), 4.0f }, { TEXT("For I know the plans I have for you, declares the Lord."), 4.0f } } }
    };
    Jerusalem.EncounterWaves =
    {
        { ENazareneSpawnTrigger::OnPrayerSiteConsecrated, 0, { { FName(TEXT("jerusalem_wave_shield_01")), TEXT("Temple Guard"), ENazareneEnemyArchetype::MeleeShield, FVector(700.0f, -800.0f, 100.0f) }, { FName(TEXT("jerusalem_wave_shield_02")), TEXT("Temple Guard"), ENazareneEnemyArchetype::MeleeShield, FVector(-700.0f, -800.0f, 100.0f) } }, 1.5f }
    };

    // Update Jerusalem travel gate prompt for new chapter progression
    Jerusalem.TravelGatePrompt = TEXT("Press E to travel to Gethsemane");

    FNazareneRegionDefinition Gethsemane;
    Gethsemane.RegionId = FName(TEXT("gethsemane"));
    Gethsemane.RegionName = TEXT("Garden of Gethsemane");
    Gethsemane.Chapter = 5;
    Gethsemane.Objective = TEXT("Endure the hour of anguish in the garden.");
    Gethsemane.StreamedLevelPackage = FName(TEXT("/Game/Maps/Regions/Gethsemane/L_GardenGethsemane"));
    Gethsemane.PlayerSpawn = FVector(0.0f, 0.0f, 220.0f);
    Gethsemane.PrayerSiteId = FName(TEXT("gethsemane_site_01"));
    Gethsemane.PrayerSiteName = TEXT("Prayer Site: Garden of Gethsemane");
    Gethsemane.PrayerSiteLocation = FVector(0.0f, 0.0f, 20.0f);
    Gethsemane.TravelGatePrompt = TEXT("Press E to travel to Via Dolorosa");
    Gethsemane.TravelGateLocation = FVector(0.0f, 1900.0f, 20.0f);
    Gethsemane.BossSpawnId = FName(TEXT("gethsemane_named_boss_01"));
    Gethsemane.RewardHealthBonus = 14.0f;
    Gethsemane.RewardStaminaBonus = 10.0f;
    Gethsemane.LoreText = TEXT("Not my will, but yours be done.");
    Gethsemane.Enemies =
    {
        { FName(TEXT("gethsemane_shield_01")), TEXT("Temple Guard I"), ENazareneEnemyArchetype::MeleeShield, FVector(900.0f, 400.0f, 100.0f) },
        { FName(TEXT("gethsemane_shield_02")), TEXT("Temple Guard II"), ENazareneEnemyArchetype::MeleeShield, FVector(-900.0f, 430.0f, 100.0f) },
        { FName(TEXT("gethsemane_spear_01")), TEXT("Garden Sentinel I"), ENazareneEnemyArchetype::Spear, FVector(1200.0f, -300.0f, 100.0f) },
        { FName(TEXT("gethsemane_spear_02")), TEXT("Garden Sentinel II"), ENazareneEnemyArchetype::Spear, FVector(-1200.0f, -350.0f, 100.0f) },
        { FName(TEXT("gethsemane_ranged_01")), TEXT("Temple Archer"), ENazareneEnemyArchetype::Ranged, FVector(1300.0f, 900.0f, 100.0f) },
        { FName(TEXT("gethsemane_ranged_02")), TEXT("Temple Archer"), ENazareneEnemyArchetype::Ranged, FVector(-1300.0f, 920.0f, 100.0f) },
        { FName(TEXT("gethsemane_demon_01")), TEXT("Spirit of Anguish I"), ENazareneEnemyArchetype::Demon, FVector(400.0f, -1300.0f, 100.0f) },
        { FName(TEXT("gethsemane_demon_02")), TEXT("Spirit of Anguish II"), ENazareneEnemyArchetype::Demon, FVector(-400.0f, -1350.0f, 100.0f) },
        { FName(TEXT("gethsemane_demon_03")), TEXT("Spirit of Anguish III"), ENazareneEnemyArchetype::Demon, FVector(0.0f, -1700.0f, 100.0f) },
        { FName(TEXT("gethsemane_named_boss_01")), TEXT("Warden of the Garden"), ENazareneEnemyArchetype::Boss, FVector(0.0f, -2400.0f, 100.0f) }
    };
    Gethsemane.NPCs =
    {
        { TEXT("Peter"), TEXT("peter"), FVector(-300.0f, 200.0f, 100.0f), TEXT("Press E to speak with Peter"), { { TEXT("Stay awake and pray, lest you fall into temptation."), 4.0f }, { TEXT("The spirit is willing, but the flesh is weak."), 4.0f } } },
        { TEXT("James"), TEXT("james"), FVector(-500.0f, 300.0f, 100.0f), TEXT("Press E to speak with James"), { { TEXT("We will keep watch with you, Lord."), 4.0f } } },
        { TEXT("John"), TEXT("john"), FVector(-400.0f, 400.0f, 100.0f), TEXT("Press E to speak with John"), { { TEXT("The hour has come. Be strong and courageous."), 4.0f } } }
    };
    Gethsemane.EncounterWaves =
    {
        { ENazareneSpawnTrigger::OnBossPhaseChange, 2, { { FName(TEXT("gethsemane_wave_demon_01")), TEXT("Spirit of Dread"), ENazareneEnemyArchetype::Demon, FVector(600.0f, -2000.0f, 100.0f) }, { FName(TEXT("gethsemane_wave_demon_02")), TEXT("Spirit of Dread"), ENazareneEnemyArchetype::Demon, FVector(-600.0f, -2000.0f, 100.0f) } }, 1.0f }
    };

    FNazareneRegionDefinition ViaDolorosa;
    ViaDolorosa.RegionId = FName(TEXT("via_dolorosa"));
    ViaDolorosa.RegionName = TEXT("Via Dolorosa");
    ViaDolorosa.Chapter = 6;
    ViaDolorosa.Objective = TEXT("Bear the weight of judgment on the road of sorrow.");
    ViaDolorosa.StreamedLevelPackage = FName(TEXT("/Game/Maps/Regions/ViaDolorosa/L_ViaDolorosa"));
    ViaDolorosa.PlayerSpawn = FVector(0.0f, 0.0f, 220.0f);
    ViaDolorosa.PrayerSiteId = FName(TEXT("via_dolorosa_site_01"));
    ViaDolorosa.PrayerSiteName = TEXT("Prayer Site: Via Dolorosa");
    ViaDolorosa.PrayerSiteLocation = FVector(0.0f, 0.0f, 20.0f);
    ViaDolorosa.TravelGatePrompt = TEXT("Press E to travel to the Empty Tomb");
    ViaDolorosa.TravelGateLocation = FVector(0.0f, 1900.0f, 20.0f);
    ViaDolorosa.BossSpawnId = FName(TEXT("via_dolorosa_named_boss_01"));
    ViaDolorosa.RewardHealthBonus = 16.0f;
    ViaDolorosa.RewardStaminaBonus = 12.0f;
    ViaDolorosa.LoreText = TEXT("He bore the cross for the joy set before him.");
    ViaDolorosa.Enemies =
    {
        { FName(TEXT("via_dolorosa_shield_01")), TEXT("Roman Legionary I"), ENazareneEnemyArchetype::MeleeShield, FVector(800.0f, 350.0f, 100.0f) },
        { FName(TEXT("via_dolorosa_shield_02")), TEXT("Roman Legionary II"), ENazareneEnemyArchetype::MeleeShield, FVector(-800.0f, 380.0f, 100.0f) },
        { FName(TEXT("via_dolorosa_shield_03")), TEXT("Roman Legionary III"), ENazareneEnemyArchetype::MeleeShield, FVector(0.0f, 600.0f, 100.0f) },
        { FName(TEXT("via_dolorosa_spear_01")), TEXT("Roman Centurion"), ENazareneEnemyArchetype::Spear, FVector(1100.0f, -250.0f, 100.0f) },
        { FName(TEXT("via_dolorosa_spear_02")), TEXT("Roman Centurion"), ENazareneEnemyArchetype::Spear, FVector(-1100.0f, -280.0f, 100.0f) },
        { FName(TEXT("via_dolorosa_ranged_01")), TEXT("Roman Marksman"), ENazareneEnemyArchetype::Ranged, FVector(1400.0f, 850.0f, 100.0f) },
        { FName(TEXT("via_dolorosa_ranged_02")), TEXT("Roman Marksman"), ENazareneEnemyArchetype::Ranged, FVector(-1400.0f, 870.0f, 100.0f) },
        { FName(TEXT("via_dolorosa_demon_01")), TEXT("Spirit of Condemnation"), ENazareneEnemyArchetype::Demon, FVector(350.0f, -1500.0f, 100.0f) },
        { FName(TEXT("via_dolorosa_demon_02")), TEXT("Spirit of Condemnation"), ENazareneEnemyArchetype::Demon, FVector(-350.0f, -1550.0f, 100.0f) },
        { FName(TEXT("via_dolorosa_named_boss_01")), TEXT("Centurion of Golgotha"), ENazareneEnemyArchetype::Boss, FVector(0.0f, -2500.0f, 100.0f) }
    };
    ViaDolorosa.NPCs =
    {
        { TEXT("Mary"), TEXT("mary"), FVector(-350.0f, 250.0f, 100.0f), TEXT("Press E to speak with Mary"), { { TEXT("My son... my son."), 4.0f }, { TEXT("A sword shall pierce your own soul also."), 4.0f } } },
        { TEXT("Mary Magdalene"), TEXT("mary_magdalene"), FVector(350.0f, 280.0f, 100.0f), TEXT("Press E to speak with Mary Magdalene"), { { TEXT("We follow you to the end, Lord."), 4.0f } } }
    };
    ViaDolorosa.EncounterWaves =
    {
        { ENazareneSpawnTrigger::OnPrayerSiteConsecrated, 0, { { FName(TEXT("via_dolorosa_wave_shield_01")), TEXT("Roman Ambusher"), ENazareneEnemyArchetype::MeleeShield, FVector(700.0f, -800.0f, 100.0f) }, { FName(TEXT("via_dolorosa_wave_shield_02")), TEXT("Roman Ambusher"), ENazareneEnemyArchetype::MeleeShield, FVector(-700.0f, -800.0f, 100.0f) } }, 1.5f },
        { ENazareneSpawnTrigger::OnBossPhaseChange, 2, { { FName(TEXT("via_dolorosa_wave_demon_01")), TEXT("Spirit of Despair"), ENazareneEnemyArchetype::Demon, FVector(500.0f, -2100.0f, 100.0f) }, { FName(TEXT("via_dolorosa_wave_ranged_01")), TEXT("Roman Marksman"), ENazareneEnemyArchetype::Ranged, FVector(-500.0f, -2100.0f, 100.0f) } }, 1.0f }
    };

    FNazareneRegionDefinition EmptyTomb;
    EmptyTomb.RegionId = FName(TEXT("empty_tomb"));
    EmptyTomb.RegionName = TEXT("The Empty Tomb");
    EmptyTomb.Chapter = 7;
    EmptyTomb.Objective = TEXT("Rise and proclaim the commission to the nations.");
    EmptyTomb.StreamedLevelPackage = FName(TEXT("/Game/Maps/Regions/EmptyTomb/L_EmptyTomb"));
    EmptyTomb.PlayerSpawn = FVector(0.0f, 0.0f, 220.0f);
    EmptyTomb.PrayerSiteId = FName(TEXT("empty_tomb_site_01"));
    EmptyTomb.PrayerSiteName = TEXT("Prayer Site: The Empty Tomb");
    EmptyTomb.PrayerSiteLocation = FVector(0.0f, 0.0f, 20.0f);
    EmptyTomb.TravelGatePrompt = TEXT("Step into the light to conclude the pilgrimage");
    EmptyTomb.TravelGateLocation = FVector(0.0f, 1900.0f, 20.0f);
    EmptyTomb.BossSpawnId = FName(TEXT("empty_tomb_named_boss_01"));
    EmptyTomb.LoreText = TEXT("He is not here. He is risen.");
    EmptyTomb.Enemies =
    {
        { FName(TEXT("empty_tomb_shield_01")), TEXT("Shadow Guard I"), ENazareneEnemyArchetype::MeleeShield, FVector(900.0f, 400.0f, 100.0f) },
        { FName(TEXT("empty_tomb_shield_02")), TEXT("Shadow Guard II"), ENazareneEnemyArchetype::MeleeShield, FVector(-900.0f, 430.0f, 100.0f) },
        { FName(TEXT("empty_tomb_spear_01")), TEXT("Shadow Warden"), ENazareneEnemyArchetype::Spear, FVector(1200.0f, -300.0f, 100.0f) },
        { FName(TEXT("empty_tomb_ranged_01")), TEXT("Shadow Archer"), ENazareneEnemyArchetype::Ranged, FVector(1400.0f, 900.0f, 100.0f) },
        { FName(TEXT("empty_tomb_demon_01")), TEXT("Spirit of Death I"), ENazareneEnemyArchetype::Demon, FVector(400.0f, -1300.0f, 100.0f) },
        { FName(TEXT("empty_tomb_demon_02")), TEXT("Spirit of Death II"), ENazareneEnemyArchetype::Demon, FVector(-400.0f, -1350.0f, 100.0f) },
        { FName(TEXT("empty_tomb_demon_03")), TEXT("Spirit of Death III"), ENazareneEnemyArchetype::Demon, FVector(0.0f, -1700.0f, 100.0f) },
        { FName(TEXT("empty_tomb_named_boss_01")), TEXT("Shadow of Death"), ENazareneEnemyArchetype::Boss, FVector(0.0f, -2500.0f, 100.0f) }
    };
    EmptyTomb.NPCs =
    {
        { TEXT("Mary Magdalene"), TEXT("mary_magdalene"), FVector(-300.0f, 250.0f, 100.0f), TEXT("Press E to speak with Mary Magdalene"), { { TEXT("I have seen the Lord!"), 4.0f }, { TEXT("He called my name, and I knew him."), 4.0f } } },
        { TEXT("Peter"), TEXT("peter"), FVector(300.0f, 280.0f, 100.0f), TEXT("Press E to speak with Peter"), { { TEXT("He is risen, just as he said."), 4.0f }, { TEXT("Go and tell the others."), 4.0f } } }
    };
    EmptyTomb.EncounterWaves =
    {
        { ENazareneSpawnTrigger::OnBossPhaseChange, 3, { { FName(TEXT("empty_tomb_wave_demon_01")), TEXT("Final Spirit"), ENazareneEnemyArchetype::Demon, FVector(600.0f, -2100.0f, 100.0f) }, { FName(TEXT("empty_tomb_wave_demon_02")), TEXT("Final Spirit"), ENazareneEnemyArchetype::Demon, FVector(-600.0f, -2100.0f, 100.0f) } }, 0.5f }
    };

    Regions = { Galilee, Decapolis, Wilderness, Jerusalem, Gethsemane, ViaDolorosa, EmptyTomb };
}

void ANazareneCampaignGameMode::LoadRegion(int32 TargetRegionIndex)
{
    if (Regions.Num() == 0)
    {
        return;
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
        {
            HUD->SetLoadingOverlayVisible(true, GetRandomLoreTip());
        }
    }

    UnloadRegionSublevel();
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
    const bool bLoadedStreamedLevel = TryLoadRegionSublevel(Region);
    if (!bLoadedStreamedLevel)
    {
        SpawnRegionEnvironment(Region);
    }
    SpawnRegionActors(Region);
    ActiveStoryLines.Empty();
    StoryLineIndex = 0;
    GetWorldTimerManager().ClearTimer(StoryLineTimerHandle);

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
            PlayerCharacter->SetCampaignBaseVitals(120.0f + State.MaxHealthBonus, 100.0f + State.MaxStaminaBonus, false);
            PlayerCharacter->SetUnlockedMiracles(State.UnlockedMiracles);
            PlayerCharacter->SetSkillTreeState(State.UnlockedSkills, State.SkillPoints, State.TotalXP, State.PlayerLevel);
        }
    }

    bRegionCompleted = false;
    bPrayerSiteConsecrated = false;
    if (Session && !Region.PrayerSiteId.IsNone())
    {
        const FName ConsecratedFlag(*FString::Printf(TEXT("site_%s_consecrated"), *Region.PrayerSiteId.ToString()));
        bPrayerSiteConsecrated = Session->IsFlagSet(ConsecratedFlag);
    }
    SyncCompletionState();
    EnsureRetryCounterForCurrentRegion();
    UpdateChapterStageFromState();
    UpdateHUDForRegion(Region, bRegionCompleted);
    SetMusicState(ENazareneMusicState::Peace, false);

    CrossfadeToMusic(ResolveRegionMusic(Region));

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
        {
            HUD->SetLoadingOverlayVisible(false, TEXT(""));
        }
    }
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

bool ANazareneCampaignGameMode::TryLoadRegionSublevel(const FNazareneRegionDefinition& Region)
{
    if (Region.StreamedLevelPackage.IsNone())
    {
        return false;
    }

    const FString PackagePath = Region.StreamedLevelPackage.ToString();
    if (!FPackageName::DoesPackageExist(PackagePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Region sublevel missing: %s"), *PackagePath);
        return false;
    }

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    UGameplayStatics::LoadStreamLevel(this, Region.StreamedLevelPackage, true, true, LatentInfo);
    LoadedRegionLevelPackage = Region.StreamedLevelPackage;
    return true;
}

void ANazareneCampaignGameMode::UnloadRegionSublevel()
{
    if (LoadedRegionLevelPackage.IsNone())
    {
        return;
    }

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    UGameplayStatics::UnloadStreamLevel(this, LoadedRegionLevelPackage, LatentInfo, true);
    LoadedRegionLevelPackage = NAME_None;
}

void ANazareneCampaignGameMode::SpawnRegionEnvironment(const FNazareneRegionDefinition& Region)
{
    const FName GalileeId(TEXT("galilee"));
    const FName DecapolisId(TEXT("decapolis"));
    const FName WildernessId(TEXT("wilderness"));
    const FName JerusalemId(TEXT("jerusalem"));
    const FName GethsemaneId(TEXT("gethsemane"));
    const FName ViaDolorosaId(TEXT("via_dolorosa"));
    const FName EmptyTombId(TEXT("empty_tomb"));

    FLinearColor SunColor = FLinearColor(1.0f, 0.95f, 0.82f);
    float SunIntensity = 10.0f;
    FLinearColor GroundTint = FLinearColor(0.35f, 0.28f, 0.19f);
    FLinearColor AccentTint = FLinearColor(0.72f, 0.63f, 0.47f);
    float HeightJitter = 80.0f;
    const TCHAR* EnvironmentMaterialPath = TEXT("/Game/Art/Materials/MI_Env_Stone.MI_Env_Stone");

    if (Region.RegionId == GalileeId)
    {
        SunColor = FLinearColor(1.0f, 0.97f, 0.88f);
        SunIntensity = 10.5f;
        GroundTint = FLinearColor(0.32f, 0.35f, 0.22f);
        AccentTint = FLinearColor(0.42f, 0.54f, 0.66f);
        HeightJitter = 120.0f;
        EnvironmentMaterialPath = TEXT("/Game/Art/Materials/MI_Env_OliveLeaf.MI_Env_OliveLeaf");
    }
    else if (Region.RegionId == DecapolisId)
    {
        SunColor = FLinearColor(0.96f, 0.90f, 0.80f);
        SunIntensity = 9.8f;
        GroundTint = FLinearColor(0.34f, 0.31f, 0.30f);
        AccentTint = FLinearColor(0.68f, 0.66f, 0.61f);
        HeightJitter = 160.0f;
        EnvironmentMaterialPath = TEXT("/Game/Art/Materials/MI_Env_Stone.MI_Env_Stone");
    }
    else if (Region.RegionId == WildernessId)
    {
        SunColor = FLinearColor(0.98f, 0.78f, 0.58f);
        SunIntensity = 11.2f;
        GroundTint = FLinearColor(0.42f, 0.29f, 0.16f);
        AccentTint = FLinearColor(0.79f, 0.58f, 0.32f);
        HeightJitter = 210.0f;
        EnvironmentMaterialPath = TEXT("/Game/Art/Materials/MI_Env_Sand.MI_Env_Sand");
    }
    else if (Region.RegionId == JerusalemId)
    {
        SunColor = FLinearColor(1.0f, 0.92f, 0.76f);
        SunIntensity = 10.1f;
        GroundTint = FLinearColor(0.39f, 0.35f, 0.27f);
        AccentTint = FLinearColor(0.74f, 0.69f, 0.56f);
        HeightJitter = 140.0f;
        EnvironmentMaterialPath = TEXT("/Game/Art/Materials/MI_Env_Stone.MI_Env_Stone");
    }
    else if (Region.RegionId == GethsemaneId)
    {
        SunColor = FLinearColor(0.52f, 0.55f, 0.72f);
        SunIntensity = 5.8f;
        GroundTint = FLinearColor(0.22f, 0.26f, 0.18f);
        AccentTint = FLinearColor(0.34f, 0.42f, 0.30f);
        HeightJitter = 90.0f;
        EnvironmentMaterialPath = TEXT("/Game/Art/Materials/MI_Env_OliveLeaf.MI_Env_OliveLeaf");
    }
    else if (Region.RegionId == ViaDolorosaId)
    {
        SunColor = FLinearColor(0.88f, 0.72f, 0.58f);
        SunIntensity = 8.4f;
        GroundTint = FLinearColor(0.44f, 0.34f, 0.26f);
        AccentTint = FLinearColor(0.62f, 0.52f, 0.40f);
        HeightJitter = 100.0f;
        EnvironmentMaterialPath = TEXT("/Game/Art/Materials/MI_Env_Stone.MI_Env_Stone");
    }
    else if (Region.RegionId == EmptyTombId)
    {
        SunColor = FLinearColor(1.0f, 1.0f, 0.96f);
        SunIntensity = 13.0f;
        GroundTint = FLinearColor(0.46f, 0.42f, 0.38f);
        AccentTint = FLinearColor(0.82f, 0.80f, 0.74f);
        HeightJitter = 60.0f;
        EnvironmentMaterialPath = TEXT("/Game/Art/Materials/MI_Env_Stone.MI_Env_Stone");
    }

    auto SpawnMeshActor = [this, EnvironmentMaterialPath](const TCHAR* MeshPath, const FVector& Location, const FRotator& Rotation, const FVector& Scale, const FLinearColor& Tint) -> AStaticMeshActor*
    {
        AStaticMeshActor* Actor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation);
        if (Actor == nullptr)
        {
            return nullptr;
        }

        const TCHAR* ResolvedMeshPath = MeshPath;
        if (FCString::Strcmp(MeshPath, TEXT("/Engine/BasicShapes/Cube.Cube")) == 0)
        {
            ResolvedMeshPath = TEXT("/Game/Art/Environment/Meshes/SM_BiblicalBlock.SM_BiblicalBlock");
        }
        else if (FCString::Strcmp(MeshPath, TEXT("/Engine/BasicShapes/Cylinder.Cylinder")) == 0)
        {
            ResolvedMeshPath = TEXT("/Game/Art/Environment/Meshes/SM_BiblicalColumn.SM_BiblicalColumn");
        }
        else if (FCString::Strcmp(MeshPath, TEXT("/Engine/BasicShapes/Cone.Cone")) == 0)
        {
            ResolvedMeshPath = TEXT("/Game/Art/Environment/Meshes/SM_BiblicalTent.SM_BiblicalTent");
        }
        else if (FCString::Strcmp(MeshPath, TEXT("/Engine/BasicShapes/Sphere.Sphere")) == 0)
        {
            ResolvedMeshPath = TEXT("/Game/Art/Environment/Meshes/SM_BiblicalCanopy.SM_BiblicalCanopy");
        }
        else if (FCString::Strcmp(MeshPath, TEXT("/Engine/BasicShapes/Plane.Plane")) == 0)
        {
            ResolvedMeshPath = TEXT("/Game/Art/Environment/Meshes/SM_BiblicalFloorPlane.SM_BiblicalFloorPlane");
        }

        UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, ResolvedMeshPath);
        if (Mesh == nullptr)
        {
            Mesh = LoadObject<UStaticMesh>(nullptr, MeshPath);
        }
        if (Mesh != nullptr)
        {
            Actor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
        }

        Actor->GetStaticMeshComponent()->SetWorldScale3D(Scale);
        UMaterialInterface* ShapeMaterial = LoadObject<UMaterialInterface>(nullptr, EnvironmentMaterialPath);
        if (ShapeMaterial == nullptr)
        {
            ShapeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
        }
        if (ShapeMaterial != nullptr)
        {
            UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(ShapeMaterial, Actor);
            if (DynamicMaterial != nullptr)
            {
                DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Tint);
                Actor->GetStaticMeshComponent()->SetMaterial(0, DynamicMaterial);
                Actor->GetStaticMeshComponent()->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(Tint.R, Tint.G, Tint.B));
            }
            else
            {
                Actor->GetStaticMeshComponent()->SetMaterial(0, ShapeMaterial);
                Actor->GetStaticMeshComponent()->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(Tint.R, Tint.G, Tint.B));
            }
        }

        RegionActors.Add(Actor);
        return Actor;
    };

    // Task 6: Helper lambda for spawning point lights in any region
    auto SpawnPointLight = [&](const FVector& Location, FLinearColor Color, float Intensity, float Radius) -> APointLight*
    {
        APointLight* Light = GetWorld()->SpawnActor<APointLight>(APointLight::StaticClass(), FTransform(FRotator::ZeroRotator, Location));
        if (Light && Light->PointLightComponent)
        {
            Light->PointLightComponent->SetLightColor(Color);
            Light->PointLightComponent->SetIntensity(Intensity);
            Light->PointLightComponent->SetAttenuationRadius(Radius);
        }
        RegionActors.Add(Light);
        return Light;
    };

    auto SpawnPathSegment = [&SpawnMeshActor, &GroundTint](float Y, float HalfWidthScale, float ThicknessScale, float Elevation)
    {
        SpawnMeshActor(
            TEXT("/Engine/BasicShapes/Cube.Cube"),
            FVector(0.0f, Y, Elevation),
            FRotator::ZeroRotator,
            FVector(HalfWidthScale, 7.4f, ThicknessScale),
            GroundTint * 1.04f
        );
    };

    auto SpawnColonnade = [&SpawnMeshActor, &AccentTint](float StartY, int32 Count, float Spacing, float LateralOffset, float HeightScale, float RadiusScale)
    {
        for (int32 Index = 0; Index < Count; ++Index)
        {
            const float Y = StartY + float(Index) * Spacing;
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(-LateralOffset, Y, 140.0f), FRotator::ZeroRotator, FVector(RadiusScale, RadiusScale, HeightScale), AccentTint);
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(LateralOffset, Y, 140.0f), FRotator::ZeroRotator, FVector(RadiusScale, RadiusScale, HeightScale), AccentTint);
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0.0f, Y, 310.0f), FRotator::ZeroRotator, FVector(LateralOffset * 0.012f, 0.3f, 0.2f), AccentTint * 0.9f);
        }
    };

    auto SpawnOliveTree = [&SpawnMeshActor](const FVector& Root, const FLinearColor& TrunkTint, const FLinearColor& LeafTint, float CanopyScale)
    {
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), Root + FVector(0.0f, 0.0f, 80.0f), FRotator::ZeroRotator, FVector(0.18f, 0.18f, 1.2f), TrunkTint);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Sphere.Sphere"), Root + FVector(-35.0f, 10.0f, 200.0f), FRotator::ZeroRotator, FVector(CanopyScale, CanopyScale, CanopyScale * 0.8f), LeafTint);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Sphere.Sphere"), Root + FVector(30.0f, -25.0f, 190.0f), FRotator::ZeroRotator, FVector(CanopyScale * 0.9f, CanopyScale * 0.9f, CanopyScale * 0.72f), LeafTint * 0.95f);
    };

    ADirectionalLight* Sun = GetWorld()->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), FVector(0.0f, 0.0f, 600.0f), FRotator(-38.0f, -32.0f, 0.0f));
    if (Sun != nullptr)
    {
        Sun->GetLightComponent()->SetIntensity(SunIntensity);
        Sun->GetLightComponent()->SetTemperature(6000.0f);
        Sun->GetLightComponent()->SetLightColor(SunColor);
        RegionActors.Add(Sun);
    }

    ASkyLight* SkyLight = GetWorld()->SpawnActor<ASkyLight>(ASkyLight::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    if (SkyLight != nullptr)
    {
        SkyLight->GetLightComponent()->SetIntensity(0.95f);
        SkyLight->GetLightComponent()->SetLightColor(SunColor * 0.55f);
        RegionActors.Add(SkyLight);
    }

    AExponentialHeightFog* HeightFog = GetWorld()->SpawnActor<AExponentialHeightFog>(AExponentialHeightFog::StaticClass(), FVector(0.0f, 0.0f, -120.0f), FRotator::ZeroRotator);
    if (HeightFog != nullptr)
    {
        HeightFog->GetComponent()->SetFogDensity(0.014f);
        HeightFog->GetComponent()->SetFogHeightFalloff(0.24f);
        HeightFog->GetComponent()->SetFogInscatteringColor(SunColor * 0.35f);
        RegionActors.Add(HeightFog);
    }

    APostProcessVolume* PostProcessVolume = GetWorld()->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    if (PostProcessVolume != nullptr)
    {
        PostProcessVolume->bUnbound = true;
        PostProcessVolume->BlendWeight = 0.72f;
        PostProcessVolume->Settings.bOverride_ColorSaturation = true;
        PostProcessVolume->Settings.ColorSaturation = FVector4(0.96f, 0.94f, 0.92f, 1.0f);
        PostProcessVolume->Settings.bOverride_ColorContrast = true;
        PostProcessVolume->Settings.ColorContrast = FVector4(1.05f, 1.05f, 1.05f, 1.0f);
        PostProcessVolume->Settings.bOverride_AutoExposureMethod = true;
        PostProcessVolume->Settings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;
        PostProcessVolume->Settings.bOverride_AutoExposureBias = true;
        PostProcessVolume->Settings.AutoExposureBias = 0.25f;
        RegionActors.Add(PostProcessVolume);
    }

    // Task 6: Expanded 3x3 ground plane grid for all regions
    for (int32 GridX = -1; GridX <= 1; ++GridX)
    {
        for (int32 GridY = -1; GridY <= 1; ++GridY)
        {
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"),
                FVector(float(GridX) * 3200.0f, float(GridY) * 3200.0f, -60.0f),
                FRotator::ZeroRotator,
                FVector(128.0f, 128.0f, 1.2f),
                GroundTint);
        }
    }

    for (int32 Segment = 0; Segment < 9; ++Segment)
    {
        const float Y = 340.0f - float(Segment) * 520.0f;
        const float Width = 11.0f + float((Segment % 3) == 0 ? 2 : 0);
        const float Thickness = 0.34f + float(Segment % 2) * 0.05f;
        SpawnPathSegment(Y, Width, Thickness, -42.0f);
    }

    // Sanctuary near spawn point.
    SpawnColonnade(120.0f, 3, 220.0f, 380.0f, 2.2f, 0.24f);
    SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0.0f, 430.0f, 60.0f), FRotator::ZeroRotator, FVector(1.7f, 1.3f, 1.0f), AccentTint * 0.86f);
    SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0.0f, 740.0f, 190.0f), FRotator::ZeroRotator, FVector(2.5f, 0.4f, 3.4f), AccentTint * 0.92f);

    FVector BossArenaLocation(0.0f, -2200.0f, -45.0f);
    for (const FNazareneEnemySpawnDefinition& Spec : Region.Enemies)
    {
        if (Spec.SpawnId == Region.BossSpawnId)
        {
            BossArenaLocation = FVector(Spec.Location.X, Spec.Location.Y, -45.0f);
            break;
        }
    }

    SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), BossArenaLocation, FRotator::ZeroRotator, FVector(10.0f, 10.0f, 0.2f), AccentTint);

    const float RingRadius = 1960.0f;
    for (int32 Index = 0; Index < 10; ++Index)
    {
        const float Angle = FMath::DegreesToRadians(float(Index) * 36.0f);
        const FVector RingLocation(
            FMath::Cos(Angle) * RingRadius,
            FMath::Sin(Angle) * RingRadius - 250.0f,
            70.0f + FMath::Sin(Angle * 3.0f) * HeightJitter
        );
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), RingLocation, FRotator::ZeroRotator, FVector(0.75f, 0.75f, 2.8f), AccentTint * 0.85f);
    }

    for (int32 Index = 0; Index < 14; ++Index)
    {
        const float X = -2100.0f + float(Index) * 320.0f;
        const float Y = -2800.0f + float((Index % 4) * 180);
        const float Z = -10.0f + float((Index % 3) * 40);
        const FVector Scale = FVector(1.6f + float(Index % 2) * 0.4f, 1.1f + float(Index % 3) * 0.2f, 1.0f + float(Index % 4) * 0.5f);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(X, Y, Z), FRotator(0.0f, float(Index) * 9.0f, 0.0f), Scale, GroundTint * 0.92f);
    }

    // -----------------------------------------------------------------------
    // Per-region environment details (Tasks 5 + 6)
    // -----------------------------------------------------------------------

    if (Region.RegionId == GalileeId)
    {
        for (int32 Index = 0; Index < 7; ++Index)
        {
            const float X = -1800.0f + float(Index) * 620.0f;
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Sphere.Sphere"), FVector(X, 1950.0f, -15.0f), FRotator::ZeroRotator, FVector(1.4f, 1.4f, 0.28f), FLinearColor(0.23f, 0.35f, 0.48f));
        }
        for (int32 Index = 0; Index < 10; ++Index)
        {
            const FVector Root(-1850.0f + float(Index) * 410.0f, 1080.0f + float((Index % 2) * 340.0f), 0.0f);
            SpawnOliveTree(Root, FLinearColor(0.31f, 0.23f, 0.16f), FLinearColor(0.24f, 0.37f, 0.20f), 0.72f);
        }
        // Task 6: Galilee point lights
        SpawnPointLight(Region.PrayerSiteLocation + FVector(0.0f, 0.0f, 250.0f), FLinearColor(1.0f, 0.90f, 0.65f), 600.0f, 900.0f);
        SpawnPointLight(BossArenaLocation + FVector(0.0f, 0.0f, 300.0f), FLinearColor(0.45f, 0.55f, 0.82f), 400.0f, 800.0f);
        SpawnPointLight(FVector(-350.0f, 250.0f, 260.0f), FLinearColor(1.0f, 0.95f, 0.85f), 350.0f, 700.0f);
        SpawnPointLight(FVector(350.0f, 280.0f, 260.0f), FLinearColor(1.0f, 0.95f, 0.85f), 350.0f, 700.0f);
        // Task 6: Galilee pottery objects (small cubes with sand tint)
        const FLinearColor PotteryTint(0.62f, 0.50f, 0.34f);
        const FVector PotteryPositions[] = {
            FVector(-600.0f, 350.0f, 10.0f), FVector(-420.0f, 520.0f, 10.0f), FVector(580.0f, 400.0f, 10.0f),
            FVector(320.0f, 600.0f, 10.0f), FVector(-150.0f, 800.0f, 10.0f), FVector(200.0f, 900.0f, 10.0f),
            FVector(-800.0f, 750.0f, 10.0f), FVector(700.0f, 700.0f, 10.0f), FVector(-100.0f, 1100.0f, 10.0f),
            FVector(450.0f, 1200.0f, 10.0f)
        };
        for (const FVector& Pos : PotteryPositions)
        {
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), Pos, FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f), FVector(0.25f, 0.25f, 0.35f), PotteryTint);
        }
    }
    else if (Region.RegionId == DecapolisId)
    {
        for (int32 Index = 0; Index < 12; ++Index)
        {
            const float X = -1650.0f + float(Index) * 300.0f;
            const float Y = 1300.0f + float((Index % 2) * 260.0f);
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(X, Y, 120.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.45f, 0.45f, 3.2f), AccentTint);
        }
        SpawnColonnade(560.0f, 6, 250.0f, 700.0f, 3.2f, 0.28f);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0.0f, 540.0f, 380.0f), FRotator::ZeroRotator, FVector(8.0f, 0.55f, 0.22f), AccentTint * 0.92f);
        // Task 6: Decapolis point lights (torch-like orange at columns)
        SpawnPointLight(FVector(-700.0f, 560.0f, 320.0f), FLinearColor(1.0f, 0.68f, 0.28f), 500.0f, 900.0f);
        SpawnPointLight(FVector(700.0f, 560.0f, 320.0f), FLinearColor(1.0f, 0.68f, 0.28f), 500.0f, 900.0f);
        SpawnPointLight(FVector(-700.0f, 1060.0f, 320.0f), FLinearColor(1.0f, 0.70f, 0.30f), 500.0f, 900.0f);
        SpawnPointLight(FVector(700.0f, 1060.0f, 320.0f), FLinearColor(1.0f, 0.70f, 0.30f), 500.0f, 900.0f);
        SpawnPointLight(FVector(-700.0f, 1560.0f, 320.0f), FLinearColor(1.0f, 0.65f, 0.26f), 450.0f, 850.0f);
        SpawnPointLight(FVector(700.0f, 1560.0f, 320.0f), FLinearColor(1.0f, 0.65f, 0.26f), 450.0f, 850.0f);
        // Task 6: Decapolis additional colonnade formation (8 columns flanking the path)
        SpawnColonnade(1600.0f, 4, 300.0f, 450.0f, 3.0f, 0.26f);
    }
    else if (Region.RegionId == WildernessId)
    {
        for (int32 Index = 0; Index < 16; ++Index)
        {
            const float X = -2300.0f + float(Index) * 300.0f;
            const float Y = -500.0f + float((Index % 5) * 280.0f);
            const float Z = 80.0f + float((Index % 4) * 95.0f);
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cone.Cone"), FVector(X, Y, Z), FRotator(0.0f, float(Index) * 14.0f, 0.0f), FVector(0.9f, 0.9f, 2.0f), AccentTint * 0.88f);
        }
        for (int32 Index = 0; Index < 10; ++Index)
        {
            const float X = -1750.0f + float(Index) * 370.0f;
            const float Y = 1250.0f + float((Index % 2) * 300.0f);
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(X, Y, 90.0f), FRotator(0.0f, float(Index) * 6.0f, 0.0f), FVector(1.4f, 1.0f, 0.05f), FLinearColor(0.63f, 0.48f, 0.26f));
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(X - 210.0f, Y, 70.0f), FRotator::ZeroRotator, FVector(0.06f, 0.06f, 0.9f), AccentTint * 0.8f);
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(X + 210.0f, Y, 70.0f), FRotator::ZeroRotator, FVector(0.06f, 0.06f, 0.9f), AccentTint * 0.8f);
        }
        // Task 6: Wilderness dim red/orange point lights
        SpawnPointLight(FVector(-800.0f, -200.0f, 200.0f), FLinearColor(0.92f, 0.42f, 0.18f), 280.0f, 700.0f);
        SpawnPointLight(FVector(800.0f, -600.0f, 200.0f), FLinearColor(0.88f, 0.38f, 0.15f), 260.0f, 700.0f);
        SpawnPointLight(FVector(0.0f, 600.0f, 200.0f), FLinearColor(0.85f, 0.40f, 0.20f), 300.0f, 750.0f);
        SpawnPointLight(BossArenaLocation + FVector(0.0f, 0.0f, 280.0f), FLinearColor(0.95f, 0.35f, 0.12f), 350.0f, 800.0f);
        // Task 6: Wilderness rock formations (irregular cubes with stone material)
        const FLinearColor RockTint(0.52f, 0.44f, 0.36f);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-1800.0f, 200.0f, 40.0f), FRotator(0.0f, 22.0f, 8.0f), FVector(2.8f, 1.6f, 2.0f), RockTint);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(1600.0f, -400.0f, 60.0f), FRotator(0.0f, -15.0f, 5.0f), FVector(2.2f, 1.8f, 2.4f), RockTint * 0.92f);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-500.0f, -1800.0f, 30.0f), FRotator(0.0f, 40.0f, 0.0f), FVector(3.0f, 2.0f, 1.6f), RockTint * 1.05f);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(1200.0f, 400.0f, 50.0f), FRotator(0.0f, -35.0f, 4.0f), FVector(1.8f, 2.4f, 1.9f), RockTint);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-1400.0f, -1200.0f, 20.0f), FRotator(0.0f, 55.0f, 0.0f), FVector(2.5f, 1.4f, 2.2f), RockTint * 0.88f);
    }
    else if (Region.RegionId == JerusalemId)
    {
        for (int32 Row = 0; Row < 2; ++Row)
        {
            for (int32 Column = 0; Column < 6; ++Column)
            {
                const float X = -950.0f + float(Column) * 380.0f;
                const float Y = 980.0f + float(Row) * 450.0f;
                SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(X, Y, 220.0f), FRotator::ZeroRotator, FVector(0.45f, 0.45f, 4.6f), AccentTint * 1.04f);
            }
        }
        SpawnColonnade(420.0f, 7, 210.0f, 560.0f, 3.8f, 0.3f);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0.0f, 420.0f, 440.0f), FRotator::ZeroRotator, FVector(8.0f, 0.48f, 0.24f), AccentTint * 0.96f);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0.0f, 360.0f, 180.0f), FRotator::ZeroRotator, FVector(2.5f, 0.8f, 3.8f), AccentTint * 0.86f);
        // Task 6: Jerusalem warm lantern point lights
        SpawnPointLight(FVector(-560.0f, 420.0f, 380.0f), FLinearColor(1.0f, 0.88f, 0.52f), 400.0f, 800.0f);
        SpawnPointLight(FVector(560.0f, 420.0f, 380.0f), FLinearColor(1.0f, 0.88f, 0.52f), 400.0f, 800.0f);
        SpawnPointLight(FVector(-560.0f, 840.0f, 380.0f), FLinearColor(1.0f, 0.85f, 0.50f), 380.0f, 780.0f);
        SpawnPointLight(FVector(560.0f, 840.0f, 380.0f), FLinearColor(1.0f, 0.85f, 0.50f), 380.0f, 780.0f);
        SpawnPointLight(FVector(0.0f, 0.0f, 280.0f), FLinearColor(1.0f, 0.90f, 0.55f), 450.0f, 900.0f);
        SpawnPointLight(FVector(-560.0f, 1260.0f, 380.0f), FLinearColor(1.0f, 0.82f, 0.48f), 360.0f, 760.0f);
        SpawnPointLight(FVector(560.0f, 1260.0f, 380.0f), FLinearColor(1.0f, 0.82f, 0.48f), 360.0f, 760.0f);
        SpawnPointLight(BossArenaLocation + FVector(0.0f, 0.0f, 300.0f), FLinearColor(1.0f, 0.90f, 0.56f), 500.0f, 950.0f);
        // Task 6: Jerusalem market stalls (cube + cone pairs)
        const FLinearColor StallWood(0.48f, 0.36f, 0.22f);
        const FLinearColor StallFabric(0.72f, 0.58f, 0.38f);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-800.0f, 700.0f, 60.0f), FRotator::ZeroRotator, FVector(1.2f, 0.8f, 0.9f), StallWood);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cone.Cone"), FVector(-800.0f, 700.0f, 180.0f), FRotator::ZeroRotator, FVector(1.0f, 1.0f, 0.6f), StallFabric);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(800.0f, 750.0f, 60.0f), FRotator::ZeroRotator, FVector(1.2f, 0.8f, 0.9f), StallWood);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cone.Cone"), FVector(800.0f, 750.0f, 180.0f), FRotator::ZeroRotator, FVector(1.0f, 1.0f, 0.6f), StallFabric);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-800.0f, 1200.0f, 60.0f), FRotator::ZeroRotator, FVector(1.0f, 0.7f, 0.8f), StallWood);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cone.Cone"), FVector(-800.0f, 1200.0f, 160.0f), FRotator::ZeroRotator, FVector(0.9f, 0.9f, 0.55f), StallFabric);
        // Task 6: Jerusalem grand entrance archway
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(-380.0f, 1800.0f, 180.0f), FRotator::ZeroRotator, FVector(0.4f, 0.4f, 4.0f), AccentTint * 1.06f);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(380.0f, 1800.0f, 180.0f), FRotator::ZeroRotator, FVector(0.4f, 0.4f, 4.0f), AccentTint * 1.06f);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0.0f, 1800.0f, 520.0f), FRotator::ZeroRotator, FVector(5.2f, 0.6f, 0.5f), AccentTint);
    }
    // -----------------------------------------------------------------------
    // Task 5: Chapter 5 - Gethsemane (olive grove at night)
    // -----------------------------------------------------------------------
    else if (Region.RegionId == GethsemaneId)
    {
        // Override post-process for heavier nighttime blue tint / lower exposure
        if (PostProcessVolume != nullptr)
        {
            PostProcessVolume->BlendWeight = 0.85f;
            PostProcessVolume->Settings.ColorSaturation = FVector4(0.82f, 0.86f, 1.06f, 1.0f);
            PostProcessVolume->Settings.AutoExposureBias = -0.35f;
        }

        // Override fog for moonlit atmosphere
        if (HeightFog != nullptr)
        {
            HeightFog->GetComponent()->SetFogDensity(0.022f);
            HeightFog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.18f, 0.20f, 0.36f));
        }

        // 28 olive trees arranged in organic clusters
        const FLinearColor OliveTrunk(0.26f, 0.20f, 0.14f);
        const FLinearColor OliveLeaf(0.16f, 0.28f, 0.14f);

        // Cluster 1: northwest grove
        const FVector Cluster1[] = {
            FVector(-1200.0f, 400.0f, 0.0f), FVector(-1050.0f, 550.0f, 0.0f), FVector(-1350.0f, 280.0f, 0.0f),
            FVector(-1180.0f, 700.0f, 0.0f), FVector(-1400.0f, 600.0f, 0.0f), FVector(-1000.0f, 350.0f, 0.0f),
            FVector(-1280.0f, 800.0f, 0.0f)
        };
        for (const FVector& Root : Cluster1)
        {
            SpawnOliveTree(Root, OliveTrunk, OliveLeaf, 0.80f + FMath::FRandRange(-0.08f, 0.08f));
        }

        // Cluster 2: northeast grove
        const FVector Cluster2[] = {
            FVector(1100.0f, 350.0f, 0.0f), FVector(1250.0f, 500.0f, 0.0f), FVector(950.0f, 600.0f, 0.0f),
            FVector(1300.0f, 300.0f, 0.0f), FVector(1150.0f, 720.0f, 0.0f), FVector(1050.0f, 200.0f, 0.0f),
            FVector(1350.0f, 650.0f, 0.0f)
        };
        for (const FVector& Root : Cluster2)
        {
            SpawnOliveTree(Root, OliveTrunk, OliveLeaf * 0.92f, 0.76f + FMath::FRandRange(-0.06f, 0.06f));
        }

        // Cluster 3: south perimeter
        const FVector Cluster3[] = {
            FVector(-600.0f, -800.0f, 0.0f), FVector(-200.0f, -950.0f, 0.0f), FVector(250.0f, -880.0f, 0.0f),
            FVector(650.0f, -750.0f, 0.0f), FVector(-400.0f, -1100.0f, 0.0f), FVector(100.0f, -1050.0f, 0.0f),
            FVector(500.0f, -1000.0f, 0.0f)
        };
        for (const FVector& Root : Cluster3)
        {
            SpawnOliveTree(Root, OliveTrunk * 0.95f, OliveLeaf * 0.88f, 0.72f + FMath::FRandRange(-0.05f, 0.05f));
        }

        // Scattered singles (7 more to reach ~28)
        const FVector Singles[] = {
            FVector(-500.0f, 1000.0f, 0.0f), FVector(500.0f, 1100.0f, 0.0f), FVector(-800.0f, -200.0f, 0.0f),
            FVector(850.0f, -300.0f, 0.0f), FVector(0.0f, 1400.0f, 0.0f), FVector(-300.0f, 1500.0f, 0.0f),
            FVector(400.0f, 1350.0f, 0.0f)
        };
        for (const FVector& Root : Singles)
        {
            SpawnOliveTree(Root, OliveTrunk, OliveLeaf * 0.96f, 0.68f);
        }

        // Stone wall perimeter (14 cube segments in loose rectangle)
        const FLinearColor WallTint(0.38f, 0.35f, 0.30f);
        const FVector WallSegments[] = {
            FVector(-1600.0f, -400.0f, 30.0f), FVector(-1600.0f, 100.0f, 30.0f), FVector(-1600.0f, 600.0f, 30.0f),
            FVector(-1600.0f, 1100.0f, 30.0f), FVector(-1100.0f, 1500.0f, 30.0f), FVector(-400.0f, 1500.0f, 30.0f),
            FVector(300.0f, 1500.0f, 30.0f), FVector(1000.0f, 1500.0f, 30.0f), FVector(1600.0f, 1100.0f, 30.0f),
            FVector(1600.0f, 600.0f, 30.0f), FVector(1600.0f, 100.0f, 30.0f), FVector(1600.0f, -400.0f, 30.0f),
            FVector(800.0f, -1300.0f, 30.0f), FVector(-800.0f, -1300.0f, 30.0f)
        };
        for (const FVector& WallPos : WallSegments)
        {
            const float WallYaw = FMath::Atan2(WallPos.Y, WallPos.X) * (180.0f / PI);
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), WallPos, FRotator(0.0f, WallYaw, 0.0f), FVector(3.0f, 0.5f, 1.2f), WallTint);
        }

        // Circular clearing around the prayer site (4 floor planes)
        const FLinearColor ClearingTint = GroundTint * 1.15f;
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Plane.Plane"), Region.PrayerSiteLocation + FVector(-120.0f, -120.0f, 2.0f), FRotator::ZeroRotator, FVector(3.0f, 3.0f, 1.0f), ClearingTint);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Plane.Plane"), Region.PrayerSiteLocation + FVector(120.0f, -120.0f, 2.0f), FRotator::ZeroRotator, FVector(3.0f, 3.0f, 1.0f), ClearingTint);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Plane.Plane"), Region.PrayerSiteLocation + FVector(-120.0f, 120.0f, 2.0f), FRotator::ZeroRotator, FVector(3.0f, 3.0f, 1.0f), ClearingTint);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Plane.Plane"), Region.PrayerSiteLocation + FVector(120.0f, 120.0f, 2.0f), FRotator::ZeroRotator, FVector(3.0f, 3.0f, 1.0f), ClearingTint);

        // 8 warm amber point lights scattered among trees for moonlit candle effect
        SpawnPointLight(FVector(-1100.0f, 480.0f, 160.0f), FLinearColor(1.0f, 0.82f, 0.46f), 200.0f, 800.0f);
        SpawnPointLight(FVector(1150.0f, 420.0f, 160.0f), FLinearColor(1.0f, 0.80f, 0.44f), 200.0f, 800.0f);
        SpawnPointLight(FVector(-300.0f, -900.0f, 140.0f), FLinearColor(1.0f, 0.85f, 0.50f), 180.0f, 750.0f);
        SpawnPointLight(FVector(400.0f, -850.0f, 140.0f), FLinearColor(1.0f, 0.84f, 0.48f), 180.0f, 750.0f);
        SpawnPointLight(FVector(-600.0f, 1000.0f, 150.0f), FLinearColor(1.0f, 0.78f, 0.42f), 190.0f, 780.0f);
        SpawnPointLight(FVector(550.0f, 1100.0f, 150.0f), FLinearColor(1.0f, 0.82f, 0.45f), 190.0f, 780.0f);
        SpawnPointLight(Region.PrayerSiteLocation + FVector(150.0f, 0.0f, 180.0f), FLinearColor(1.0f, 0.88f, 0.52f), 220.0f, 850.0f);
        SpawnPointLight(Region.PrayerSiteLocation + FVector(-150.0f, 0.0f, 180.0f), FLinearColor(1.0f, 0.86f, 0.50f), 220.0f, 850.0f);
    }
    // -----------------------------------------------------------------------
    // Task 5: Chapter 6 - Via Dolorosa (narrow street / corridor)
    // -----------------------------------------------------------------------
    else if (Region.RegionId == ViaDolorosaId)
    {
        // Override fog for heavy dusty atmosphere
        if (HeightFog != nullptr)
        {
            HeightFog->GetComponent()->SetFogDensity(0.028f);
            HeightFog->GetComponent()->SetFogHeightFalloff(0.18f);
            HeightFog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.52f, 0.38f, 0.26f));
        }

        // Two long rows of tall building walls (corridor, 800 units apart)
        const FLinearColor WallTint(0.52f, 0.44f, 0.36f);
        for (int32 Seg = 0; Seg < 8; ++Seg)
        {
            const float Y = 800.0f - float(Seg) * 600.0f;
            // Left wall
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-400.0f, Y, 200.0f), FRotator::ZeroRotator, FVector(2.0f, 8.0f, 3.0f), WallTint);
            // Right wall
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(400.0f, Y, 200.0f), FRotator::ZeroRotator, FVector(2.0f, 8.0f, 3.0f), WallTint * 0.94f);
        }

        // 5 archway gates (two columns + lintel) along the path
        const FLinearColor ArchTint = AccentTint * 1.08f;
        const float ArchY[] = { 400.0f, -200.0f, -800.0f, -1400.0f, -2000.0f };
        for (int32 Arch = 0; Arch < 5; ++Arch)
        {
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(-300.0f, ArchY[Arch], 180.0f), FRotator::ZeroRotator, FVector(0.35f, 0.35f, 3.6f), ArchTint);
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(300.0f, ArchY[Arch], 180.0f), FRotator::ZeroRotator, FVector(0.35f, 0.35f, 3.6f), ArchTint);
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0.0f, ArchY[Arch], 480.0f), FRotator::ZeroRotator, FVector(4.2f, 0.5f, 0.4f), ArchTint * 0.92f);
            // Orange point light at each archway
            SpawnPointLight(FVector(0.0f, ArchY[Arch], 350.0f), FLinearColor(1.0f, 0.72f, 0.32f), 400.0f, 850.0f);
        }

        // Market stall obstacles along the sides (small cube + tent/cone pairs)
        const FLinearColor StallWood(0.44f, 0.32f, 0.20f);
        const FLinearColor StallFabric(0.68f, 0.52f, 0.34f);
        const FVector StallPositions[] = {
            FVector(-300.0f, 100.0f, 40.0f), FVector(280.0f, -500.0f, 40.0f),
            FVector(-280.0f, -1100.0f, 40.0f), FVector(300.0f, -1700.0f, 40.0f),
            FVector(-260.0f, -2300.0f, 40.0f), FVector(280.0f, 700.0f, 40.0f)
        };
        for (const FVector& StallPos : StallPositions)
        {
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), StallPos, FRotator::ZeroRotator, FVector(0.8f, 0.6f, 0.7f), StallWood);
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cone.Cone"), StallPos + FVector(0.0f, 0.0f, 110.0f), FRotator::ZeroRotator, FVector(0.7f, 0.7f, 0.5f), StallFabric);
        }

        // Staircases: 4 sets of stacked thin cubes creating elevation changes
        const FLinearColor StairTint = GroundTint * 1.1f;
        auto SpawnStaircase = [&SpawnMeshActor, &StairTint](const FVector& Base, int32 Steps)
        {
            for (int32 Step = 0; Step < Steps; ++Step)
            {
                SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"),
                    Base + FVector(0.0f, float(Step) * 40.0f, float(Step) * 20.0f),
                    FRotator::ZeroRotator,
                    FVector(3.0f, 0.4f, 0.15f),
                    StairTint);
            }
        };
        SpawnStaircase(FVector(0.0f, -300.0f, -30.0f), 5);
        SpawnStaircase(FVector(0.0f, -1000.0f, -30.0f), 4);
        SpawnStaircase(FVector(0.0f, -1600.0f, -30.0f), 6);
        SpawnStaircase(FVector(0.0f, -2200.0f, -30.0f), 4);

        // Boss arena widening at the end (open square)
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-700.0f, -2500.0f, 200.0f), FRotator::ZeroRotator, FVector(1.0f, 5.0f, 3.0f), WallTint * 0.88f);
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(700.0f, -2500.0f, 200.0f), FRotator::ZeroRotator, FVector(1.0f, 5.0f, 3.0f), WallTint * 0.88f);
    }
    // -----------------------------------------------------------------------
    // Task 5: Chapter 7 - Empty Tomb (cave / garden)
    // -----------------------------------------------------------------------
    else if (Region.RegionId == EmptyTombId)
    {
        // Override post-process for bright golden dawn
        if (PostProcessVolume != nullptr)
        {
            PostProcessVolume->BlendWeight = 0.60f;
            PostProcessVolume->Settings.ColorSaturation = FVector4(1.02f, 1.0f, 0.94f, 1.0f);
            PostProcessVolume->Settings.AutoExposureBias = 0.60f;
        }

        // Minimal fog for open feeling
        if (HeightFog != nullptr)
        {
            HeightFog->GetComponent()->SetFogDensity(0.006f);
            HeightFog->GetComponent()->SetFogHeightFalloff(0.32f);
            HeightFog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.92f, 0.88f, 0.72f));
        }

        // Tomb entrance: large semicircle of cube meshes forming cave mouth
        const FLinearColor CaveTint(0.40f, 0.36f, 0.32f);
        for (int32 Index = 0; Index < 9; ++Index)
        {
            const float AngleDeg = 180.0f + float(Index) * (180.0f / 8.0f);
            const float Rad = FMath::DegreesToRadians(AngleDeg);
            const FVector CavePos(
                FMath::Cos(Rad) * 600.0f,
                -1800.0f + FMath::Sin(Rad) * 600.0f,
                60.0f + FMath::Abs(FMath::Sin(Rad)) * 200.0f
            );
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), CavePos, FRotator(0.0f, AngleDeg, 0.0f),
                FVector(2.0f, 1.5f, 2.0f + FMath::Abs(FMath::Sin(Rad)) * 1.5f), CaveTint);
        }

        // Prominent tomb stone (large cube "rolled aside" near cave entrance)
        SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(500.0f, -1800.0f, 30.0f), FRotator(0.0f, 25.0f, 12.0f), FVector(2.2f, 2.2f, 2.0f), CaveTint * 0.85f);

        // Stone path leading from spawn to tomb
        const FLinearColor PathTint = GroundTint * 1.2f;
        for (int32 Seg = 0; Seg < 12; ++Seg)
        {
            const float Y = 200.0f - float(Seg) * 180.0f;
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Cube.Cube"),
                FVector(FMath::Sin(float(Seg) * 0.4f) * 60.0f, Y, -45.0f),
                FRotator::ZeroRotator,
                FVector(2.5f, 1.5f, 0.15f),
                PathTint);
        }

        // Garden elements: small sphere meshes (flowers/bushes) in clusters
        const FLinearColor FlowerTint1(0.78f, 0.42f, 0.58f);
        const FLinearColor FlowerTint2(0.92f, 0.88f, 0.52f);
        const FLinearColor BushTint(0.28f, 0.48f, 0.26f);
        const FVector GardenPositions[] = {
            FVector(-400.0f, -600.0f, 10.0f), FVector(-600.0f, -400.0f, 10.0f), FVector(350.0f, -500.0f, 10.0f),
            FVector(600.0f, -350.0f, 10.0f), FVector(-500.0f, 200.0f, 10.0f), FVector(-700.0f, 500.0f, 10.0f),
            FVector(500.0f, 300.0f, 10.0f), FVector(700.0f, 600.0f, 10.0f), FVector(-250.0f, -1200.0f, 10.0f),
            FVector(300.0f, -1100.0f, 10.0f), FVector(-500.0f, -1500.0f, 10.0f), FVector(450.0f, -1400.0f, 10.0f)
        };
        for (int32 Index = 0; Index < 12; ++Index)
        {
            const FLinearColor& Tint = (Index % 3 == 0) ? FlowerTint1 : ((Index % 3 == 1) ? FlowerTint2 : BushTint);
            const float Scale = (Index % 3 == 2) ? 0.6f : 0.3f;
            SpawnMeshActor(TEXT("/Engine/BasicShapes/Sphere.Sphere"), GardenPositions[Index], FRotator::ZeroRotator,
                FVector(Scale, Scale, Scale * 0.8f), Tint);
        }

        // Bright golden dawn point lights (sparse, open feeling)
        SpawnPointLight(FVector(0.0f, -1800.0f, 400.0f), FLinearColor(1.0f, 0.96f, 0.80f), 800.0f, 1200.0f);
        SpawnPointLight(FVector(-300.0f, -600.0f, 200.0f), FLinearColor(1.0f, 0.94f, 0.78f), 350.0f, 800.0f);
        SpawnPointLight(FVector(300.0f, -500.0f, 200.0f), FLinearColor(1.0f, 0.94f, 0.78f), 350.0f, 800.0f);
        SpawnPointLight(Region.PrayerSiteLocation + FVector(0.0f, 0.0f, 250.0f), FLinearColor(1.0f, 0.98f, 0.86f), 500.0f, 1000.0f);
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
        ConfigureEnemyBehaviorTree(Enemy);
        Enemy->OnRedeemed.AddDynamic(this, &ANazareneCampaignGameMode::HandleEnemyRedeemed);

        RegionActors.Add(Enemy);
        EnemyBySpawnId.Add(Spec.SpawnId, Enemy);
        if (Spec.SpawnId == Region.BossSpawnId)
        {
            BossEnemy = Enemy;
            BossEnemy->OnPhaseChanged.AddDynamic(this, &ANazareneCampaignGameMode::HandleReinforcementWave);
        }
    }

    for (const FNazareneNPCSpawnDefinition& NPCSpec : Region.NPCs)
    {
        ANazareneNPC* NPC = GetWorld()->SpawnActor<ANazareneNPC>(ANazareneNPC::StaticClass(), NPCSpec.Location, FRotator::ZeroRotator);
        if (NPC != nullptr)
        {
            NPC->NPCName = NPCSpec.NPCName;
            NPC->CharacterSlug = NPCSpec.CharacterSlug;
            NPC->IdleGreeting = NPCSpec.IdleGreeting;
            NPC->DialogueLines = NPCSpec.DialogueLines;
            RegionActors.Add(NPC);
        }
    }

    DeferredWaves.Empty();
    for (const FNazareneEncounterWave& Wave : Region.EncounterWaves)
    {
        if (Wave.Trigger == ENazareneSpawnTrigger::OnRegionLoad)
        {
            SpawnWaveEnemies(Wave);
        }
        else
        {
            DeferredWaves.Add(Wave);
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
        ChapterStage = ENazareneChapterStage::Completed;
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

void ANazareneCampaignGameMode::NotifyPrayerSiteRest(FName SiteId)
{
    if (!Regions.IsValidIndex(RegionIndex))
    {
        return;
    }

    const FNazareneRegionDefinition& Region = Regions[RegionIndex];
    if (SiteId != Region.PrayerSiteId)
    {
        return;
    }

    const FName ConsecratedFlag(*FString::Printf(TEXT("site_%s_consecrated"), *SiteId.ToString()));
    if (Session && Session->IsFlagSet(ConsecratedFlag))
    {
        bPrayerSiteConsecrated = true;
        return;
    }

    bPrayerSiteConsecrated = true;
    if (Session)
    {
        Session->MarkFlag(ConsecratedFlag);
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
        {
            HUD->ShowMessage(TEXT("Prayer Site consecrated. The guardian now stands revealed."), 5.0f);
        }
    }

    UpdateChapterStageFromState();
    UpdateHUDForRegion(Region, bRegionCompleted);
    SetMusicState(ENazareneMusicState::Tension, true);
    SaveCheckpoint();
    CheckDeferredWaves(ENazareneSpawnTrigger::OnPrayerSiteConsecrated);
}

void ANazareneCampaignGameMode::NotifyPlayerDefeated()
{
    if (!Regions.IsValidIndex(RegionIndex))
    {
        return;
    }

    EnsureRetryCounterForCurrentRegion();
    const int32 NextRetryCount = GetCurrentRegionRetryCount() + 1;
    SetCurrentRegionRetryCount(NextRetryCount);

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
        {
            HUD->ShowMessage(FString::Printf(TEXT("You were struck down. Attempt %d begins anew."), NextRetryCount + 1), 4.2f);
            HUD->ShowDeathOverlay(NextRetryCount + 1);
        }
    }

    if (Regions.IsValidIndex(RegionIndex))
    {
        UpdateHUDForRegion(Regions[RegionIndex], bRegionCompleted);
    }
    SetMusicState(ENazareneMusicState::Tension, true);
    SaveCheckpoint();
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
        if (PlayerCharacter != nullptr)
        {
            FNazareneCampaignState& MutableState = Session->GetMutableCampaignState();
            MutableState.UnlockedSkills = PlayerCharacter->GetUnlockedSkills();
            MutableState.SkillPoints = PlayerCharacter->GetSkillPoints();
            MutableState.TotalXP = PlayerCharacter->GetTotalXP();
            MutableState.PlayerLevel = PlayerCharacter->GetPlayerLevel();
        }
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
        PlayerCharacter->SetCampaignBaseVitals(120.0f + Payload.Campaign.MaxHealthBonus, 100.0f + Payload.Campaign.MaxStaminaBonus, false);
        PlayerCharacter->SetUnlockedMiracles(Payload.Campaign.UnlockedMiracles);
        PlayerCharacter->SetSkillTreeState(Payload.Campaign.UnlockedSkills, Payload.Campaign.SkillPoints, Payload.Campaign.TotalXP, Payload.Campaign.PlayerLevel);
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
    bPrayerSiteConsecrated = false;
    if (Session && Regions.IsValidIndex(RegionIndex) && !Regions[RegionIndex].PrayerSiteId.IsNone())
    {
        const FName ConsecratedFlag(*FString::Printf(TEXT("site_%s_consecrated"), *Regions[RegionIndex].PrayerSiteId.ToString()));
        bPrayerSiteConsecrated = Session->IsFlagSet(ConsecratedFlag);
    }
    EnsureRetryCounterForCurrentRegion();
    UpdateChapterStageFromState();
    if (Regions.IsValidIndex(RegionIndex))
    {
        UpdateHUDForRegion(Regions[RegionIndex], bRegionCompleted);
    }
}

void ANazareneCampaignGameMode::SyncCompletionState()
{
    bRegionCompleted = (BossEnemy != nullptr && BossEnemy->IsRedeemed());
    EnableTravelGate(bRegionCompleted);
    UpdateChapterStageFromState();
}

void ANazareneCampaignGameMode::HandleEnemyRedeemed(ANazareneEnemyCharacter* Enemy, float FaithReward)
{
    if (bSuppressRedeemedCallbacks)
    {
        return;
    }

    int32 XPReward = FMath::Max(0, FMath::RoundToInt(FaithReward * 4.0f));
    bool bLeveledUp = false;
    int32 NewLevel = 1;
    int32 NewSkillPoints = 0;

    if (Session != nullptr && XPReward > 0)
    {
        FNazareneCampaignState& State = Session->GetMutableCampaignState();
        State.PlayerLevel = FMath::Max(State.PlayerLevel, 1);
        State.TotalXP = FMath::Max(State.TotalXP, 0);
        State.SkillPoints = FMath::Max(State.SkillPoints, 0);

        State.TotalXP += XPReward;
        while (State.TotalXP >= XPForLevel(State.PlayerLevel + 1))
        {
            ++State.PlayerLevel;
            ++State.SkillPoints;
            bLeveledUp = true;
        }

        NewLevel = State.PlayerLevel;
        NewSkillPoints = State.SkillPoints;

        if (PlayerCharacter != nullptr)
        {
            PlayerCharacter->SetSkillTreeState(State.UnlockedSkills, State.SkillPoints, State.TotalXP, State.PlayerLevel);
        }
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
        {
            const FString EnemyLabel = Enemy != nullptr ? Enemy->EnemyName : TEXT("Enemy");
            if (XPReward > 0)
            {
                HUD->ShowMessage(FString::Printf(TEXT("%s redeemed (+%d XP)"), *EnemyLabel, XPReward), 2.8f);
            }
            if (bLeveledUp)
            {
                HUD->ShowMessage(FString::Printf(TEXT("Level Up! Lvl %d | Skill Points %d"), NewLevel, NewSkillPoints), 3.8f);
            }
        }
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
    UpdateChapterStageFromState();
    UpdateHUDForRegion(Region, true);
    SetMusicState(ENazareneMusicState::Victory, true);

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
        PlayerCharacter->SetCampaignBaseVitals(120.0f + State.MaxHealthBonus, 100.0f + State.MaxStaminaBonus, true);
        PlayerCharacter->SetUnlockedMiracles(State.UnlockedMiracles);
        PlayerCharacter->SetSkillTreeState(State.UnlockedSkills, State.SkillPoints, State.TotalXP, State.PlayerLevel);

        FNazarenePlayerSnapshot Snapshot = PlayerCharacter->BuildSnapshot();
        Snapshot.Health = PlayerCharacter->MaxHealth;
        Snapshot.Stamina = PlayerCharacter->MaxStamina;
        PlayerCharacter->ApplySnapshot(Snapshot);
    }

    return bAnyReward;
}

void ANazareneCampaignGameMode::QueueIntroStoryIfNeeded()
{
    if (Session == nullptr || RegionIndex != 0)
    {
        return;
    }

    const FName IntroFlag(TEXT("intro_ch1_seen"));
    if (Session->IsFlagSet(IntroFlag))
    {
        return;
    }

    ActiveStoryLines =
    {
        TEXT("A hush falls over Galilee as the pilgrimage begins."),
        TEXT("Consecrate the prayer site and gather your strength."),
        TEXT("When the guardian falls, the road to Decapolis will open.")
    };
    StoryLineIndex = 0;
    Session->MarkFlag(IntroFlag);
    SetMusicState(ENazareneMusicState::Peace, false);

    AdvanceStoryLine();
    GetWorldTimerManager().SetTimer(StoryLineTimerHandle, this, &ANazareneCampaignGameMode::AdvanceStoryLine, 5.2f, true, 5.2f);
}

void ANazareneCampaignGameMode::AdvanceStoryLine()
{
    if (StoryLineIndex >= ActiveStoryLines.Num())
    {
        GetWorldTimerManager().ClearTimer(StoryLineTimerHandle);
        return;
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
        {
            HUD->ShowMessage(ActiveStoryLines[StoryLineIndex], 4.8f);
        }
    }

    ++StoryLineIndex;
}

FString ANazareneCampaignGameMode::BuildObjectiveText(const FNazareneRegionDefinition& Region, bool bCompleted) const
{
    const int32 RetryCount = GetCurrentRegionRetryCount();
    const FString RetrySuffix = RetryCount > 0 ? FString::Printf(TEXT(" (Retries: %d)"), RetryCount) : FString();

    switch (ChapterStage)
    {
    case ENazareneChapterStage::ConsecratePrayerSite:
        return FString::Printf(TEXT("Consecrate %s to reveal the guardian.%s"), *Region.PrayerSiteName, *RetrySuffix);

    case ENazareneChapterStage::DefeatGuardian:
        return FString::Printf(TEXT("%s%s"), *Region.Objective, *RetrySuffix);

    case ENazareneChapterStage::ReachTravelGate:
        if (RegionIndex >= Regions.Num() - 1)
        {
            return FString::Printf(TEXT("Step into the light to conclude the pilgrimage.%s"), *RetrySuffix);
        }
        return FString::Printf(TEXT("Journey marker open: travel to %s.%s"), *Regions[RegionIndex + 1].RegionName, *RetrySuffix);

    case ENazareneChapterStage::Completed:
        return FString::Printf(TEXT("Pilgrimage complete.%s"), *RetrySuffix);

    default:
        break;
    }

    return bCompleted ? TEXT("Pilgrimage complete.") : Region.Objective;
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
    HUD->SetObjective(BuildObjectiveText(Region, bCompleted));
}


void ANazareneCampaignGameMode::SetMusicState(ENazareneMusicState NewState, bool bAnnounceOnHUD)
{
    if (MusicState == NewState)
    {
        return;
    }

    MusicState = NewState;

    const TCHAR* StateLabel = TEXT("Peace");
    switch (MusicState)
    {
    case ENazareneMusicState::Peace:
        StateLabel = TEXT("Peace");
        break;
    case ENazareneMusicState::Tension:
        StateLabel = TEXT("Tension");
        break;
    case ENazareneMusicState::Combat:
        StateLabel = TEXT("Combat");
        break;
    case ENazareneMusicState::Boss:
        StateLabel = TEXT("Boss");
        break;
    case ENazareneMusicState::Victory:
        StateLabel = TEXT("Victory");
        break;
    default:
        break;
    }

    UE_LOG(LogTemp, Log, TEXT("Music state updated: %s"), StateLabel);

    if (!bAnnounceOnHUD)
    {
        return;
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
        {
            HUD->ShowMessage(FString::Printf(TEXT("Music: %s"), StateLabel), 2.4f);
        }
    }
}

USoundBase* ANazareneCampaignGameMode::ResolveRegionMusic(const FNazareneRegionDefinition& Region) const
{
    TSoftObjectPtr<USoundBase> Candidate;
    if (Region.RegionId == FName(TEXT("galilee")))
    {
        Candidate = GalileeMusic;
    }
    else if (Region.RegionId == FName(TEXT("decapolis")))
    {
        Candidate = DecapolisMusic;
    }
    else if (Region.RegionId == FName(TEXT("wilderness")))
    {
        Candidate = WildernessMusic;
    }
    else if (Region.RegionId == FName(TEXT("jerusalem")))
    {
        Candidate = JerusalemMusic;
    }
    else if (Region.RegionId == FName(TEXT("gethsemane")))
    {
        Candidate = GethsemaneMusic;
    }
    else if (Region.RegionId == FName(TEXT("via_dolorosa")))
    {
        Candidate = ViaDolorosaMusic;
    }
    else if (Region.RegionId == FName(TEXT("empty_tomb")))
    {
        Candidate = EmptyTombMusic;
    }

    return Candidate.ToSoftObjectPath().IsValid() ? Candidate.LoadSynchronous() : nullptr;
}

void ANazareneCampaignGameMode::CrossfadeToMusic(USoundBase* NewMusic)
{
    if (NewMusic == nullptr)
    {
        // Fade out current music
        if (RegionMusicComponent != nullptr && RegionMusicComponent->IsPlaying())
        {
            RegionMusicComponent->FadeOut(MusicCrossfadeDuration, 0.0f);
        }
        bCrossfading = false;
        return;
    }

    // Create the primary music component if it does not exist
    if (RegionMusicComponent == nullptr)
    {
        RegionMusicComponent = UGameplayStatics::CreateSound2D(this, NewMusic, 0.75f, 1.0f, 0.0f, nullptr, true);
        if (RegionMusicComponent != nullptr)
        {
            RegionMusicComponent->Play();
        }
        return;
    }

    // If already playing the same sound, do nothing
    if (RegionMusicComponent->IsPlaying() && RegionMusicComponent->Sound == NewMusic)
    {
        return;
    }

    // Create the crossfade component if it does not exist
    if (CrossfadeAudioComponent == nullptr)
    {
        CrossfadeAudioComponent = UGameplayStatics::CreateSound2D(this, NewMusic, 0.0f, 1.0f, 0.0f, nullptr, true);
    }
    else
    {
        CrossfadeAudioComponent->Stop();
        CrossfadeAudioComponent->SetSound(NewMusic);
        CrossfadeAudioComponent->SetVolumeMultiplier(0.0f);
    }

    if (CrossfadeAudioComponent != nullptr)
    {
        CrossfadeAudioComponent->Play();
    }

    bCrossfading = true;
    CrossfadeElapsed = 0.0f;
    CrossfadeStartVolume = RegionMusicComponent->VolumeMultiplier;
}

void ANazareneCampaignGameMode::TickCrossfade(float DeltaSeconds)
{
    if (!bCrossfading)
    {
        return;
    }

    CrossfadeElapsed += DeltaSeconds;
    const float Alpha = FMath::Clamp(CrossfadeElapsed / MusicCrossfadeDuration, 0.0f, 1.0f);

    if (RegionMusicComponent != nullptr)
    {
        RegionMusicComponent->SetVolumeMultiplier(FMath::Lerp(CrossfadeStartVolume, 0.0f, Alpha));
    }

    if (CrossfadeAudioComponent != nullptr)
    {
        CrossfadeAudioComponent->SetVolumeMultiplier(FMath::Lerp(0.0f, 0.75f, Alpha));
    }

    if (Alpha >= 1.0f)
    {
        if (RegionMusicComponent != nullptr)
        {
            RegionMusicComponent->Stop();
        }

        Swap(RegionMusicComponent, CrossfadeAudioComponent);
        bCrossfading = false;
    }
}

FString ANazareneCampaignGameMode::GetRandomLoreTip() const
{
    TArray<FString> Tips;
    const FString CharacterLibraryPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../../docs/CHARACTER_LIBRARY.md"));

    FString FileContents;
    if (FFileHelper::LoadFileToString(FileContents, *CharacterLibraryPath))
    {
        TArray<FString> Lines;
        FileContents.ParseIntoArrayLines(Lines, true);
        for (const FString& RawLine : Lines)
        {
            FString Line = RawLine;
            Line.TrimStartAndEndInline();
            if (Line.StartsWith(TEXT("## ")))
            {
                Line.RemoveFromStart(TEXT("## "));
                if (!Line.IsEmpty())
                {
                    Tips.Add(FString::Printf(TEXT("Lore: %s"), *Line));
                }
            }
            else if (Line.StartsWith(TEXT("- **")) && Tips.Num() < 64)
            {
                Tips.Add(Line);
            }
        }
    }

    if (Tips.Num() == 0)
    {
        Tips =
        {
            TEXT("Lore: The Sermon on the Mount reframed mercy as active strength."),
            TEXT("Lore: Galilee settlements blended fishing life with Roman pressure."),
            TEXT("Lore: Prayer sites in this campaign represent refuge and resolve."),
            TEXT("Lore: Decapolis city culture mixed Hellenistic and local traditions.")
        };
    }

    return Tips[FMath::RandRange(0, Tips.Num() - 1)];
}

void ANazareneCampaignGameMode::ConfigureEnemyBehaviorTree(ANazareneEnemyCharacter* Enemy) const
{
    if (Enemy == nullptr)
    {
        return;
    }

    TSoftObjectPtr<UBehaviorTree> Candidate;
    switch (Enemy->Archetype)
    {
    case ENazareneEnemyArchetype::MeleeShield:
        Candidate = BTMeleeShieldAsset;
        break;
    case ENazareneEnemyArchetype::Spear:
        Candidate = BTSpearAsset;
        break;
    case ENazareneEnemyArchetype::Ranged:
        Candidate = BTRangedAsset;
        break;
    case ENazareneEnemyArchetype::Demon:
        Candidate = BTDemonAsset;
        break;
    case ENazareneEnemyArchetype::Boss:
        Candidate = BTBossAsset;
        break;
    default:
        break;
    }

    if (Candidate.ToSoftObjectPath().IsValid())
    {
        if (UBehaviorTree* BTAsset = Candidate.LoadSynchronous())
        {
            Enemy->BehaviorTreeAsset = BTAsset;
        }
    }
}

int32 ANazareneCampaignGameMode::XPForLevel(int32 LevelValue) const
{
    const int32 SafeLevel = FMath::Max(LevelValue, 1);
    return 45 + (SafeLevel - 1) * (SafeLevel - 1) * 28;
}

void ANazareneCampaignGameMode::UpdateChapterStageFromState()
{
    if (bRegionCompleted)
    {
        if (RegionIndex >= Regions.Num() - 1)
        {
            ChapterStage = ENazareneChapterStage::Completed;
        }
        else
        {
            ChapterStage = ENazareneChapterStage::ReachTravelGate;
        }
        return;
    }

    ChapterStage = bPrayerSiteConsecrated ? ENazareneChapterStage::DefeatGuardian : ENazareneChapterStage::ConsecratePrayerSite;
}

void ANazareneCampaignGameMode::EnsureRetryCounterForCurrentRegion()
{
    if (Session == nullptr)
    {
        return;
    }

    FNazareneCampaignState& CampaignState = Session->GetMutableCampaignState();
    while (CampaignState.RegionRetryCounts.Num() <= RegionIndex)
    {
        CampaignState.RegionRetryCounts.Add(0);
    }
}

int32 ANazareneCampaignGameMode::GetCurrentRegionRetryCount() const
{
    if (Session == nullptr)
    {
        return 0;
    }

    const FNazareneCampaignState& CampaignState = Session->GetCampaignState();
    return CampaignState.RegionRetryCounts.IsValidIndex(RegionIndex) ? CampaignState.RegionRetryCounts[RegionIndex] : 0;
}

void ANazareneCampaignGameMode::SetCurrentRegionRetryCount(int32 RetryCount)
{
    if (Session == nullptr)
    {
        return;
    }

    EnsureRetryCounterForCurrentRegion();
    Session->GetMutableCampaignState().RegionRetryCounts[RegionIndex] = FMath::Max(0, RetryCount);
}

void ANazareneCampaignGameMode::SpawnWaveEnemies(const FNazareneEncounterWave& Wave)
{
    for (const FNazareneEnemySpawnDefinition& Spec : Wave.Enemies)
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
        ConfigureEnemyBehaviorTree(Enemy);
        Enemy->OnRedeemed.AddDynamic(this, &ANazareneCampaignGameMode::HandleEnemyRedeemed);
        RegionActors.Add(Enemy);
        EnemyBySpawnId.Add(Spec.SpawnId, Enemy);
    }
}

void ANazareneCampaignGameMode::CheckDeferredWaves(ENazareneSpawnTrigger Trigger, int32 Param)
{
    for (int32 Index = DeferredWaves.Num() - 1; Index >= 0; --Index)
    {
        const FNazareneEncounterWave& Wave = DeferredWaves[Index];
        if (Wave.Trigger != Trigger)
        {
            continue;
        }
        if (Trigger == ENazareneSpawnTrigger::OnBossPhaseChange && Wave.TriggerParam != Param)
        {
            continue;
        }

        if (Wave.DelaySeconds > 0.0f)
        {
            FNazareneEncounterWave WaveCopy = Wave;
            WaveCopy.DelaySeconds = 0.0f;
            FTimerHandle TimerHandle;
            GetWorldTimerManager().SetTimer(TimerHandle, [this, WaveCopy]() { SpawnWaveEnemies(WaveCopy); }, Wave.DelaySeconds, false);
        }
        else
        {
            SpawnWaveEnemies(Wave);
        }

        DeferredWaves.RemoveAt(Index);
    }
}

void ANazareneCampaignGameMode::HandleReinforcementWave(ANazareneEnemyCharacter* Boss, int32 Phase)
{
    CheckDeferredWaves(ENazareneSpawnTrigger::OnBossPhaseChange, Phase);
}

// -----------------------------------------------------------------------
// Task 7: Menu Camera
// -----------------------------------------------------------------------

void ANazareneCampaignGameMode::SpawnMenuCamera()
{
    if (MenuCamera != nullptr)
    {
        return;
    }

    FVector CameraCenter = FVector::ZeroVector;
    if (Regions.IsValidIndex(RegionIndex))
    {
        CameraCenter = Regions[RegionIndex].PrayerSiteLocation;
    }

    MenuCamera = GetWorld()->SpawnActor<ANazareneMenuCameraActor>(
        ANazareneMenuCameraActor::StaticClass(),
        CameraCenter + FVector(1800.0f, 0.0f, 400.0f),
        FRotator::ZeroRotator
    );

    if (MenuCamera != nullptr)
    {
        MenuCamera->OrbitCenter = CameraCenter;

        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC != nullptr)
        {
            PC->SetViewTarget(MenuCamera);
        }
    }
}

void ANazareneCampaignGameMode::DestroyMenuCamera()
{
    if (MenuCamera == nullptr)
    {
        return;
    }

    MenuCamera->Destroy();
    MenuCamera = nullptr;

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC != nullptr && PlayerCharacter != nullptr)
    {
        PC->SetViewTarget(PlayerCharacter);
    }
}

void ANazareneCampaignGameMode::OnMenuDismissed()
{
    DestroyMenuCamera();
}
