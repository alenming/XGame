#ifndef LayerCommFormation_h__
#define LayerCommFormation_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ui/UIDef.h"
#include "MainModule/MainModel.h"
#include "ui/ModalLayer.h"
#include "ToolModule/UIToolHeader.h"
#include "Utils/ResourceUtils.h"
#include "Common/Common.h"
#include "DataManager/DataManager.h"
#include "PvpModule/PvpHeader.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const float MOVE_TIME = 0.3f;
class LayerCommFormation: public ModalLayer
{
public:
	LayerCommFormation();

	~LayerCommFormation();

	static LayerCommFormation* create(PVP_PLAYER_DATA playerInfo);  

protected:
	bool initUi(PVP_PLAYER_DATA playerInfo);

	virtual void onEnter();

	virtual void onExit();

private:
	Layout *m_pRoot;
};

#endif // LayerPvpLoading_h__
