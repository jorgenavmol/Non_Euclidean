#include "GravController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void AGravController::UpdateRotation(float DeltaTime)
{
	FVector GravityDirection = FVector::DownVector;

	if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement())
		{
			GravityDirection = MoveComp->GetGravityDirection();

			WorkingGravity = FMath::VInterpTo(WorkingGravity, GravityDirection, DeltaTime, InterpSpeed);
			
			// Normalizar WorkingGravity
			WorkingGravity.Normalize();
		}
	}

	// Obtener rotación actual de la cámara
	FRotator ViewRotation = GetControlRotation();

	// Si hay una gravedad anterior, aplicar delta de rotación
	if (!LastFrameGravity.IsZero())
	{
		const FQuat DeltaGravityRotation = FQuat::FindBetweenNormals(LastFrameGravity, WorkingGravity);
		
		if (GEngine)
		{
			FString DebugMessage = FString::Printf(TEXT("DeltaGravityRotation: X=%f, Y=%f, Z=%f, W=%f"),
				DeltaGravityRotation.X, DeltaGravityRotation.Y, DeltaGravityRotation.Z, DeltaGravityRotation.W);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);
		}

		const FQuat WarpedCameraRotation = DeltaGravityRotation * FQuat(ViewRotation);
		ViewRotation = WarpedCameraRotation.Rotator();
	}
	LastFrameGravity = WorkingGravity;

	// Pasar a espacio relativo a gravedad
	ViewRotation = GetGravityRelativeRotation(ViewRotation, WorkingGravity);

	FRotator DeltaRot(RotationInput);

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);

		ViewRotation.Roll = 0; // Mantener horizonte en relación a gravedad

		// Volver a rotación global (mundo) y aplicarla
		SetControlRotation(GetGravityWorldRotation(ViewRotation, WorkingGravity));
		
	}

	if (APawn* P = GetPawnOrSpectator())
	{
		P->FaceRotation(ViewRotation, DeltaTime);

		GetPawn()->SetActorRotation(GetGravityWorldRotation(ViewRotation, WorkingGravity));
	}
}

FRotator AGravController::GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection)
{
	if (!GravityDirection.Equals(FVector::DownVector))
	{
		const FQuat GravityRotation = FQuat::FindBetweenNormals(GravityDirection, FVector::DownVector);
		return (GravityRotation * Rotation.Quaternion()).Rotator();
	}

	return Rotation;
}

FRotator AGravController::GetGravityWorldRotation(FRotator Rotation, FVector GravityDirection)
{
	if (!GravityDirection.Equals(FVector::DownVector))
	{
		const FQuat GravityRotation = FQuat::FindBetweenNormals(FVector::DownVector, GravityDirection);
		return (GravityRotation * Rotation.Quaternion()).Rotator();
	}

	return Rotation;
}
