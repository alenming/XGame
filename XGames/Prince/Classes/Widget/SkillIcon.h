#ifndef SkillIcon_h__
#define SkillIcon_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "cocostudio.h"
#include "SkillTips.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;
using namespace std;

class SkillIcon : public Node
{

public:

	SkillIcon();
	virtual ~SkillIcon();

	static SkillIcon* create(int skillTempletId);

public:

	virtual bool init(int skillTempletId);

	//按钮点击响应
	void onBtnClicked(Ref* ref, Widget::TouchEventType type);

	//设置是否显示技能提示
	void setShowTips(bool showTips);

	//设置按钮是否可响应
	void setBtnEnable(bool enable);

private:

	int mTempletId;

	//边框按钮
	Button* mBorderBtn;

	//边框图片
	ImageView* mIconImg;

	//是否显示提示
	bool mShowTips;
};

#endif //SkillIcon_h__