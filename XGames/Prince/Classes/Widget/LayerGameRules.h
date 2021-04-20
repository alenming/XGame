
#ifndef LayerGameRules_h__
#define LayerGameRules_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "FormationModule/CustomScrollBar.h"

USING_NS_CC;
using namespace cocos2d::ui;

//所有规则ID
typedef enum RuleId
{
	PVP_ARENA_RULE_ID = 1001,						//竞技场
	SHI_LIAN_RULE_ID,								//试练塔
	LOVE_RULE_ID,									//情缘
	SIGN_RULE_ID,									//签到
	BOSS_RULE_ID,									//BOSS
	STARSCAMP_RULE_ID,								//星盘
	GROUP_RULE_ID,									//帮派
	LIMIT_TIME_HERO_CARD_RULE_ID,					//限时神将(抽卡)
	LIMIT_TIME_HERO_BOX_RULE_ID,					//限时神将(抽宝匣)
	GROUP_ACTIVITY_RULE=1010								//帮派寻龙活动
}eRuleId;


typedef struct labelAndPlayDes
{
	string labelDes;
	vector<string> playDes;

}sLabelAndPlayDes;

class LayerGameRules : public ModalLayer
{
public:
	LayerGameRules();
	~LayerGameRules();

public:
	//使用时，只需要传入玩法规则表中第一项id就可以了
	static LayerGameRules* create(int ruleTableId);
	
private:
	virtual bool initWithId(int ruleTableId);
	void initUI();
	
private:
	//根据规则id分别解析标签描述和玩法描述
	void parseDesByRuleId(int ruleTableId);

	void initScrollPanel();

	//计算文本区域大小
	Size getContentTextSize();

	//设置内容
	void setContent();

	void onBtnClick( Ref* ref, Widget::TouchEventType type);

	//滚动层事件监听
	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

private:
	//布局根节点
	Layout* m_pRoot;

	//滚动面板
	ui::ScrollView* m_scrollPanel;

	//规则标题
	Text* m_pTitleTxt;

	//规则说明
	Text* m_pText;

	//滚动条
	CustomScrollBar* m_pScrollBar;

	//标签和玩法描述
	vector<sLabelAndPlayDes> m_vecDes;
};

#endif