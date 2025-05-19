// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class FViewportEditorFixerModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

