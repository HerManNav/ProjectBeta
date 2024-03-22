#pragma once

#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(loc, secs) if (GetWorld()) DrawDebugSphere(GetWorld(), loc, 15.f, 24, FColor(250, 250, 100), false, secs);
#define DRAW_LINE(init, end, secs) if (GetWorld()) DrawDebugLine(GetWorld(), init, end, FColor::Magenta, false, secs, 0, 1.f);
#define DRAW_POINT(loc, secs) if (GetWorld()) DrawDebugPoint(GetWorld(), loc, 50.f, FColor::FromHex("A90098FF"), false, secs);
#define DRAW_VECTOR(init, end, secs) if (GetWorld()) DRAW_LINE(init, end, secs); DRAW_POINT(end, secs)

#define DRAW_SPHERE_1F(loc) if (GetWorld()) DrawDebugSphere(GetWorld(), loc, 30.f, 24, FColor(250, 250, 100), false);
#define DRAW_LINE_1F(init, end) if (GetWorld()) DrawDebugLine(GetWorld(), init, end, FColor::Magenta, false, -1.f, 0, 1.f);
#define DRAW_POINT_1F(loc) if (GetWorld()) DrawDebugPoint(GetWorld(), loc, 50.f, FColor::FromHex("A90098FF"), false);
#define DRAW_VECTOR_1F(init, end) if (GetWorld()) DRAW_LINE_1F(init, end); DRAW_POINT_1F(end)