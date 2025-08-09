/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "WitModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/EngineVersionComparison.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Wit/Utilities/WitLog.h"

#define LOCTEXT_NAMESPACE "FWitModule"

IMPLEMENT_MODULE(FWitModule, Wit)

const FString FWitModule::Name = "Wit";

FWitModule* FWitModule::Singleton = nullptr;

/**
 * Perform module initialization. We initialize our custom HTTP manager that
 * is used to handle streaming Wit.ai requests
 */
void FWitModule::StartupModule()
{
	Singleton = this;
	
	/* We dont need Initialize FCurlHttpManager since we dont have access to it anymore */

	// find version code
	IPluginManager& PluginManager = IPluginManager::Get();
	TArray<TSharedRef<IPlugin>> Plugins = PluginManager.GetDiscoveredPlugins();
	for (const TSharedRef<IPlugin>& Plugin : Plugins)
	{
		if (Plugin->GetName() == Name)
		{
			const FPluginDescriptor& Descriptor = Plugin->GetDescriptor();
			Version = Descriptor.VersionName;
			break;
		}
	}
}

/**
 * Perform module cleanup
 */
void FWitModule::ShutdownModule()
{

	Singleton = nullptr;
}

/**
 * Retrieve singleton access to the module
 */
FWitModule& FWitModule::Get()
{
	if (Singleton == nullptr)
	{
		check(IsInGameThread());
		FModuleManager::LoadModuleChecked<FWitModule>(*Name);
	}
	
	check(Singleton != nullptr);
	return *Singleton;
}

#undef LOCTEXT_NAMESPACE