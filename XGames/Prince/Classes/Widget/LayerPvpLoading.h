#ifndef LayerPvpLoading_h__
#define LayerPvpLoading_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ui/UIDef.h"
#include "MainModule/MainModel.h"
#include "PvpModule/PvpModel.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const float MOVE_TIME = 0.3f;
class LayerPvpLoading: public cocos2d::LayerColor
{
public:
	LayerPvpLoading();

	~LayerPvpLoading();

	CREATE_FUNC(LayerPvpLoading);

	void setVsInfo(); //设置界面

	void setVsAction(); //设置切入动作

protected:
	virtual bool init();

	virtual void onEnter();

	virtual void onExit();

private:

	Layout *m_pRoot;

	ImageView *m_pMine; //我方
	Vec2 m_pMinePos;
	Text *m_pMineName;
	Text *m_pMineForce;

	ImageView *m_pVs;

	ImageView *m_pEnemy; //敌方
	Vec2 m_pEnemyPos;
	Text *m_pEnemyName;
	Text *m_pEnemyForce;
};

#endif // LayerPvpLoading_h__
