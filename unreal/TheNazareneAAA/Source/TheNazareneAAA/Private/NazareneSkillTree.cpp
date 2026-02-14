#include "NazareneSkillTree.h"

namespace
{
    static TMap<FName, FNazareneSkillDefinition> BuildSkillDefinitionTable()
    {
        TMap<FName, FNazareneSkillDefinition> Definitions;

        auto AddSkill = [&Definitions](
            const TCHAR* SkillId,
            const TCHAR* SkillName,
            ENazareneSkillBranch Branch,
            const TCHAR* Description,
            int32 XPRequirement,
            int32 Cost,
            TArray<FName> Requires
        )
        {
            FNazareneSkillDefinition Def;
            Def.SkillId = FName(SkillId);
            Def.Name = SkillName;
            Def.Branch = Branch;
            Def.Description = Description;
            Def.XPRequirement = XPRequirement;
            Def.Cost = Cost;
            Def.Requires = MoveTemp(Requires);
            Definitions.Add(Def.SkillId, Def);
        };

        AddSkill(TEXT("combat_smite"), TEXT("Smite Training"), ENazareneSkillBranch::Combat, TEXT("Increase light/heavy attack damage by 10%."), 60, 1, {});
        AddSkill(TEXT("combat_crusader"), TEXT("Crusader Momentum"), ENazareneSkillBranch::Combat, TEXT("Increase heavy poise damage and melee range."), 140, 1, { FName(TEXT("combat_smite")) });

        AddSkill(TEXT("movement_pilgrim_stride"), TEXT("Pilgrim Stride"), ENazareneSkillBranch::Movement, TEXT("Increase move speed by 12%."), 70, 1, {});
        AddSkill(TEXT("movement_swift_vow"), TEXT("Swift Vow"), ENazareneSkillBranch::Movement, TEXT("Reduce dodge stamina cost and cooldown."), 165, 1, { FName(TEXT("movement_pilgrim_stride")) });

        AddSkill(TEXT("miracles_abundance"), TEXT("Abundance of Grace"), ENazareneSkillBranch::Miracles, TEXT("Healing miracle restores more health."), 90, 1, {});
        AddSkill(TEXT("miracles_radiance_lance"), TEXT("Radiance Lance"), ENazareneSkillBranch::Miracles, TEXT("Radiance miracle gains damage and radius."), 190, 1, { FName(TEXT("miracles_abundance")) });

        AddSkill(TEXT("defense_shepherd_guard"), TEXT("Shepherd Guard"), ENazareneSkillBranch::Defense, TEXT("Increase max health and block efficiency."), 80, 1, {});
        AddSkill(TEXT("defense_steadfast"), TEXT("Steadfast Heart"), ENazareneSkillBranch::Defense, TEXT("Increase max stamina and regeneration."), 170, 1, { FName(TEXT("defense_shepherd_guard")) });

        return Definitions;
    }

    static const TMap<FName, FNazareneSkillDefinition>& GetDefinitionTable()
    {
        static const TMap<FName, FNazareneSkillDefinition> Definitions = BuildSkillDefinitionTable();
        return Definitions;
    }
}

TArray<FName> UNazareneSkillTree::GetAllSkillIds()
{
    TArray<FName> SkillIds;
    GetDefinitionTable().GetKeys(SkillIds);
    return SkillIds;
}

bool UNazareneSkillTree::GetSkillDefinition(FName SkillId, FNazareneSkillDefinition& OutDefinition)
{
    if (const FNazareneSkillDefinition* Found = GetDefinitionTable().Find(SkillId))
    {
        OutDefinition = *Found;
        return true;
    }

    OutDefinition = FNazareneSkillDefinition();
    return false;
}

bool UNazareneSkillTree::CanUnlockSkill(FName SkillId, const TArray<FName>& UnlockedSkills, int32 TotalXP, int32 SkillPoints)
{
    const FNazareneSkillDefinition* Definition = GetDefinitionTable().Find(SkillId);
    if (Definition == nullptr)
    {
        return false;
    }

    if (UnlockedSkills.Contains(SkillId))
    {
        return false;
    }

    if (SkillPoints < Definition->Cost)
    {
        return false;
    }

    if (TotalXP < Definition->XPRequirement)
    {
        return false;
    }

    for (const FName RequiredSkillId : Definition->Requires)
    {
        if (!UnlockedSkills.Contains(RequiredSkillId))
        {
            return false;
        }
    }

    return true;
}

TArray<FName> UNazareneSkillTree::GetBranchSkills(ENazareneSkillBranch Branch)
{
    TArray<FName> SkillIds;
    for (const TPair<FName, FNazareneSkillDefinition>& Pair : GetDefinitionTable())
    {
        if (Pair.Value.Branch == Branch)
        {
            SkillIds.Add(Pair.Key);
        }
    }
    return SkillIds;
}
