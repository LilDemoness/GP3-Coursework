#pragma once
#include "Collider.h"

class Collider;

struct Collision
{
public:
	Collider* a;
	Collider* b;
};