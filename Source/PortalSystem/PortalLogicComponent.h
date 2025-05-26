// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PortalLogicComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTALSYSTEM_API UPortalLogicComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPortalLogicComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	class USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	AActor* LinkedPortal;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	TArray<AActor*> FoundPortals;*/

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	int CurrentRecursion = 0;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	int MaxRecursions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	FVector LastPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	bool LastInFront = false;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Portal")
	void Init(AActor* LinkedPortalActor, int MaxRecursionsValue);

	UFUNCTION(BlueprintCallable, Category = "Portal")
	FVector UpdateLoc(FVector OldLocation);

	UFUNCTION(BlueprintCallable, Category = "Portal")
	FRotator UpdateRot(FRotator OldRotation);

	UFUNCTION(BlueprintCallable, Category = "Portal")
	void RenderFlatView();




	void RenderFlatView(const FVector& VirtualCamLoc, const FRotator& VirtualCamRot);

	UFUNCTION(BlueprintCallable, Category = "Portal")
	bool IsPortalInFront(AActor* Observer, AActor* Target);



	// Funcion que renderiza la vista recursiva.
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void RenderRecursiveView(FVector Location, FRotator Rotation, int CurrentRecursion);

	// Funcion que renderiza la vista recursiva.
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void ControlRecursions();


		
};
