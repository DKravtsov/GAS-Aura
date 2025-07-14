// Copyright 4sandwiches

#pragma once

#include "Inventory.h"
#include "Blueprint/UserWidget.h"

#define FUNC_NAME    *FString(__FUNCTION__)


#define LOG_NETFUNCTIONCALL  INTERNAL_NETFUNCTIONCALL_1(this)
#define LOG_NETFUNCTIONCALL_MSG(format, ...) INTERNAL_NETFUNCTIONCALL_1V(this, format, ##__VA_ARGS__)

#define LOG_NETFUNCTIONCALL_STRUCT(OwningObject, StructName) INTERNAL_NETFUNCTIONCALL_2(OwningObject, TEXT(#StructName))
#define LOG_NETFUNCTIONCALL_STRUCT_MSG(OwningObject, StructName, format, ...) INTERNAL_NETFUNCTIONCALL_IMPL(OwningObject, TEXT(#StructName), format, ##__VA_ARGS__)

inline void DebugPrint(const FString& Message, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(InKey, 7.f, Color, Message);
        UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
    }
}

FString GetInventoryItemId(const class UInventoryItem* Item);




/*
 *  Internal stuff
 */

#define INTERNAL_NETFUNCTIONCALL_1(Obj) INTERNAL_NETFUNCTIONCALL_IMPL(Obj, nullptr, )  
#define INTERNAL_NETFUNCTIONCALL_2(Obj, StructName) INTERNAL_NETFUNCTIONCALL_IMPL(Obj, StructName, )  
#define INTERNAL_NETFUNCTIONCALL_1V(Obj, format, ...) INTERNAL_NETFUNCTIONCALL_IMPL(Obj, nullptr, format, ##__VA_ARGS__)
#define INTERNAL_NETFUNCTIONCALL_IMPL(Obj, NameSuffix, format, ...) \
    { const AActor* Outer_Actor = nullptr; const FString Path_Name = DebugHelper::GetCallerPathAndOwner(Obj, Outer_Actor, NameSuffix);\
    UE_LOG(LogInventory, Log, INTERNAL_CONCAT(TEXT("[%s] [%s] [%s] [%s] "), format), \
    DebugHelper::GetNetModeText(Outer_Actor->GetWorld()), DebugHelper::GetNetRoleText(DebugHelper::GetNetRole(Outer_Actor)), *Path_Name, FUNC_NAME, ##__VA_ARGS__); }
#define INTERNAL_CONCAT(A, B) A B

namespace DebugHelper
{
    FORCEINLINE ENetRole GetNetRole(const AActor* Actor) { return Actor ? Actor->GetLocalRole() : ROLE_None; }
    FORCEINLINE ENetRole GetNetRole(const UUserWidget* Widget) { return Widget ? Widget->GetOwningPlayer()->GetLocalRole() : ROLE_None; }

    FString GetCallerPathAndOwner(const UObject* Object, const AActor* &OwnerActor, const TCHAR* Extra = nullptr);
    FString GetCallerPathAndOwner(const UUserWidget* Object, const AActor* &OwnerActor, const TCHAR* Extra = nullptr);
    
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
}