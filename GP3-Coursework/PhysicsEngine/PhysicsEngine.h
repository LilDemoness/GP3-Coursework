#pragma once

#ifdef PHYSICSENGINE_EXPORTS
#define PHYSICS_API __declspec(dllexport)
#else
#define PHYSICS_API __declspec(dllexport)
#endif

extern "C" PHYSICS_API void HelloWorld();