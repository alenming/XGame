#ifndef LayerFightBack_h__
#define LayerFightBack_h__

#include "cocos2d.h"
#include <functional>

USING_NS_CC;
using namespace std;

//�ɰ��ʼ����
const float MASK_START_SCALE = 2.2f;

//��Ч����ϵ��
const float EFFECT_EASE_COE = 2.0f;
//��Ч����ʱ��
const float EFFECT_DURATION = 0.3f;

//Զ��ͼ���š���ת����
const float BACK_IMG_START_SCALE = 1.1f;
const float BACK_IMG_END_SCALE = 1.0f;
const float BACK_IMG_ROTATE_X = 15.0f;

//�ذ�ǰ��ͼ���š���ת����
const float GROUND_FRONT_IMG_ROTATE_X = -10.0f;

//�ذ屳��ͼ���š���ת����
const float GROUND_BACK_IMG_END_SCALE = 0.8f;
const float GROUND_BACK_IMG_ROTATE_X = -10.0f;

//�ذ���ת����
const float GROUND_IMG_ROTATE_X = -30.0f;

//�л���������ʱ��
const float CHANGE_FADE_DURATION = 0.5f;

//ͼƬ����
const string BG_NAME_B2 = "b2.png";
const string BG_NAME_B1 = "b1.png";
const string BG_NAME_F2 = "f2.png";
const string BG_NAME_F1 = "f1.png";
const string BG_NAME_F3 = "f3.png";

typedef function<void()> EffectStartCallBack;
typedef function<void()> EffectEndCallBack;
typedef function<void()> ChangeCallBack;

class LayerFightBack : public Layer
{

public:

	LayerFightBack();
	virtual ~LayerFightBack();

	//����ս��������
	static LayerFightBack* create(int fightBgId, Node* frontParent = nullptr);

public:

	virtual bool init(int fightBgId, Node* frontParent);

	virtual void onEnter();

	virtual void onExit();

	//����3d͸����Ч
	void start3dEffect(const EffectStartCallBack& effectStartCallBack = nullptr);

	//�ز�3d͸����Ч
	void return3dEffect(const EffectEndCallBack& effectEndCallBack = nullptr);

	//�����л�����
	void changeWithFade(int fightBgId, const ChangeCallBack& callBack = nullptr);

	//�����л��ӿ�
	void change(int fightBgId);

private:

	//ս��������Դid
	int mFightBgId;

	//����ͼ
	Sprite* mBackImg;

	//�ذ�ͼ1
	Sprite* mGroundImg;

	//�ذ��ͼ1
	Sprite* mGroundBackImg;

	//�ذ��ͼ2
	Sprite* mGroundFrontImg;

	//ǰ��ͼ2
	Sprite* mGroundFrontImg2;

	//�ɰ�
	Sprite* mMask;

	//ǰ���㸸���
	Node* mFrontParent;

};



#endif //LayerFightBack_h__