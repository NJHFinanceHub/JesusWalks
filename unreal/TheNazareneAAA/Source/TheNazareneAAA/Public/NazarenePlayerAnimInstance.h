#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NazarenePlayerAnimInstance.generated.h"

UCLASS(BlueprintType)
class THENAZARENEAAA_API UNazarenePlayerAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    float GroundSpeed = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    float ForwardSpeed = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    float RightSpeed = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    bool bInAir = false;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    bool bBlocking = false;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    bool bDodging = false;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    bool bAttacking = false;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    bool bLockedOn = false;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    bool bDefeated = false;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    float HurtTimeRemaining = 0.0f;
};

