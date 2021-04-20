#ifndef UIComboItem_h__
#define UIComboItem_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CCArmature.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;
using namespace cocostudio;

class UIComboItem : public Node
{

public:

	UIComboItem();
	~UIComboItem();

	static UIComboItem* create(bool isLeft);

public:

	//初始化
	virtual bool init(bool isLeft);

	//设置Combo数量
	void setComboCount(int comboCount);

	//增加Combo数量
	void addComboCount();

private:

	//获取动画名称
	string getAnimationName();

private:

	//当前Combo数量
	int mComboCount;

	//combo数字组件
	TextAtlas* mNumberAtlas;

	//combo动画
	Armature* mComboAnimation;

	//combo动画方向
	bool mIsLeft;
};


#endif //UIComboItem_h__