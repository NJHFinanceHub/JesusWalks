#include "NazareneCampaignSubsystem.h"

void UNazareneCampaignSubsystem::StartNewCampaign()
{
    CampaignState = FNazareneCampaignState();
}

void UNazareneCampaignSubsystem::RecordPrayer()
{
    CampaignState.PrayerCount += 1;
}

bool UNazareneCampaignSubsystem::RecordEncounterWin(const FName RegionId, const bool bBossEncounter)
{
    FNazareneRegionState* RegionState = FindRegionStateMutable(RegionId);
    if (!RegionState)
    {
        return false;
    }

    RegionState->ClearedEncounters += 1;

    if (bBossEncounter)
    {
        RegionState->bBossDefeated = true;
    }

    if (const FNazareneRegionDefinition* Definition = RegionDefinitions.Find(RegionId))
    {
        if (RegionState->ClearedEncounters >= Definition->EncountersToClear)
        {
            CampaignState.TotalSoulCurrency += Definition->SoulRewardOnClear;
            CampaignState.ChapterIndex += 1;
            return true;
        }
    }

    return false;
}

void UNazareneCampaignSubsystem::RegisterRegionDefinition(FNazareneRegionDefinition Definition)
{
    if (Definition.RegionId.IsNone())
    {
        return;
    }

    Definition.EncountersToClear = FMath::Max(1, Definition.EncountersToClear);
    Definition.SoulRewardOnClear = FMath::Max(0, Definition.SoulRewardOnClear);
    RegionDefinitions.FindOrAdd(Definition.RegionId) = Definition;

    FindRegionStateMutable(Definition.RegionId);
}

const FNazareneCampaignState& UNazareneCampaignSubsystem::GetCampaignState() const
{
    return CampaignState;
}

void UNazareneCampaignSubsystem::ApplyLoadedCampaignState(const FNazareneCampaignState& LoadedState)
{
    CampaignState = LoadedState;
}

FNazareneRegionState* UNazareneCampaignSubsystem::FindRegionStateMutable(const FName RegionId)
{
    if (RegionId.IsNone())
    {
        return nullptr;
    }

    if (FNazareneRegionState* Existing = CampaignState.Regions.FindByPredicate([RegionId](const FNazareneRegionState& Region)
        {
            return Region.RegionId == RegionId;
        }))
    {
        return Existing;
    }

    FNazareneRegionState& NewRegion = CampaignState.Regions.AddDefaulted_GetRef();
    NewRegion.RegionId = RegionId;
    return &NewRegion;
}
