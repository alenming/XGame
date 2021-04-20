#ifndef UIFighterDamage_h__
#define UIFighterDamage_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "Skill/SkillResultParam.h"

USING_NS_CC;
using namespace ui;

enum DAMAGE_TEXT_TYPE
{
	NORMAL_TEXT,
	CURE_TEXT,
	CRIT_TEXT,
	DODGE_TEXT,
	RESTRICTION_TEXT
};

class UIFighterDamage: public Node
{

public:

	UIFighterDamage();
	~UIFighterDamage();

	// 在UIFighter上添加伤害节点，显示伤害或加血数字，直接调用该静态方法，自动释放
	static UIFighterDamage* create(int damage, int harmType = HarmParam::HARM_NORMAL);

public:

	//初始化
	bool init(int damage, int harmType);

	//启动动画
	virtual void onEnterTransitionDidFinish();

private:

	//动画播放结束，移除自身
	virtual	void onActionFinish();

	//创建伤害数值
	void createDamageText(int harmType, int damage);

	//获取伤害数字
	TextAtlas* getDamageNumAtlas(int harmType, int damage);

	//获取伤害
	ImageView* getDamageTextImg(int harmType);

private:

	//缩放值
	float mScaleValue;

private:
};

#endif // UIFighterDamage_h__
