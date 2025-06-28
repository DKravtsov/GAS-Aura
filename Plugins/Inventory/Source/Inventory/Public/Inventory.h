// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

class FInventoryModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define FUNC_NAME    *FString(__FUNCTION__)

#define LOG_NETFUNCTIONCALL  \
    UE_LOG(LogInventory, Warning, TEXT("%s[%s] --> (%s) %s"), NETMODE_WORLD, *GetName(), NETROLE_TEXT(GetLocalRole()), FUNC_NAME)

#define LOG_NETFUNCTIONCALL_MSG(format, ...) \
    UE_LOG(LogInventory, Warning, TEXT("%s[%s] --> (%s) %s: %s"), NETMODE_WORLD, *GetName(), NETROLE_TEXT(GetLocalRole()), FUNC_NAME, *(FString::Printf(format, ##__VA_ARGS__)))

#define LOG_NETFUNCTIONCALL_W  \
    UE_LOG(LogInventory, Warning, TEXT("%s[%s] --> %s"), NETMODE_WORLD, *GetName(), FUNC_NAME)

#define LOG_NETFUNCTIONCALL_W_MSG(format, ...) \
    UE_LOG(LogInventory, Warning, TEXT("%s[%s] --> %s: %s"), NETMODE_WORLD, *GetName(), FUNC_NAME, *(FString::Printf(format, ##__VA_ARGS__)))

#define LOG_NETFUNCTIONCALL_COMPONENT \
    if (auto ComponentOwner = GetOwner()) \
    { UE_LOG(LogInventory, Warning, TEXT("%s[%s.%s] --> (%s) %s"), NETMODE_WORLD, *ComponentOwner->GetName(), *GetName(), NETROLE_TEXT(ComponentOwner->GetLocalRole()), FUNC_NAME); } \

#define LOG_NETFUNCTIONCALL_COMPONENT_MSG(format, ...) \
    if (auto ComponentOwner = GetOwner()) \
    { UE_LOG(LogInventory, Warning, TEXT("%s[%s.%s] --> (%s) %s: %s"), NETMODE_WORLD, *ComponentOwner->GetName(), *GetName(), NETROLE_TEXT(ComponentOwner->GetLocalRole()), FUNC_NAME, *(FString::Printf(format, ##__VA_ARGS__))); }

#define NETMODE_WORLD (((GEngine == nullptr) || (GetWorld() == nullptr)) ? TEXT("") \
        : (GEngine->GetNetMode(GetWorld()) == NM_Client) ? TEXT("[Client] ") \
        : (GEngine->GetNetMode(GetWorld()) == NM_ListenServer) ? TEXT("[ListenServer] ") \
        : (GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer) ? TEXT("[DedicatedServer] ") \
        : TEXT("[Standalone] "))

#define NETROLE_TEXT(Role) \
        ( Role == ROLE_None ? TEXT("None") \
        : Role == ROLE_SimulatedProxy ? TEXT("SimulatedProxy") \
        : Role == ROLE_AutonomousProxy ? TEXT("AutonomousProxy") \
        : Role == ROLE_Authority ? TEXT("Authority") \
        : TEXT("Undefined?"))