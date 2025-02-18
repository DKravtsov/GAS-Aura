// Copyright (c) Dmytro "Duke" Kravtsov

#pragma once

#include "CoreMinimal.h"

// this should be declared before including this header. (also, don't forget to define them)
DECLARE_LOG_CATEGORY_EXTERN(LogMyGame, Log, All);

#if _MSC_VER
#define FUNC_NAME    *FString(__FUNCTION__)
#else // FIXME - GCC?
#define FUNC_NAME    TEXT(__func__)
#endif

//#define LOG_LINE             UE_LOG(LogMyGame, Log, TEXT("%s, file: %s, line %d"), FUNC_NAME, *FString(__FILE__), __LINE__);

#define LOG(format, ...) \
    UE_LOG(LogMyGame, Warning, format, ##__VA_ARGS__)

#define LOG_DEBUG(format, ...) \
    UE_LOG(LogMyGame, Log, format, ##__VA_ARGS__)

#define LOG_FUNCTIONCALL \
    UE_LOG(LogMyGame, Warning, TEXT("--> %s"), FUNC_NAME)

#define LOG_FUNCTIONCALL_MSG(format, ...) \
    UE_LOG(LogMyGame, Warning, TEXT("--> %s: %s"), FUNC_NAME, *FString::Printf(format, ##__VA_ARGS__))

#define LOG_ERROR(format, ...) \
    UE_LOG(LogMyGame, Error, TEXT("--> %s: %s"), FUNC_NAME, *FString::Printf(format, ##__VA_ARGS__))

#define LOG_FUNCTIONCALL_A \
    UE_LOG(LogMyGame, Warning, TEXT("[%s] --> %s"), *GetName(), FUNC_NAME)

#define LOG_FUNCTIONCALL_A_MSG(format, ...) \
    UE_LOG(LogMyGame, Warning, TEXT("[%s] --> %s: %s"), *GetName(), FUNC_NAME, *FString::Printf(format, ##__VA_ARGS__))

#define LOG_NETFUNCTIONCALL  \
    UE_LOG(LogMyGame, Warning, TEXT("[%s] --> (%s) %s"), *GetName(), NETROLE_TEXT(GetLocalRole()), FUNC_NAME)

#define LOG_NETFUNCTIONCALL_MSG(format, ...) \
    UE_LOG(LogMyGame, Warning, TEXT("[%s] --> (%s) %s: %s"), *GetName(), NETROLE_TEXT(GetLocalRole()), FUNC_NAME, *(FString::Printf(format, ##__VA_ARGS__)))

#define LOG_NETFUNCTIONCALL_COMPONENT \
    if (auto ComponentOwner = GetOwner()) \
    { UE_LOG(LogMyGame, Warning, TEXT("[%s.%s] --> (%s) %s"), *ComponentOwner->GetName(), *GetName(), NETROLE_TEXT(ComponentOwner->GetLocalRole()), FUNC_NAME); } \

#define LOG_NETFUNCTIONCALL_COMPONENT_MSG(format, ...) \
    if (auto ComponentOwner = GetOwner()) \
    { UE_LOG(LogMyGame, Warning, TEXT("[%s.%s] --> (%s) %s: %s"), *ComponentOwner->GetName(), *GetName(), NETROLE_TEXT(ComponentOwner->GetLocalRole()), FUNC_NAME, *(FString::Printf(format, ##__VA_ARGS__))); }

#define LOG_FUNCTION_SCOPED   \
    LogPrivateNamespace::ScoppedLogger ScoppedLogger__ {FUNC_NAME};

#define LOG_NETFUNCTION_SCOPED   \
    LogPrivateNamespace::ScoppedLogger ScoppedLogger__ {FUNC_NAME, this};



#define LOG_CATEGORY(Category, format, ...) \
{ \
    const FString sMsg = FString::Printf(format, ##__VA_ARGS__); \
    UE_LOG(Category, Log, TEXT("%s : %s"), FUNC_NAME, *sMsg);\
}

#define PRINT(format, ...) \
{ \
    const FString sText = FString::Printf(format, ##__VA_ARGS__); \
    const FString sMsg = FString::Printf(TEXT("%s : %s"), FUNC_NAME, *sText); \
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, sMsg); \
}

#define PRINT_ERROR(format, ...) \
{ \
    const FString sText = FString::Printf(format, ##__VA_ARGS__); \
    const FString sMsg = FString::Printf(TEXT("Error: %s : %s"), FUNC_NAME, *sText); \
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sMsg); \
    UE_LOG(LogTemp, Error, TEXT("%s : %s"), FUNC_NAME, *sMsg);\
}

// prints colored message on screen
#define PRINT_MSG_C(color, format, ...) \
{ \
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, color, FString::Printf(format, ##__VA_ARGS__)); \
}

#define ShowDebugMessage if (GEngine) GEngine->AddOnScreenDebugMessage

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

namespace LogPrivateNamespace
{
    struct ScoppedLogger
    {
        FString FuncName;

        ScoppedLogger(FString&& InFuncName, AActor* Actor = nullptr) : FuncName(InFuncName)
        {
            if (Actor == nullptr)
            {
                UE_LOG(LogMyGame, Warning, TEXT("--> %s - BEGIN"), *FuncName);
            }
            else
            {
                UE_LOG(LogMyGame, Warning, TEXT("[%s] --> (%s) %s - BEGIN"), *Actor->GetName(), NETROLE_TEXT(Actor->GetLocalRole()), *FuncName);
            }
        }
        ~ScoppedLogger()
        {
            UE_LOG(LogMyGame, Warning, TEXT("--> %s - END"), *FuncName);
        }
    };
}

