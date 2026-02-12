#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NazareneEnemyCharacter.generated.h"

USTRUCT(BlueprintType)
struct FNazareneEnemyArchetype
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ArchetypeId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackPower = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Aggression = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RegionWeight = 1;
};

UCLASS()
class THENAZARENEAAA_API ANazareneEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ANazareneEnemyCharacter();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    FNazareneEnemyArchetype Archetype;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    float CurrentHealth = 100.0f;

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void InitializeFromArchetype(const FNazareneEnemyArchetype& InArchetype);

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    float ReceiveDamage(float DamageAmount);

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    bool IsDefeated() const;
};
