#ifndef __ModalLayer__
#define __ModalLayer__

#include "cocos2d.h"
#include "Utils/SoundUtils.h"
USING_NS_CC;

class ModalLayer : public LayerColor
{

public:

	ModalLayer();
	virtual ~ModalLayer();

public:

	//��ʼ��
	virtual bool init();

	//��ʾ
	void show(Node* parent,int zoder = 0);

	//����
	void dispose();

	//�Ƿ����հ���������
	void setClickEmptyDispose(bool clickEmptyDispose);

private:

	//�����¼���Ӧ
	bool onTouchBegan(Touch *touch, Event *event);
	void onTouchEnded(Touch *touch, Event *event);

	//���ٻص�
	void callBackDispose(float dt);

private:

	bool mClickEmptyDispose;

	
};





#endif //ModalLayer__