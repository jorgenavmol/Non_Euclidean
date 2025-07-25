// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalLogicComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UPortalLogicComponent::UPortalLogicComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPortalLogicComponent::BeginPlay()
{
	Super::BeginPlay();

	// Busca una SceneCaptureComponent2D en el actor (puedes refinar esto)
	SceneCapture = GetOwner()->FindComponentByClass<USceneCaptureComponent2D>();

	//Guardamos todos los portales encontrados
	//UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Portal"), FoundPortals);
}


// Called every frame
void UPortalLogicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPortalLogicComponent::Init(AActor* LinkedPortalActor, int MaxRecursionsValue)
{
	LinkedPortal = LinkedPortalActor;
	MaxRecursions = MaxRecursionsValue;
}

FVector UPortalLogicComponent::UpdateLoc(FVector OldLocation)
{
	FTransform OrigTransf = GetOwner()->GetTransform();
	FVector InverseScale = OrigTransf.GetScale3D();

	float ScaleX = -InverseScale.X;
	float ScaleY = -InverseScale.Y;

	FVector newVec(ScaleX, ScaleY, InverseScale.Z);
	FTransform NewTransf = FTransform(OrigTransf.GetRotation(), OrigTransf.GetLocation(), newVec);

	FVector InverseLocation = NewTransf.InverseTransformPosition(OldLocation);

	FVector NewLocation = LinkedPortal->GetTransform().TransformPosition(InverseLocation);

	return NewLocation;

}

FRotator UPortalLogicComponent::UpdateRot(FRotator OldRotation)
{
	FTransform OrigTransf = GetOwner()->GetTransform();

	FMatrix OldRotMatrix = FRotationMatrix(OldRotation);

	FVector XAxis = OldRotMatrix.GetUnitAxis(EAxis::X); // Forward
	FVector YAxis = OldRotMatrix.GetUnitAxis(EAxis::Y); // Right
	FVector ZAxis = OldRotMatrix.GetUnitAxis(EAxis::Z); // Up

	FVector LocalX = OrigTransf.InverseTransformVectorNoScale(XAxis);
	FVector LocalY = OrigTransf.InverseTransformVectorNoScale(YAxis);
	FVector LocalZ = OrigTransf.InverseTransformVectorNoScale(ZAxis);

	FVector MirrorX1 = UKismetMathLibrary::MirrorVectorByNormal(LocalX, FVector(1, 0, 0));
	FVector MirrorY1 = UKismetMathLibrary::MirrorVectorByNormal(LocalY, FVector(1, 0, 0));
	FVector MirrorZ1 = UKismetMathLibrary::MirrorVectorByNormal(LocalZ, FVector(1, 0, 0));

	FVector MirrorX2 = UKismetMathLibrary::MirrorVectorByNormal(MirrorX1, FVector(0, 1, 0));
	FVector MirrorY2 = UKismetMathLibrary::MirrorVectorByNormal(MirrorY1, FVector(0, 1, 0));
	FVector MirrorZ2 = UKismetMathLibrary::MirrorVectorByNormal(MirrorZ1, FVector(0, 1, 0));

	FVector NewX = LinkedPortal->GetTransform().TransformVectorNoScale(MirrorX2);
	FVector NewY = LinkedPortal->GetTransform().TransformVectorNoScale(MirrorY2);
	FVector NewZ = LinkedPortal->GetTransform().TransformVectorNoScale(MirrorZ2);

	FMatrix NewRotMatrix = FMatrix(NewX, NewY, NewZ, FVector(0, 0, 0));
	FRotator NewRot = NewRotMatrix.Rotator();

	return NewRot;
}

void UPortalLogicComponent::RenderFlatView()
{
	USceneCaptureComponent2D* LinkedCamera = LinkedPortal->FindComponentByClass<USceneCaptureComponent2D>();
	if (!LinkedCamera) return;

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CameraManager) return;

	FVector PlayerLoc = CameraManager->GetCameraLocation();
	FRotator PlayerRot = CameraManager->GetCameraRotation();

	FVector NewLoc = UpdateLoc(PlayerLoc);
	FRotator NewRot = UpdateRot(PlayerRot);

	LinkedCamera->SetWorldLocationAndRotation(NewLoc, NewRot);

	// Limpiar render target si es necesario
	//LinkedCamera->TextureTarget->UpdateResourceImmediate(true);

	LinkedCamera->CaptureScene();



	//for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	//{
	//	AActor* OtherPortal = *It;
	//	if (OtherPortal == GetOwner() || OtherPortal == LinkedPortal) continue;

	//	if (IsPortalInFront(LinkedPortal, OtherPortal))
	//	{
	//		UPortalLogicComponent* OtherLogic = OtherPortal->FindComponentByClass<UPortalLogicComponent>();
	//		if (!OtherLogic) continue;

	//		// Renderizar desde la vista virtual
	//		FVector TransformedLoc = UpdateLoc(CameraManager->GetCameraLocation());
	//		FRotator TransformedRot = UpdateRot(CameraManager->GetCameraRotation());

	//		OtherLogic->RenderFlatView(TransformedLoc, TransformedRot);
	//	}
	//}
}

