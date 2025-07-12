// Copyright 4sandwiches

#pragma once

#include "Inventory.h"

#define FUNC_NAME    *FString(__FUNCTION__)
#define _LOG_CONCAT(A, B) A B

#define LOG_NETFUNCTIONCALL  \
    UE_LOG(LogInventory, Log, TEXT("[%s] [%s] --> (%s) %s"), NETMODE_WORLD, *GetName(), GetNetRoleText(GetLocalRole()), FUNC_NAME)

#define LOG_NETFUNCTIONCALL_MSG(format, ...) \
    UE_LOG(LogInventory, Log, _LOG_CONCAT(TEXT("[%s] [%s] --> (%s) %s: "), format), NETMODE_WORLD, *GetName(), GetNetRoleText(GetLocalRole()), FUNC_NAME, ##__VA_ARGS__)

#define LOG_NETFUNCTIONCALL_W  \
    UE_LOG(LogInventory, Log, TEXT("[%s] [%s:w %s] --> %s"), NETMODE_WORLD, *GetOwningPlayer()->GetName(), *GetName(), FUNC_NAME)

#define LOG_NETFUNCTIONCALL_W_MSG(format, ...) \
    UE_LOG(LogInventory, Log, _LOG_CONCAT(TEXT("[%s] [%s:w %s] --> %s: "), format), NETMODE_WORLD, *GetOwningPlayer()->GetName(), *GetName(), FUNC_NAME, ##__VA_ARGS__)

#define LOG_NETFUNCTIONCALL_COMPONENT \
    if (auto ComponentOwner = GetOwner()) \
    { UE_LOG(LogInventory, Log, TEXT("[%s] [%s.%s] --> (%s) %s"), NETMODE_WORLD, *ComponentOwner->GetName(), *GetName(), GetNetRoleText(ComponentOwner->GetLocalRole()), FUNC_NAME); } 

#define LOG_NETFUNCTIONCALL_COMPONENT_MSG(format, ...) \
    if (auto ComponentOwner = GetOwner()) \
    { UE_LOG(LogInventory, Log, _LOG_CONCAT(TEXT("[%s] [%s.%s] --> (%s) %s: "), format) , NETMODE_WORLD, *ComponentOwner->GetName(), *GetName(), GetNetRoleText(ComponentOwner->GetLocalRole()), FUNC_NAME, ##__VA_ARGS__); }

#define LOG_NETFUNCTIONCALL_STRUCT(OwningActor, StructName) \
    { UE_LOG(LogInventory, Log, TEXT("[%s] [%s.%s] --> (%s) %s"), NETMODE_WORLD_A(OwningActor), *OwningActor->GetName(), (StructName), GetNetRoleText(OwningActor->GetLocalRole()), FUNC_NAME); }

#define LOG_NETFUNCTIONCALL_STRUCT_MSG(OwningActor, StructName, format, ...) \
    { UE_LOG(LogInventory, Log, _LOG_CONCAT(TEXT("[%s] [%s.*] --> (%s) %s: "), format), NETMODE_WORLD_A(OwningActor), *OwningActor->GetName(), (StreuctName), GetNetRoleText(OwningActor->GetLocalRole()), FUNC_NAME, ##__VA_ARGS__); }

#define LOG_NETFUNCTIONCALL_OWNER(OwningActor) \
    { UE_LOG(LogInventory, Log, TEXT("[%s] [%s.%s] --> (%s) %s"), NETMODE_WORLD_A(OwningActor), *OwningActor->GetName(), *GetName(), GetNetRoleText(OwningActor->GetLocalRole()), FUNC_NAME); }

#define LOG_NETFUNCTIONCALL_OWNER_MSG(OwningActor, format, ...) \
    { UE_LOG(LogInventory, Log, _LOG_CONCAT(TEXT("[%s] [%s.%s] --> (%s) %s: "), format), NETMODE_WORLD_A(OwningActor), *OwningActor->GetName(), *GetName(), GetNetRoleText(OwningActor->GetLocalRole()), FUNC_NAME, ##__VA_ARGS__); }

static void DebugPrint(const FString& Message, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(InKey, 7.f, Color, Message);
        UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
    }
}


extern const TCHAR* GetInventoryItemId(const class UInventoryItem* Item);


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


