// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EditorMouseTracker.generated.h"

/**
 * 
 */
UCLASS()
class PORTALSYSTEM_API UEditorMouseTracker : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Editor|Viewport")
	static bool IsMouseOverViewport();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Editor|Viewport")
	static bool GetMouseWorldPositionEditor(FVector& WorldPosition);
	
};
