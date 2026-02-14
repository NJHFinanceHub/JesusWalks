#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NazareneSkillTree.generated.h"

UENUM(BlueprintType)
enum class ENazareneSkillBranch : uint8
{
    Combat = 0,
    Movement = 1,
    Miracles = 2,
    Defense = 3
};

USTRUCT(BlueprintType)
struct FNazareneSkillDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName SkillId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ENazareneSkillBranch Branch = ENazareneSkillBranch::Combat;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 XPRequirement = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Cost = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FName> Requires;
};

UCLASS(BlueprintType)
class THENAZARENEAAA_API UNazareneSkillTree : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Skills")
    static TArray<FName> GetAllSkillIds();

    UFUNCTION(BlueprintCallable, Category = "Skills")
    static bool GetSkillDefinition(FName SkillId, FNazareneSkillDefinition& OutDefinition);

    UFUNCTION(BlueprintCallable, Category = "Skills")
    static bool CanUnlockSkill(FName SkillId, const TArray<FName>& UnlockedSkills, int32 TotalXP, int32 SkillPoints);

    UFUNCTION(BlueprintCallable, Category = "Skills")
    static TArray<FName> GetBranchSkills(ENazareneSkillBranch Branch);
};
