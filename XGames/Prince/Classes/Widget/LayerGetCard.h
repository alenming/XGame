#ifndef LayerGetCard_h__
#define LayerGetCard_h__

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
#include "Utils/StringFormat.h"
#include "GuideCenter/GuideManager.h"
#include "ToolModule/UICardHead.h"
#include "ResourceManager/ResourceLoader.h"
#include "Utils/SoundUtils.h"
#include "Widget/ActionCreator.h"
#include "Widget/SpineAnimation.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const float _MOVE_TIME = 0.2f;
typedef std::function<void()> getCardCallback;


class LayerGetCard: public ModalLayer
{
public:
	LayerGetCard();

	~LayerGetCard();

	//showHeroId传入英雄id isChangeChip是否分解成碎片 changeChipCount分解成碎片个数  call关闭此界面回调
	static LayerGetCard* create(int showHeroId,bool isChangeChip,int changeChipCount,getCardCallback call = nullptr);

	void showCard();
	void showHeroIntroduction();
	void showHeroHeadRunAction();
	void showHeroAni();

	void setShowHeroSkillInfo(RowData *data,ImageView *Node,Color3B color);
protected:
	bool initUi(int showHeroId,bool isChangeChip,int changeChipCount,getCardCallback call);
	void showResultShowCardsItem();

	//检测获得的新英雄影响到的其它缘分英雄是否存在
	bool checkFatedHeroId();

	virtual void onEnter();

	virtual void onExit();

private:

	getCardCallback m_call;//关闭本界面回调
	int     m_CurShowHeroId;// 卡牌id
	bool    m_isChangeChip; //是否转化碎片
	int     m_ChangChipCount; //转化碎片个数


	Layout *m_pRoot;
	Layout     *pShowChouka;  //展示抽卡
	Layout     *pShowChoukaPrompt;  //抽到英雄提示
	Layout     *pShowChoukaPromptWenzi;
	Text       *pChangChipCount; //转换碎片个数
	Layout     *pShowHero;   //展示的英雄
	Layout     *heroAni; //英雄骨骼动画
	Vec2 pShowHeroPos;//英雄卡牌原始位置
	Vec2 heroAniPos;
	Layout     *pShowCardMask;


	Text       *pTextHeroProduct; //角色简介
	ImageView  *pImgHeroPro;  //角色属性图片

	ImageView  *pImgHeroSkill2; //角色技能图片

	vector<Widget*>pWidget; 
	vector<Vec2> pWidgetPos;

	// 英雄骨骼动画
	SpineAnimation* m_pSkelFighter;
};

#endif // LayerGetCard_h__
