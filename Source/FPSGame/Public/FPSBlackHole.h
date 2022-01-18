#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "FPSBlackHole.generated.h"

UCLASS()
class AFPSBlackHole : public AActor
{
	GENERATED_BODY()


public:
	AFPSBlackHole();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* AttractorSphere;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* DestroyerSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float ForceStrength = 2500.f;


public:
	virtual void Tick(float DeltaSeconds) override;
};
