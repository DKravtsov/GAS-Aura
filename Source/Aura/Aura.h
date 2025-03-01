// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Logs.h"

constexpr int32 CUSTOM_DEPTH_STENCIL_RED = 250;

const FName CollisionProfile_Projectile = FName("Projectile");
const FName CollisionProfile_FireBall = FName("FireBall");

#define ECC_Projectile ECC_GameTraceChannel1
#define ECC_MouseTrace ECC_GameTraceChannel2
#define ECC_MagicCircleTrace ECC_GameTraceChannel3