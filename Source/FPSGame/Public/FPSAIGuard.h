// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "FPSAIGuard.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Suspicious,
	Alerted
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	AFPSAIGuard();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

	FRotator OriginalRotation;
	UFUNCTION()
	void ResetOrientation();
	FTimerHandle TimerHandle_ResetOrientation;

	UFUNCTION()
	void OnRep_GuardState();

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EAIState GuardState;

	void SetGuardState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category="AI")
	void OnStateChanged(EAIState NewState);

	UPROPERTY(EditAnywhere, Category="AI")
	TArray<ATargetPoint*> PatrolPoints;
	UPROPERTY(VisibleAnywhere, Category="AI")
	ATargetPoint* CurrentTarget;
	UPROPERTY(EditAnywhere, Category="AI")
	bool bIsPatrol;
	void Patrol();
	int PatrolIndex;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Tick(float DeltaTime) override;
};
