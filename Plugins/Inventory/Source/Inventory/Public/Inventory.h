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
#define _LOG_CONCAT(A, B) A B

#define LOG_NETFUNCTIONCALL  \
    UE_LOG(LogInventory, Log, TEXT("[%s] [%s] --> (%s) %s"), NETMODE_WORLD, *GetName(), GetNetRoleText(GetLocalRole()), FUNC_NAME)

#define LOG_NETFUNCTIONCALL_MSG(format, ...) \
    UE_LOG(LogInventory, Log, _LOG_CONCAT(TEXT("[%s] [%s] --> (%s) %s: "), format), NETMODE_WORLD, *GetName(), GetNetRoleText(GetLocalRole()), FUNC_NAME, ##__VA_ARGS__)

#define LOG_NETFUNCTIONCALL_W  \
    UE_LOG(LogInventory, Log, TEXT("[%s] [%s] --> %s"), NETMODE_WORLD, *GetName(), FUNC_NAME)

#define LOG_NETFUNCTIONCALL_W_MSG(format, ...) \
    UE_LOG(LogInventory, Log, _LOG_CONCAT(TEXT("[%s] [%s] --> %s: "), format), NETMODE_WORLD, *GetName(), FUNC_NAME, ##__VA_ARGS__)

#define LOG_NETFUNCTIONCALL_COMPONENT \
    if (auto ComponentOwner = GetOwner()) \
    { UE_LOG(LogInventory, Log, TEXT("[%s] [%s.%s] --> (%s) %s"), NETMODE_WORLD, *ComponentOwner->GetName(), *GetName(), GetNetRoleText(ComponentOwner->GetLocalRole()), FUNC_NAME); } 

#define LOG_NETFUNCTIONCALL_COMPONENT_MSG(format, ...) \
    if (auto ComponentOwner = GetOwner()) \
    { UE_LOG(LogInventory, Log, _LOG_CONCAT(TEXT("[%s] [%s.%s] --> (%s) %s: "), format) , NETMODE_WORLD, *ComponentOwner->GetName(), *GetName(), GetNetRoleText(ComponentOwner->GetLocalRole()), FUNC_NAME, ##__VA_ARGS__); }

#define LOG_NETFUNCTIONCALL_OWNER(OwningActor) \
    { UE_LOG(LogInventory, Log, TEXT("[%s] [%s.*] --> (%s) %s"), NETMODE_WORLD_A(OwningActor), *OwningActor->GetName(), GetNetRoleText(OwningActor->GetLocalRole()), FUNC_NAME); }

#define LOG_NETFUNCTIONCALL_OWNER_MSG(OwningActor, format, ...) \
    { UE_LOG(LogInventory, Log, _LOG_CONCAT(TEXT("[%s] [%s.*] --> (%s) %s: "), format), NETMODE_WORLD_A(OwningActor), *OwningActor->GetName(), GetNetRoleText(OwningActor->GetLocalRole()), FUNC_NAME, ##__VA_ARGS__); }

#define NETMODE_WORLD NETMODE_WORLD_TEXT(GetWorld())
#define NETMODE_WORLD_A(A) NETMODE_WORLD_TEXT(A->GetWorld())

#define NETMODE_WORLD_TEXT(World) GetNetModeText(World)

constexpr const TCHAR* GetNetModeText(const UWorld* World)
{
    if (World != nullptr)
    {
        switch (World->GetNetMode())
        {
        case NM_Standalone: return TEXT("Standalone");
        case NM_DedicatedServer: return TEXT("DedicatedServer");
        case NM_ListenServer: return TEXT("ListenServer");
        case NM_Client: return TEXT("Client");
        default:;
        }
    }
    return TEXT("");
}

constexpr const TCHAR* GetNetRoleText(const ENetRole Role)
{
    switch (Role)
    {
    case ROLE_None: return TEXT("None");
    case ROLE_SimulatedProxy: return TEXT("SimulatedProxy");
    case ROLE_AutonomousProxy: return TEXT("AutonomousProxy");
    case ROLE_Authority: return TEXT("Authority");
    default: return TEXT("Undefined?");
    }
}