#include "GravController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void AGravController::UpdateRotation(float DeltaTime)
{
    FVector GravityDirection = FVector::DownVector;
    bool bGravityFlipped = false;

    FRotator ViewRotation = GetControlRotation();

    if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn()))
    {
        if (UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement())
        {
            GravityDirection = MoveComp->GetGravityDirection();
            float Dot = FVector::DotProduct(WorkingGravity, GravityDirection);

            if (Dot < -0.999f) // Dirección de gravedad opuesta = giro 180°
            {
                bGravityFlipped = true;
                WorkingGravity = GravityDirection;

                if (APawn* P = GetPawnOrSpectator())
                {
                    StartQuat = P->GetActorQuat();

                    FVector NewUp = -GravityDirection;
                    FVector Forward = P->GetActorForwardVector();

                    // Si están alineados, no se puede proyectar = construir dirección alternativa
                    float Alignment = FMath::Abs(FVector::DotProduct(Forward, NewUp));
                    if (Alignment > 0.99f)
                    {
                        FVector Arbitrary = FVector::RightVector;
                        if (FMath::Abs(FVector::DotProduct(Arbitrary, NewUp)) > 0.99f)
                        {
                            Arbitrary = FVector::ForwardVector;
                        }
                        FVector Right = FVector::CrossProduct(Arbitrary, NewUp).GetSafeNormal();
                        Forward = FVector::CrossProduct(NewUp, Right).GetSafeNormal();
                    }
                    else
                    {
                        Forward = FVector::VectorPlaneProject(Forward, NewUp).GetSafeNormal();
                    }

                    FVector Right = FVector::CrossProduct(NewUp, Forward).GetSafeNormal();
                    FVector CorrectedForward = FVector::CrossProduct(Right, NewUp).GetSafeNormal();

                    FMatrix RotMatrix;
                    RotMatrix.SetAxis(0, CorrectedForward);
                    RotMatrix.SetAxis(1, Right);
                    RotMatrix.SetAxis(2, NewUp);

                    TargetQuat = FQuat(RotMatrix);

                    bIsInterpolating180 = true;
                    RotationInterpProgress = 0.0f;
                }
            }
            else
            {
                WorkingGravity = FMath::VInterpTo(WorkingGravity, GravityDirection, DeltaTime, InterpSpeed);
            }

            WorkingGravity.Normalize();
        }
    }

    if (bIsInterpolating180)
    {
        RotationInterpProgress += DeltaTime * Rotation180InterpSpeed;

        if (APawn* P = GetPawnOrSpectator())
        {
            const float Alpha = FMath::Clamp(RotationInterpProgress, 0.0f, 1.0f);
            const FQuat InterpolatedQuat = FQuat::Slerp(StartQuat, TargetQuat, Alpha);
			P->SetActorRotation(InterpolatedQuat); // Rotacion del jugador.

            // También rotar la cámara
            const FQuat ControlQuat = InterpolatedQuat;
			SetControlRotation(ControlQuat.Rotator()); // Rotacion de la cámara.
        }

        if (RotationInterpProgress >= 1.0f)
        {
            bIsInterpolating180 = false;

            if (APawn* P = GetPawnOrSpectator())
            {
                FRotator FinalRot = TargetQuat.Rotator();
                FinalRot.Pitch = 0.0f;
                FinalRot.Roll = 0.0f;
                SetControlRotation(FinalRot);
            }

            return;
        }

        return;
    }

    if (!LastFrameGravity.IsZero() && !bGravityFlipped)
    {
        const FQuat DeltaGravityRotation = FQuat::FindBetweenNormals(LastFrameGravity, WorkingGravity);
        const FQuat WarpedCameraRotation = DeltaGravityRotation * FQuat(ViewRotation);
        //ViewRotation = WarpedCameraRotation.Rotator();
    }

    LastFrameGravity = WorkingGravity;

    ViewRotation = GetGravityRelativeRotation(ViewRotation, WorkingGravity);
    FRotator DeltaRot(RotationInput);

    if (PlayerCameraManager)
    {
        PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
        ViewRotation.Roll = 0;
		SetControlRotation(GetGravityWorldRotation(ViewRotation, WorkingGravity)); // Rotación de la cámara.
    }

    if (!bIsInterpolating180)
    {
        if (APawn* P = GetPawnOrSpectator())
        {
            P->FaceRotation(ViewRotation, DeltaTime);
			GetPawn()->SetActorRotation(GetGravityWorldRotation(ViewRotation, WorkingGravity)); // Rotación del jugador.
        }
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
