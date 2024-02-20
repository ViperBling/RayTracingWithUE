// Copyright Epic Games, Inc. All Rights Reserved.

#include "RayTracingWithUE.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FRayTracingWithUEModule"

void FRayTracingWithUEModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("RayTracingWithUE"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugins/RayTracingWithUE"), PluginShaderDir);
}

void FRayTracingWithUEModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRayTracingWithUEModule, RayTracingWithUE)