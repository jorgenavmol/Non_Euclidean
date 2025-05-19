#include "GravController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void AGravController::UpdateRotation(float DeltaTime)
{
    FVector GravityDirection = FVector::DownVector;
    bool bGravityFlipped = false;

    if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn()))
    {
        if (UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement())
        {
            GravityDirection = MoveComp->GetGravityDirection();

            float Dot = FVector::DotProduct(WorkingGravity, GravityDirection);

            if (Dot < -0.999f)
            {
                bGravityFlipped = true;
                WorkingGravity = GravityDirection;
            }
            else
            {
                WorkingGravity = FMath::VInterpTo(WorkingGravity, GravityDirection, DeltaTime, InterpSpeed);
            }

            WorkingGravity.Normalize();
        }
    }

    // Obtener rotación actual
    FRotator ViewRotation = GetControlRotation();

    if (!LastFrameGravity.IsZero() && !bGravityFlipped)
    {
        // Solo aplicar interpolación si no es un cambio de 180°
        const FQuat DeltaGravityRotation = FQuat::FindBetweenNormals(LastFrameGravity, WorkingGravity);
        const FQuat WarpedCameraRotation = DeltaGravityRotation * FQuat(ViewRotation);
        ViewRotation = WarpedCameraRotation.Rotator();
    }

    // Al cambiar 180°, simplemente invertir la vista vertical (mirar "desde abajo")
    if (bGravityFlipped)
    {
        ViewRotation.Pitch *= -1;
        ViewRotation.Roll *= -1;

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Gravedad 180°: manteniendo dirección sin girar"));
        }
    }

    LastFrameGravity = WorkingGravity;

    // Pasar a espacio relativo a gravedad
    ViewRotation = GetGravityRelativeRotation(ViewRotation, WorkingGravity);

    FRotator DeltaRot(RotationInput);

    if (PlayerCameraManager)
    {
        PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
        ViewRotation.Roll = 0;
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
