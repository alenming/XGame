#ifndef VipScene_h__
#define VipScene_h__

#include "cocos2d.h"
#include "VipLayer.h"
#include "VipDataDef.h"

class VipScene:public cocos2d::Scene
{

public:
	VipScene();
	~VipScene();

	//CREATE_FUNC(VipScene);

	//param1.默认展示的面板(vip/充值), param2.需要切换到的vip等级，默认会取玩家当前viplevel
	static VipScene* createScene(Panel_Type type = VIP_VIEW, int vipLevel = -1);

protected:
	virtual bool init(Panel_Type type, int vipLevel);
	virtual void onExit();
	virtual void onEnter();

private:
	VipLayer *m_pVipLayer;
};



#endif
