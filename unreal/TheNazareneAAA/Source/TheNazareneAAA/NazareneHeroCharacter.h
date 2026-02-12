#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NazareneHeroCharacter.generated.h"

USTRUCT(BlueprintType)
struct FNazareneMiracle
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MiracleId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FaithCost = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CooldownSeconds = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Power = 25.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNazareneHealthChanged, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNazareneFaithChanged, float, NewFaith, float, MaxFaith);

UCLASS()
class THENAZARENEAAA_API ANazareneHeroCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ANazareneHeroCharacter();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float CurrentHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float BaseMeleeDamage = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faith")
    float MaxFaith = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Faith")
    float CurrentFaith = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    TArray<FNazareneMiracle> KnownMiracles;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnNazareneHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnNazareneFaithChanged OnFaithChanged;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    float ReceiveDamage(float DamageAmount);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool IsDead() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    float PerformMeleeAttack(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Faith")
    void RestoreAtPrayer(float HealthRestorePercent = 1.0f, float FaithRestorePercent = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Miracles")
    bool CastMiracleById(FName MiracleId, AActor* Target, float& OutDamageDone);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(Transient)
    TMap<FName, float> MiracleReadyTimes;

    bool CanCastMiracle(const FNazareneMiracle& Miracle) const;
    void MarkMiracleCast(const FNazareneMiracle& Miracle);
};
