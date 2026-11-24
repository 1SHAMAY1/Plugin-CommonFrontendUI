#pragma once

#include "Modules/ModuleManager.h"

/**
 * FCommonFrontendUIModule
 *
 * Implements the core module interface for the CommonFrontendUI plugin.
 * Handles module startup and shutdown lifecycle events.
 */
class FCommonFrontendUIModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
