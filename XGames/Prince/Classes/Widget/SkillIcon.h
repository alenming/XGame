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

	//��ť�����Ӧ
	void onBtnClicked(Ref* ref, Widget::TouchEventType type);

	//�����Ƿ���ʾ������ʾ
	void setShowTips(bool showTips);

	//���ð�ť�Ƿ����Ӧ
	void setBtnEnable(bool enable);

private:

	int mTempletId;

	//�߿�ť
	Button* mBorderBtn;

	//�߿�ͼƬ
	ImageView* mIconImg;

	//�Ƿ���ʾ��ʾ
	bool mShowTips;
};

#endif //SkillIcon_h__