#pragma once

struct SceneNode;

class Scene
{
public:
	Scene();
	~Scene();

	void Add(SceneNode* node);

private:
	vector<SceneNode*> nodes;
};
