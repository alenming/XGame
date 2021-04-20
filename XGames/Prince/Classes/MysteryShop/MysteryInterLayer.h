#ifndef MysteryInterLayer_h_
#define MysteryInterLayer_h_


#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/SoundUtils.h"
#include "MysteryShopDef.h"

using namespace cocos2d;
using namespace cocos2d::ui;

enum MYSTERY_INTER_TYPE
{
	MYSTERY_DEFAULT = 0,
	MYSTERY_RUN,
	MYSTERY_WAIT
};

struct UI_MysteryInter
{
	Layout		*ui_Root;		
	ImageView	*Image_BackGround;
	Button		*bRun;
	Button		*bWait;
};

class MysteryInterLayer:public Layer
{
public:
	~MysteryInterLayer();
	 static MysteryInterLayer* create();
protected:
	virtual bool init();
	virtual void onEnter() override;
	virtual void onExit() override;

	MysteryInterLayer();
private:
	void initUi();

	void updateNextFram(float dt);

	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type, MYSTERY_INTER_TYPE goType);


private:
	UI_MysteryInter m_ui;

};
#endif //ShopScene_h_
