#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
// EmergeCore: shared types, subsystems, save/backend interfaces (spec 11.4). One concern per module;
// other modules reach in ONLY through EmergeCore interfaces.
class FEmergeCoreModule : public IModuleInterface {};
