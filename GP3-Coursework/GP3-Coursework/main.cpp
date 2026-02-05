#pragma once
#include "MainGame.h"

#include <iostream>

int main(int argc, char** argv) //argument used to call SDL main
{
	MainGame mainGame;
	mainGame.Run();

	return 0;
}