#ifndef GuideLayer_h__
#define GuideLayer_h__

#include "cocos2d.h"
#include "CocoStudio.h"
#include "CocosGUI.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;
using namespace std;

class GuideLayer : public Layer
{

public:

	GuideLayer();
	virtual ~GuideLayer();

	static GuideLayer* create(const string& resName, bool isForceGuide, bool isKeyGuide);

public:

	//��ʼ��
	virtual bool init(const string& resName, bool isForceGuide, bool isKeyGuide);

	//�����¼���Ӧ
	virtual bool onTouchBegan(Touch *touch, Event *unused_event); 
	virtual void onTouchMoved(Touch *touch, Event *unused_event); 
	virtual void onTouchEnded(Touch *touch, Event *unused_event); 
	virtual void onTouchCancelled(Touch *touch, Event *unused_event);

private:

	//��ʼ������
	void initTouch();

	//���Ŷ���
	void playAnimation();

private:

	//�Ƿ�Ϊǿ������
	bool mIsForceGuide;

	//�Ƿ�Ϊ�ؼ����
	bool mIsKeyNode;

	//��������
	Layout* mGuideArea;

	//������Ȧ
	ImageView* mGuideCircle;

	//����ָ��
	ImageView* mGuidePointer;

	//�ü��ɰ�
	Sprite* mStencil;

	//�ü���ͼ
	Sprite* mStencilSp;

	//��������
	Text* mGuideText;

	//�Ƿ�������������
	bool mIsHitGuideArea;

	//������������
	string mLayoutName;
};




#endif //GuideLayer_h__