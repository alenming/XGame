#ifndef StarSkill_h__
#define StarSkill_h__

#include "AbstractSkill.h"

class StarSprite;

class StarSkill : public AbstractSkill
{
public:

	StarSkill();
	virtual ~StarSkill();

	static StarSkill* create(int skillId);

public:

	//通过技能id初始化技能
	virtual bool init(int skillId) override;

	//攻击阶段
	virtual void onAttack() override;

	//设置星灵
	void setStarSprite(StarSprite* starSprite);

	//添加精灵怒气
	void addSpriteAG();

	//是否可发动技能
	bool isReady();

private:

	//星灵总怒气值
	int mMaxSpriteAg;

	//星灵当前怒气值
	int mCurSpriteAg;

	//星灵数据
	StarSprite* mStarSprite;

};



#endif //StarSkill_h__