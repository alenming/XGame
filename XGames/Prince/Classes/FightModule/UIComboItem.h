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

	//��ʼ��
	virtual bool init(bool isLeft);

	//����Combo����
	void setComboCount(int comboCount);

	//����Combo����
	void addComboCount();

private:

	//��ȡ��������
	string getAnimationName();

private:

	//��ǰCombo����
	int mComboCount;

	//combo�������
	TextAtlas* mNumberAtlas;

	//combo����
	Armature* mComboAnimation;

	//combo��������
	bool mIsLeft;
};


#endif //UIComboItem_h__