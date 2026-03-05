#pragma once

#include "pch.h"
#include "PhysicsEngine.h"
#include <iostream>

extern "C" PHYSICS_API void HelloWorld()
{
	std::cout << "Hello world from Physics Engine DLL" << std::endl;
}