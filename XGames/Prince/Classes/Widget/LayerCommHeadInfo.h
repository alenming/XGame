#ifndef LayerCommHeadInfo_h__
#define LayerCommHeadInfo_h__

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
#include "VIPModule/VipScene.h"
#include "Utils/SoundUtils.h"
#include "HeroModule/FateModel.h"
#include "SmithyModule/LayerEquipSource.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const int MAZ_MAP_ID = 10001;
const int KechienDanId = 10015;
const int XilianshiId = 10008;
const int JinNangId = 10071;

enum HEAD_INFO_TYPE
{
	HEAD_INFO_TYPE_PVP, //竞技场
	HEAD_INFO_TYPE_PVPALL, //竞技
	HEAD_INFO_TYPE_LOVE, //情缘
	HEAD_INFO_TYPE_MALL, //商城
	HEAD_INFO_TYPE_BKS_SHOP, //铁匠铺
	HEAD_INFO_TYPE_STAR_CAMP,//星盘
	HEAD_INFO_TYPE_STAR_SU,//星宿
	HEAD_INFO_TYPE_FATE,//主角缘分
	HEAD_INFO_TYPE_HERO_FATE,//英雄缘分
	HEAD_INFO_TYPE_PLUNDER,//掠夺
	HEAD_INFO_TYPE_ACTIVITY_LEVEL,//活动关卡
	HEAD_INFO_TYPE_TOWER,//试练塔
	HEAD_INFO_TYPE_THOUSAND_TOWER,//千层塔
	HEAD_INFO_TYPE_SURVIVAL_TEST,//生存试练
	HEAD_INFO_TYPE_BAG,//背包
	HEAD_INFO_TYPE_ZHENFA,//阵法
	HEAD_INFO_TYPE_ZHENZXING,//阵型
	HEAD_INFO_TYPE_DREAM_LAND,//幻境
	HEAD_INFO_TYPE_REFINING_FURNACE,//炼化炉
	HEAD_INFO_TYPE_GANG,//帮派
	HEAD_INFO_TYPE_ADVENTURE,//冒险
	HEAD_INFO_TYPE_VIP,//vip
	HEAD_INFO_TYPE_MA,//精彩活动
	HEAD_INFO_TYPE_BOSS,//世界BOSS
	HEAD_INFO_TYPE_NEWAREA_ACT,	//新服活动
	HEAD_INFO_TYPE_RANK_BOARD, //排行榜
	HEAD_INFO_TYPE_OPERATE_ACT, //福利大厅
	HEAD_INFO_TYPE_OPERATE_TALENT, //天赋
	HEAD_INFO_TYPE_ARTIFACTS,  //上古神器
	HEAD_INFO_TYPE_STARSTONE,  //星石
	HEAD_INFO_TYPE_STARSTONEGAT,  //星石加特
	HEAD_INFO_TYPE_MARKET, //集市
	HEAD_INFO_TYPE_LIMIT_TIME_HERO, //限时神将
	HEAD_INFO_TYPE_SEABED_DRAGON, //海底寻龙
	HEAD_INFO_TYPE_DAY_DOWN_PAYMENT, //天降金幣
	HEAD_INFO_TYPE_CHALLENGE_RANKING, //挑战排行
	HEAD_INFO_TYPE_BONUS_EXCHANGE, //奖励兑换
	HEAD_INFO_TYPE_MYSTERY_SHOP, //神秘商店
	HEAD_INFO_TYPE_GROUP_BOSS	//帮派boss
};

class LayerCommHeadInfo: public Layer,public Observer
{
public:
	LayerCommHeadInfo();

	~LayerCommHeadInfo();

	static LayerCommHeadInfo* create(Widget::ccWidgetTouchCallback callback,HEAD_INFO_TYPE type);  

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

	bool initUi(Widget::ccWidgetTouchCallback callback,HEAD_INFO_TYPE type); //初始化
	void setInfo();//设置不同模块 不同显示
	void updateInfo();//数值改变做相应更新

	void setHonor(Text *item);
	void setEnergy(Text *item);
	void setGold(Text *item);
	void setPower(Text *item);
	void setCoin(Text *item);
	void setStarNum(Text *item);
	void setMazePower(Text *item);
	void setHunTiandan(Text *item);
	void setJiuZhuandan(Text *item);
	void setKechienDan(Text *item);
	void setGroupMoney(Text *item);
	void setXiLianShi(Text *item);
	void setJinNang(Text *item);

	EaseSineOut* creatScaleBig();
	EaseSineIn* creatScaleSmall();

	virtual void onEnter();
	virtual void onExit();
private:
	HEAD_INFO_TYPE m_type;
	Layout *m_pRoot;

	ImageView *m_title;

	ImageView *m_silverImg;
	ImageView *m_silver;
	Text      *m_silverTex;
	Button    *m_silverBtn;

	ImageView *m_goldImg;
	ImageView *m_gold;
	Text      *m_goldTex;
	Button    *m_goldBtn;

	ImageView *m_energyImg;
	ImageView *m_energy;
	Text      *m_energyTex;
	Button    *m_energyBtn;



	int mHonor; //荣誉
	int mEnergy; //精力
	int mGold; //金币
	int mPower; //体力
	int mCoin; //银币
	int mStarNum; //星星
	int mMazePower; //行动力
	int mHunTiandan; //浑天丹
	int mJiuZhuandan; //九转丹
	int mKechienDan; //结缘丹
	int mGroupMoney; //帮贡
	int mXilianshi; //洗练石
	int mJinNang; //锦囊
};

#endif // LayerCommHeadInfo_h__
