// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorMouseTracker.h"
#include "Editor.h"
#include "LevelEditorViewport.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "EditorViewportClient.h"

// Detecta si el rat�n est� sobre el viewport principal del editor.
bool UEditorMouseTracker::IsMouseOverViewport()
{
#if WITH_EDITOR
    // Obtenemos la posici�n del rat�n con respecto a la pantalla
    FVector2D MousePosition = FSlateApplication::Get().GetCursorPos();

    if (GEditor)
    {
        // Iteramos sobre todos los viewports activos
        for (FEditorViewportClient* ViewportClient : GEditor->GetAllViewportClients())
        {
            if (ViewportClient)
            {
                // Asegurarnos de que estamos trabajando con el "world viewport" (el que muestra el mundo)
                if (ViewportClient->IsPerspective() || ViewportClient->IsOrtho())
                {
                    // Obtenemos la posici�n inicial y el tama�o del viewport
                    FVector2D ViewportPos = ViewportClient->Viewport->GetInitialPositionXY();
                    FVector2D ViewportSize = FVector2D(ViewportClient->Viewport->GetSizeXY());

                    // Verificamos si la posici�n del rat�n est� dentro de los l�mites del viewport
                    if (MousePosition.X >= ViewportPos.X && MousePosition.X < ViewportPos.X + ViewportSize.X &&
                        MousePosition.Y >= ViewportPos.Y && MousePosition.Y < ViewportPos.Y + ViewportSize.Y)
                    {
                        return true; // El rat�n est� sobre el viewport que muestra el mundo
                    }
                }
            }
        }
    }
#endif
    return false;  // El rat�n no est� sobre el viewport que muestra el mundo
}

bool UEditorMouseTracker::GetMouseWorldPositionEditor(FVector& WorldPosition)
{
// Verificamos si estamos en el editor
#if WITH_EDITOR
    if (!GEditor) return false;

	//Obtenemos el viewport activo y el viewport client(camara, transformaciones, etc)
    FViewport* Viewport = GEditor->GetActiveViewport();
    FEditorViewportClient* ViewportClient = static_cast<FEditorViewportClient*>(Viewport ? Viewport->GetClient() : nullptr);
	if (!Viewport || !ViewportClient) return false;

    // Asegur�mos que el cursor est� visible en el editor
	FSlateApplication::Get().SetPlatformCursorVisibility(true);

	// Obtenemos la posici�n del rat�n en coordenadas de pantalla.
    FIntPoint MousePos;
    Viewport->GetMousePos(MousePos);

    // Configuramos el contexto de vista de escena para calcular c�mo la c�mara del editor ve la escena (para la deproyecci�n).
    FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
        Viewport,
        ViewportClient->GetScene(),
        ViewportClient->EngineShowFlags));

	// A partir de la anterior vista construida, creamos una vista de escena.
    FSceneView* SceneView = ViewportClient->CalcSceneView(&ViewFamily);
    if (!SceneView) return false;

	// Deproyectamos las coordenadas de pantalla a coordenadas del mundo.
    FVector Origin, Direction;
    SceneView->DeprojectFVector2D(MousePos, Origin, Direction);

	// Realizamos un rayo desde la posici�n del rat�n en el mundo.
    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World) return false;

    FHitResult Hit;
    if (World->LineTraceSingleByChannel(Hit, Origin, Origin + Direction * 100000.f, ECC_Visibility))
    {
        WorldPosition = Hit.Location;
        return true;
    }
#endif
    return false;
}

