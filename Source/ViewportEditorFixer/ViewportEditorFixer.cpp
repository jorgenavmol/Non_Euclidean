// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewportEditorFixer.h"
#include "Modules/ModuleManager.h"
#include "Editor.h"

#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "EditorViewportClient.h"
#include "LevelEditorViewport.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, ViewportEditorFixer );

void FViewportEditorFixerModule::StartupModule()
{
    if (GEditor)
    {
        // Activa Realtime en todos los viewports abiertos
        for (FEditorViewportClient* ViewportClient : GEditor->GetAllViewportClients())
        {
            if (ViewportClient && ViewportClient->IsLevelEditorClient())
            {
                ViewportClient->SetRealtime(true);  // Esto es lo importante
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Viewports forzados a Realtime al inicio del editor"));
    }
}

void FViewportEditorFixerModule::ShutdownModule()
{
    // Limpieza si hiciera falta
}