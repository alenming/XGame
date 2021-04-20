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

	//��ʼ��
	virtual bool init(int buffId, int buffType, int resId, bool isOnce, bool isFlip);

	//��ȡbuffId
	int getBuffId();

	//��ȡbuff����
	int getBuffType();

	//����һ����Ч��
	void playOnceEffect();

	//���ų�����Ч��
	void playCircleEffect();

private:

	//buffId
	int mBuffId;

	//buff����
	int mBuffType;

	//buffЧ��
	Armature* mBuffEffect;
};

class UIBuffDelegate : public Ref
{

public:

	UIBuffDelegate();
	virtual ~UIBuffDelegate();

	static UIBuffDelegate* create(UIFighter* fighter);

public:

	//��ʼ��
	bool init(UIFighter* fighter);

	//����buff
	void updateBuff(BuffUpdateParam* buffUpdateParam);

	//�Ƴ�����buff
	void removeAllBuff();

	//buffЧ���Ƿ����
	UIBuff* isUIBuffExist(int buffType);

private:

	//buff����Ŀ��
	UIFighter* mUIFighter;

	//������buff�б�
	vector<UIBuff*> mVecUIBuff;


	

};

#endif