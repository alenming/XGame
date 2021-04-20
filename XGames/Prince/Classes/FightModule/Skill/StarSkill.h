#ifndef StarSkill_h__
#define StarSkill_h__

#include "AbstractSkill.h"

class StarSprite;

class StarSkill : public AbstractSkill
{
public:

	StarSkill();
	virtual ~StarSkill();

	static StarSkill* create(int skillId);

public:

	//ͨ������id��ʼ������
	virtual bool init(int skillId) override;

	//�����׶�
	virtual void onAttack() override;

	//��������
	void setStarSprite(StarSprite* starSprite);

	//��Ӿ���ŭ��
	void addSpriteAG();

	//�Ƿ�ɷ�������
	bool isReady();

private:

	//������ŭ��ֵ
	int mMaxSpriteAg;

	//���鵱ǰŭ��ֵ
	int mCurSpriteAg;

	//��������
	StarSprite* mStarSprite;

};



#endif //StarSkill_h__