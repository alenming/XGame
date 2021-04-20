#ifndef SceneMaze_h
#define SceneMaze_h

#include "cocos2d.h"

class LayerMaze;

class SceneMaze:public cocos2d::Scene
{

public:
	SceneMaze();
	~SceneMaze();

	static SceneMaze* create(bool pushScene = false);


	LayerMaze* getLayerMaze() {return m_pLayerMaze;}; 

	static bool getPushScene() { return m_isPushScene; };

protected:
	bool init(bool pushScene);
	virtual void onExit();
	virtual void onEnter();
private:
	LayerMaze *m_pLayerMaze;

	static bool m_isPushScene;
};



#endif
