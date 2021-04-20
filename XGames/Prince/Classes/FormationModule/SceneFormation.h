/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		SceneFormation
 *  Description:	阵型的主场景UI;
 *  Version:		1.0
 *  Author:			phil
 *  Date:			09/09/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef SceneFormation_h__
#define SceneFormation_h__

#include "FormationDef.h"
#include "MainModule/MainCityScene.h"

class SceneFormation : public Scene
{
public:
	SceneFormation(void);
	~SceneFormation(void);

	static SceneFormation* create(Formation_Mode mode = F_MODE_FORMATION);

	virtual bool  init(Formation_Mode mode);

	virtual void  update(float delta);

	virtual void  onBtnClose();

private:

	void  initUI(Formation_Mode mode);

private:

	Formation_Mode m_mode;
};

#endif // SceneFormation_h__
