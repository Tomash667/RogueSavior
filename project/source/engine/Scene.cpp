#include "Pch.h"
#include "Core.h"
#include "Scene.h"
#include "SceneNode.h"

Scene::Scene()
{

}

Scene::~Scene()
{
	DeleteElements(nodes);
}

void Scene::Add(SceneNode* node)
{
	assert(node);
	nodes.push_back(node);
}