void UPortalLogicComponent::RenderRecursiveView(FVector Location, FRotator Rotation, int CurrentRecursion)
{

	if (!LinkedPortal)
	{
		UE_LOG(LogTemp, Warning, TEXT("LinkedPortal is null."));
		return;
	}

	// Buscar el SceneCaptureComponent2D dentro del actor
	USceneCaptureComponent2D* LinkedCamera = LinkedPortal->FindComponentByClass<USceneCaptureComponent2D>();

	// Buscar el Panel dentro del actor
	UStaticMeshComponent* Panel = GetOwner()->FindComponentByTag<UStaticMeshComponent>(TEXT("Panel"));

	if (Panel)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Portal] Componente de l�gica inicializado."));
	}

	if (CurrentRecursion == 0)
	{
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (!CameraManager) return;

		FVector TempLoc = UpdateLoc(CameraManager->GetCameraLocation());
		FRotator TempRot = UpdateRot(CameraManager->GetCameraRotation());

		CurrentRecursion++;

		RenderRecursiveView(TempLoc, TempRot, CurrentRecursion);

		LinkedCamera->SetWorldLocationAndRotation(TempLoc, TempRot);
		LinkedCamera->CaptureScene();
		CurrentRecursion = 0;
	}
	else if (CurrentRecursion < MaxRecursions)
	{
		FVector TempLoc = UpdateLoc(Location);
		FRotator TempRot = UpdateRot(Rotation);
		CurrentRecursion++;

		RenderRecursiveView(TempLoc, TempRot, CurrentRecursion);
		LinkedCamera->SetWorldLocationAndRotation(TempLoc, TempRot);
		LinkedCamera->CaptureScene();
	}
	else
	{
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (!CameraManager) return;

		FVector PlayerLocation = CameraManager->GetCameraLocation();
		FVector TempLoc = UpdateLoc(Location);
		FRotator TempRot = UpdateRot(Rotation);

		FVector PortalLocation = GetOwner()->GetActorLocation();
		FVector LinkedPortalLocation = LinkedPortal->GetActorLocation();

		// Vector completo entre portales (incluye vertical)
		FVector Delta = LinkedPortalLocation - PortalLocation;

		FVector PlayerToPortal = PlayerLocation - PortalLocation;
		float DistanceToPortalPlane = FVector::DotProduct(PlayerToPortal, GetOwner()->GetActorForwardVector());

		// Ajuste suave (puedes probar con multiplicadores)
		float DistanceFactor = -DistanceToPortalPlane * 1.001f;

		// Empuja en la direcci�n diagonal entre portales, manteniendo la distancia escalada
		FVector FakeInfiniteLoc = TempLoc + Delta.GetSafeNormal() * DistanceFactor;

		LinkedCamera->SetWorldLocationAndRotation(FakeInfiniteLoc, TempRot);
		Panel->SetVisibility(false);
		LinkedCamera->CaptureScene();
		Panel->SetVisibility(true);
	}
}



void UPortalLogicComponent::ControlRecursions()
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CameraManager || !LinkedPortal) return;

	FVector CameraLocation = CameraManager->GetCameraLocation();
	FVector PortalLocation = GetOwner()->GetActorLocation();
	float Distance = FVector::Dist(CameraLocation, PortalLocation);

	// Buscar el colisionador del portal
	UBoxComponent* PortalCollider = GetOwner()->FindComponentByTag<UBoxComponent>(TEXT("CollisionDetector"));
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	// Comprobar direcci�n de los portales
	FVector ToLinked = (LinkedPortal->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
	FVector ThisForward = GetOwner()->GetActorForwardVector();
	float DotView = FVector::DotProduct(ToLinked, ThisForward);

	// �Est� mirando al portal?
	FRotator CameraRotation = CameraManager->GetCameraRotation();
	FVector CameraForward = CameraRotation.Vector();
	FVector DirToPortal = (PortalLocation - CameraLocation).GetSafeNormal();
	float DotProduct = FVector::DotProduct(DirToPortal, CameraForward);

	// Valores configurables
	const float MinDistance = 300.f;   // cerca
	const float MaxDistance = 1500.f;  // lejos
	const int MinRecursions = 1;
	const int MaxRecursionsAllowed = 2;

	if ((DotProduct > 0.5f || (PortalCollider && PortalCollider->IsOverlappingActor(PlayerCharacter))) && DotView > 0.6f)
	{
		// Interpolar recursi�n seg�n distancia (cerca = m�s recursiones)
		float Alpha = FMath::Clamp((MaxDistance - Distance) / (MaxDistance - MinDistance), 0.f, 1.f);
		MaxRecursions = FMath::RoundToInt(FMath::Lerp((float)MinRecursions, (float)MaxRecursionsAllowed, Alpha));
		if (GEngine) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Recursiones: %d"), MaxRecursions));
		}
	}
	else
	{
		MaxRecursions = MinRecursions;
	}
}



bool UPortalLogicComponent::IsPortalInFront(AActor* Observer, AActor* Target)
{
	FVector Forward = Observer->GetActorForwardVector();
	FVector ToTarget = Target->GetActorLocation() - Observer->GetActorLocation();
	ToTarget.Normalize();

	float Dot = FVector::DotProduct(Forward, ToTarget);
	return Dot > 0.5f; // Ajusta umbral seg�n �ngulo de visi�n
}



