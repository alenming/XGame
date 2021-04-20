
#ifndef HeroRelationLayer_h__
#define HeroRelationLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "DataManager/DataManager.h"
#include "HeroRelationDataDef.h"
#include "HeroModule/LayerHero.h"


USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

typedef struct uiAttri
{
	//ImageView*	pImgAttriIcon;
	Text*		pLabFateName;
	Text*		pLabFateObj;
	Text*		pLabAttriName;

	uiAttri()
	{
		memset(this, 0, sizeof(uiAttri));
	}
}sUiAttri;

typedef struct fateInfo
{
	ImageView*	pImgFateBg;
	Layout*		pPnlFateObj;
	Button*		pBtnLvUp;
	ImageView*	pImgMaxLv;
	Text*		pLabFateName;
	Text*		pLabFateLv;
	Text*		pLabMaxLv;
	Text*		pLabNextAttri;

	fateInfo()
	{
		memset(this, 0, sizeof(fateInfo));
	}
}sFateInfo;

//UI结构
typedef struct tag_RelationUI
{
	Layout* pRoot;			//根节点

	ImageView* pImgFiveFate;
	ImageView* pImgSixFate;

	Layout*	pPnlHeroBg;

	vector<sUiAttri> vFiveFateAttri; 
	vector<sUiAttri> vSixFateAttri;
	vector<sFateInfo> vFateInfo;
}sTag_RelationUI;

//按钮标记
typedef enum relation_BtnFlag
{
	BTN_BACK,
	BTN_JUMP_CL_HORSE,		//马书淬炼
	BTN_JUMP_CL_BOOK,		//兵书淬炼
	BTN_JUMP_CL_WEAPON,		//武器淬炼
	BTN_JUMP_HERO			//英雄模块界面

}eRelation_BtnFlag;

class HeroRelationLayer : public Layer
{
public:
	HeroRelationLayer();
	~HeroRelationLayer(void);

	static HeroRelationLayer* create(int heroId, bool isPush, LayerHero* layer);
	virtual bool  init(int heroId, bool isPush, LayerHero* layer);
	virtual void onEnter();
	virtual void onExit(); 


private:

	void initUI();
	void onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName, int heroId );
	void update(float dt);
	void updateUI();

	void showFiveFatePnl();
	void showSixFataPnl();

	//获取缘分属性
	void getRelationData();

	//获取属性图标路径
	string getAttriIconPath(string attriEnName);


private:
	sTag_RelationUI m_ui;

	//是否pushScene进来
	bool m_bIsPush;

	//传进的英雄TemplateId
	int m_iTempId;

	LayerHero* m_heroLayer;

	//关联的英雄ID
	vector<sFateHeroOrder> m_vFateHeroId;

	//激活条件
	//vector<sFateHeroOrder> m_vFateLimit;

	//缘分ID列表
	vector<int> m_vFateId;

	//缘分属性列表
	vector<sFateAttri> m_vFateAttri;
};


#endif