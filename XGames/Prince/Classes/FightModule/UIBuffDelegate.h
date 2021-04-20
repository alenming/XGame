#ifndef UIBuffDelegate_h__
#define UIBuffDelegate_h__

#include "cocos2d.h"
#include "UIFighter.h"
#include "CocoStudio.h"
#include "CCArmature.h"

USING_NS_CC;
using namespace cocostudio;

class UIBuff : public Node
{

public:

	UIBuff();
	virtual ~UIBuff();

	static UIBuff* create(int buffId, int buffType, int resId, bool isOnce, bool isFlip);

public:

	//初始化
	virtual bool init(int buffId, int buffType, int resId, bool isOnce, bool isFlip);

	//获取buffId
	int getBuffId();

	//获取buff类型
	int getBuffType();

	//播放一次性效果
	void playOnceEffect();

	//播放持续性效果
	void playCircleEffect();

private:

	//buffId
	int mBuffId;

	//buff类型
	int mBuffType;

	//buff效果
	Armature* mBuffEffect;
};

class UIBuffDelegate : public Ref
{

public:

	UIBuffDelegate();
	virtual ~UIBuffDelegate();

	static UIBuffDelegate* create(UIFighter* fighter);

public:

	//初始化
	bool init(UIFighter* fighter);

	//更新buff
	void updateBuff(BuffUpdateParam* buffUpdateParam);

	//移除所有buff
	void removeAllBuff();

	//buff效果是否存在
	UIBuff* isUIBuffExist(int buffType);

private:

	//buff附加目标
	UIFighter* mUIFighter;

	//持续性buff列表
	vector<UIBuff*> mVecUIBuff;


	

};

#endif