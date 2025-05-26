#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GravController.generated.h"

/**
 * PlayerController adaptado para rotación de cámara con gravedad personalizada.
 * No incluye reorientación automática.
 */
UCLASS()
class AGravController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void UpdateRotation(float DeltaTime) override;

	// Converts a rotation from world space to gravity relative space.
	UFUNCTION(BlueprintPure)
	static FRotator GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection);

	// Converts a rotation from gravity relative space to world space.
	UFUNCTION(BlueprintPure)
	static FRotator GetGravityWorldRotation(FRotator Rotation, FVector GravityDirection);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LerpAlpha = 0.0f;

	FVector WorkingGravity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float InterpSpeed = 3.0f;

	FRotator TargetRotation;
	FQuat StartQuat;
	FQuat TargetQuat;

	bool bIsInterpolating180 = false;
	float RotationInterpProgress = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float Rotation180InterpSpeed = 2.0f;


private:
	FVector LastFrameGravity = FVector::ZeroVector;

};