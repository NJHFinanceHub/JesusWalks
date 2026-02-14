#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NazareneTypes.h"
#include "NazareneNPC.generated.h"

class ANazarenePlayerCharacter;
class USphereComponent;
class UStaticMeshComponent;

UCLASS(BlueprintType)
class THENAZARENEAAA_API ANazareneNPC : public AActor
{
	GENERATED_BODY()

public:
	ANazareneNPC();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FString NPCName = TEXT("Villager");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FString CharacterSlug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	float InteractionRadius = 280.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Dialogue")
	TArray<FNazareneNPCDialogueLine> DialogueLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Dialogue")
	bool bLoopDialogue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Dialogue")
	FString IdleGreeting;

	UFUNCTION(BlueprintCallable, Category = "NPC")
	void AdvanceDialogue();

	UFUNCTION(BlueprintCallable, Category = "NPC")
	void ResetDialogue();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> HeadMesh;

	UFUNCTION()
	void HandleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	int32 CurrentDialogueIndex = 0;
	bool bPlayerInRange = false;

	UPROPERTY()
	TWeakObjectPtr<ANazarenePlayerCharacter> CachedPlayer;
};
