/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		SceneTower
 *  Description:	爬塔主场景;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			11/10/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef SceneTower_h__
#define SceneTower_h__

#include "TowerModule/TowerDef.h"
#include "LayerTowerMain.h"

class SceneTower : public Scene
{
public:

	SceneTower();
	virtual ~SceneTower();

	static SceneTower* create(TOWER_TYPE type = TOWER_TYPE_INVALID, bool pushScene = false);

public:

	virtual void onEnter();

	virtual bool init(TOWER_TYPE type = TOWER_TYPE_INVALID);

	static bool getPushScene() { return mIsPushScene; };

private:
	
	void initUI(TOWER_TYPE type = TOWER_TYPE_INVALID);

private:

	LayerTowerMain* mLayerTowerMain;

	static bool mIsPushScene;
};

#endif // SceneTower_h__