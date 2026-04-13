#pragma once
#include "Scene.h"


Event<Scene::GameMode> Scene::on_exit_requested;
int Scene::score_ = 0;
Scene::~Scene()
{}