/*************英雄缘分提升通用弹窗**************/

#ifndef LayerHeroRelation_h__
#define LayerHeroRelation_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "HeroRelationModule/HeroRelationDataDef.h"
#include "SmithyModule/SmithyDef.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

//淬炼前道具资源ID计算规则： 英雄templateId*100 + 基数
//淬炼前马书道具资源ID 基数
#define HORSE_ICON_BASE_ID	50

//淬炼前兵书道具资源ID
#define BOOK_ICON_BASE_ID	60

//淬炼前武器道具资源ID
#define WEAPON_ICON_BASE_ID	30

//页面关闭回调
typedef std::function<void()> closeViewCallback;

typedef struct comRelationUI
{
	Layout* pRoot;

	ImageView*	pImgTitle;			//标题		

	Layout* pPnlContent;			//内容父节点
	Layout* pPnlTips;				//缘分提升提示层
	Layout* pPnlSingleHead;			//单个头像展示
	Layout* pPnlDoubleHead;			//两个头像展示
	Text*	pLabOperateTips;		//操作时觉醒还是淬炼

	Layout*	pPnlAttri_1;
	Layout* pPnlAttri_2;
	Layout* pPnlAttri_3;

	Layout* pPnlMask;				//遮罩层

}sComRelationUI;


class LayerHeroRelation : public ModalLayer
{
public:
	LayerHeroRelation();
	~LayerHeroRelation(void);

	//param1. 英雄tempId， param2. 是否觉醒, param3.是否淬炼, param4, 淬炼ID， param5.淬炼类型, param6. 页面回调
	static LayerHeroRelation* create(int heroId, bool isAdv = false, closeViewCallback call = nullptr, bool isEquipCL = false, 
		int cl_ToolId = 0, EQUIP_ITEM_TYPE CL_type = EQUIP_ITEM_TYPE_NONE);
	virtual bool  init(int heroId, bool isAdv = false, closeViewCallback call = nullptr, bool isEquipCL = false, 
		int cl_ToolId = 0, EQUIP_ITEM_TYPE CL_type = EQUIP_ITEM_TYPE_NONE);
	virtual void onEnter();
	virtual void onExit(); 


private:

	void initUI();
	void updateUI();
	
	//更新觉醒信息
	void updateAdvInfo();

	//更新缘分信息
	void updateFateInfo();
	void updateFateDetails();

	//计算前一觉醒等级
	void calculateFrontAdvLv();

	//淬炼触发界面弹窗
	void updateViewByCL();

	//背景动画
	void playBgAni();
	void aniAniCallBack(Armature *armature, MovementEventType movementType, const string& movementID, const string& nextAniName);
private:
	sComRelationUI m_ui;

	int m_iTempId;
	bool m_bIsAdv;
	bool m_bIsEquipCL;			//是否是装备淬炼
	int  m_iCLToolId;			//淬炼道具ID
	EQUIP_ITEM_TYPE m_clType;	//淬炼类型
	closeViewCallback m_call;	//回调类型

	//上一等级缘分属性
	vector<sFateAttri> m_vFrontFatedAttri;

	//英雄上一觉醒等级
	int m_iFrontAdvLv;
};


#endif