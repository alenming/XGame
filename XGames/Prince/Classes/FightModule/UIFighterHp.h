#ifndef UIFighterHp_h__
#define UIFighterHp_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "FightStateMachine.h"

class UIFighterHp: public cocos2d::Node
{
public:
	UIFighterHp();
	~UIFighterHp();

	static UIFighterHp* create(FightStateMachine::E_SIDE side, int heroType = -1);

public:
	
	virtual	bool init(FightStateMachine::E_SIDE side, int heroType);

	virtual	void onEnter();

	virtual	void onExit();

	virtual void setPercent(float fPercent);

	virtual	float getPercent();

private:

	// 血条bar
	cocos2d::ui::LoadingBar* m_pBar;

	// 血条背景
	cocos2d::Sprite* m_pBg;			

};

#endif // UIFighterHp_h__
