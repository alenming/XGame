
#ifndef GroupBossSkillTips_h__
#define GroupBossSkillTips_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"

USING_NS_CC;

using namespace cocos2d::ui;	
using namespace std;

typedef struct tag_UISkill
{
	Layout* pRoot;
	Text*	pSkill;
	ImageView*	pImgName;
	Layout*	pPnlDescrip;
	Layout* pPnlCD;

	Text* pLabCD;
	Text* pLabEnterCD;
	Text* pLabNuQi;

}sTag_UISkill;

class GroupBossSkillTips : public ModalLayer 
{
public:
	GroupBossSkillTips();
	~GroupBossSkillTips();
	static GroupBossSkillTips* create(int bossId, int idx,const string str,Color3B color);
public:
	bool init(int bossId, int idx, string str, Color3B color);

	virtual void onEnter();

	virtual void onExit();

	int getSkillIdx() {return m_skillIdx;};
private:
	void  initUI();

private:
	sTag_UISkill m_ui;

	int m_skillIdx;
	int m_bossId;
	string m_strDescrip;
	Color3B m_color;

};

#endif 
