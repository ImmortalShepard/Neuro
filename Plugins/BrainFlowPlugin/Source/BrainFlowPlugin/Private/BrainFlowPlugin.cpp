// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BrainFlowPlugin.h"

#define LOCTEXT_NAMESPACE "FBrainFlowPluginModule"
DEFINE_LOG_CATEGORY_STATIC(LogBrainFlow, Log, All);

void FBrainFlowPluginModule::StartupModule()
{
  // This code will execute after your module is loaded into memory; the exact
  // timing is specified in the .uplugin file per-module
  UE_LOG(LogBrainFlow, Log, TEXT("BrainFlow Plugin loaded!"));
}

void FBrainFlowPluginModule::ShutdownModule() {
  // This function may be called during shutdown to clean up your module.  For
  // modules that support dynamic reloading, we call this function before
  // unloading the module.
  UE_LOG(LogBrainFlow, Log, TEXT("BrainFlow Helper Plugin unloaded!"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBrainFlowPluginModule, BrainFlowPlugin)