#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NazareneTypes.h"
#include "NazareneEnemyAnimInstance.generated.h"

UCLASS(BlueprintType)
class THENAZARENEAAA_API UNazareneEnemyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    float GroundSpeed = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    ENazareneEnemyState State = ENazareneEnemyState::Idle;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    int32 BossPhase = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    bool bRedeemed = false;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    bool bParried = false;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    bool bStaggered = false;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    float StateTimerRemaining = 0.0f;
};

