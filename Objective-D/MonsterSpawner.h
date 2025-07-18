#pragma once
#include "Scene.h"
#include "ScriptUtil.h"

class MonsterSpawner : public GameObject {
private:
	std::string           currentMapName{};
	ScriptUtil            script{};
	std::vector<XMFLOAT3> position{};
	std::vector<int>      type{};
	float                 currentCreateDelay{};
	int                   currentCreateIndex{-1};
	bool                  editMode{};
	bool                  spawnStarted{};


public:
	MonsterSpawner(const std::string& currentMapName, bool editMode);
	void InputKey(KeyEvent& Event) override;
	void LoadDataAndSpawnMonster();
	void Update(float Delta);
};

