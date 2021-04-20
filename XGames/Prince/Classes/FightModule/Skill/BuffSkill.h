#ifndef BuffSkill_h__
#define BuffSkill_h__

#include "AbstractSkill.h"

//�ü����ѷ���
class BuffSkill : public AbstractSkill
{

public:

	BuffSkill();
	virtual ~BuffSkill();

	//��������
	static BuffSkill* create(int skillId);

public:

	//ͨ������id��ʼ������
	virtual bool init(int skillId);

	//�����ߴ����Ĺ����¼�
	void onAttack(SkillResultParam* resultParam, Fighter* attacker, Fighter* target);

	//�������ߴ����ı����¼�
	void onBeaten(SkillResultParam* resultParam, Fighter* attacker, Fighter* target);

	//��������
	ENCRYPT_INT_GETTER_SETTER(BuffSourceType, mBuffSourceType);

public:

	enum BUFF_SOURCE_TYPE
	{
		SOURCE_ATTACK = 1,	//��������
		SOURCE_BEATEN		//��������
	};

private:

	//����2buff��Դ
	int mBuffSourceType;

};

#endif //BuffSkill_h__