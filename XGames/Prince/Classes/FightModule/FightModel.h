#ifndef FightModel_h__
#define FightModel_h__

#include "cocos2d.h"
#include "FightStateMachine.h"
#include "base/CCVector.h"
#include "Utils/Observer.h"
#include "Utils/ValueCrypt.h"
#include "Fighter.h"
#include <map>
#include <vector>

USING_NS_CC;

enum SKIP_FLAG 
{
	CANNOT_SKIP = 0,
	CAN_SKIP
};

enum FIGHT_WIN_COND_TYPE
{
	DEFEAT_ALL_ENEMY = 1,	//����ȫ������
	WIN_AND_ALIVE_N,		//ʤ���Ҵ��N��
	WIN_IN_ROUND_N,			//N�غ���ȡʤ
};

enum BG_COND
{
	BG_COND_ROUND,					//�غ�ǰ�����任
	BG_COND_ROUND_BEFORE_ENEMY		//�غϵз�����ǰ
};

//��ս��ɫ����ս��ʱ���ݵĽ�ɫ��Ϣ
struct AssistFighterInfo
{
	FightStateMachine::E_SIDE side;
	int index;
};

class StarSprite;

class FightModel : public Observable
{

private:

	FightModel();
	virtual ~FightModel();

public:

	//��ȡ��ʵ��
	static FightModel* getInstance();

	//���ٵ�ʵ��
	static void destroyInstance();

	//����ҷ���ɫ
 	void addHero(Fighter* hero);

	//��ӵз���ɫ
	void addMonster(Fighter* monster);

	//�����ս��ɫ
	void addAssist(Fighter* fighter);

	//����ҷ���ɫ
	void clearHero();

	//��յз���ɫ
	void clearMonster();

	//�����ս��ɫ
	void clearAssist();

	//��ȡ�ҷ�Ӣ��
	Vector<Fighter*>* getHeroList();
	Fighter* getHeroByIndex(int n);

	//��ȡ�з�Ӣ��
	Vector<Fighter*>* getMonsterList();
	Fighter* getMonsterByIndex(int n);

	//��ȡ��ս�б�
	Vector<Fighter*>* getAssistList();

	//��ȡս����ɫ
	Fighter* getFighter(FightStateMachine::E_SIDE side, int nIndex);

	//��ȡ��ս��ɫ
	Fighter* getAssistFighter(FightStateMachine::E_SIDE side, int nIndex);

	//��ȡ����ս����ɫ
	Fighter* getAliveFighter(FightStateMachine::E_SIDE side, int nIndex);

	//��ȡ���д���ս����ɫ
	void getAllAliveFighter(FightStateMachine::E_SIDE side, vector<Fighter*>& vecAliveFighter);

	//��ȡ�ҷ�����ɫ����
	int getMyAliveFighterCount();

	//��ȡ�ҷ�������ɫ����
	int getMyDeadFighterCount();

	//������н�ɫ�ĸ���״̬(���ա����ˡ�����)
	void clearAllFighterState();

	//�ҷ����ߵз���ɫ�Ƿ�ȫ������
	bool isAllFighterDead(FightStateMachine::E_SIDE side);

	//�ҷ���з���ɫ�Ƿ�ȫ�����
	bool isAllFighterAlive(FightStateMachine::E_SIDE side);

	//������г�Ա��OnRush���
	void clearOnRushFlag();

	//��ȡ��ǰ��HP
	long long getTotalHp(FightStateMachine::E_SIDE side);

	//��ȡ�����HP
	long long getTotalMaxHp(FightStateMachine::E_SIDE side);

	//��ȡ�����SP
	int getTotalMaxSp();

	//�ۼ����н�ɫ��buff�����غ�
	void decreaseAllBuffRound(RoundParam& roundParam);

	//����ҷ�������
	void clearMyHitCount();

	//�����ҷ�������
	void addMyHitCount(int hitCount);

	//��յз�������
	void clearEnemyHitCount();

	//���ӵз�������
	void addEnemyHitCount(int hitCount);

	//�����Ƿ������ս��
	void setSkipFlag(int skipFlag);

	//�Ƿ������ս��
	bool canSkip();

	//��ȡ��ǰ����
	StarSprite* getStarSprite(FightStateMachine::E_SIDE side);

	//���þ���
	void setStarSprite(StarSprite* starSprite, FightStateMachine::E_SIDE side);

	//buffÿ�غϵ�ִ��Ч��
	void onBuffRoundTick(RoundParam& roundParam);

	//����ս��ʤ������
	void setWinCondAndCount(int winCond, int condCount);

	//����ս��������Դid
	void setBgResId(int resId);

	//��ȡս��������Դid
	int getBgResId();

	//��ȡ�ó�ս���ҷ��Եз���ɵ����˺�
	int getTotalDamage();

	//��ȡս����boss�ܵ����˺�(boss��������λ�м�)
	int getBossDamage();

	// ��ȡ�ó�ս����ɶȰٷֱ�(0-100);
	float getCompletionPercent();

	//�����ս��ɫ
	void addAssistFighter(int round, vector<AssistFighterInfo>& vecAssistInfo);

	//����/��ȡ�����л���Դid
	void setChangeBgResId(int bgResId) { mChangeBgResId = bgResId; }
	int getChangeBgResId() { return mChangeBgResId; }

	//����/��ȡ�����л�����
	void setChangeBgCond(int cond) { mChangeBgCond = cond; }
	int getChangeBgCond() { return mChangeBgCond; }

	//����/��ȡ�����л��غ���
	void setChangeBgRound(int round) { mChangeBgRound = round; }
	int getChangeBgRound() { return mChangeBgRound; }

	//�ж��ҷ��Ƿ��ȳ���
	bool isMySideFirst();


	//��������
	ENCRYPT_INT_GETTER_SETTER(MyTotalSp, mMyTotalSp);
	ENCRYPT_INT_GETTER_SETTER(EnemyTotalSp, mEnemyTotalSp);
	ENCRYPT_INT_GETTER_SETTER(MyHitCount, mMyHitCount);
	ENCRYPT_INT_GETTER_SETTER(EnemyHitCount, mEnemyHitCount);
	ENCRYPT_INT_GETTER_SETTER(RoundCount, mRoundCount);
	ENCRYPT_INT_GETTER_SETTER(WinCond, mWinCond);
	ENCRYPT_INT_GETTER_SETTER(WinCondCount, mWinCondCount);
	ENCRYPT_INT_GETTER_SETTER(AssistEnterRound, mAssistEnterRound);
	ENCRYPT_INT_GETTER_SETTER(AssistSide, mAssistSide);
	ENCRYPT_INT_GETTER_SETTER(MyFirstValue, mMyFirstValue);
	ENCRYPT_INT_GETTER_SETTER(EnemyFirstValue, mEnemyFirstValue);
	
private:

	//��ʵ��
	static FightModel* ms_pInstance;

	//�ҷ�Ӣ��
	Vector<Fighter*> mVecHero;

	//�з�Ӣ��
	Vector<Fighter*> mVecMonster;

	//�����Ƿ�������
	bool m_bIsUpdateDone;

	//�ҷ���ŭ��ֵ*
	int mMyTotalSp;

	//�з���ŭ��ֵ*
	int mEnemyTotalSp;

	//�ҷ�������*
	int mMyHitCount;

	//�з�������*
	int mEnemyHitCount;

	//��ǰս���غ���*
	int mRoundCount;

	//�Ƿ������
	int mSkipFlag;

	//˫��������Ϣ
	StarSprite* mMyStarSprite;
	StarSprite* mEnemyStarSprite;

	//ʤ������*
	int mWinCond;

	//ʤ��������������*
	int mWinCondCount;


	//�ҷ��ȹ�ֵ
	int mMyFirstValue;
	//�з��ȹ�ֵ
	int mEnemyFirstValue;

	//////////////////////////////////��ս��////////////////////////////////////////

	//��ս�����ҷ����ǵз�*
	int mAssistSide;

	//��ս���볡�غ���*
	int mAssistEnterRound;

	//��ս��
	Vector<Fighter*> mVecAssistFighter;

	//////////////////////////////////�������////////////////////////////////////////
	//������Դid
	int mBgResId;

	//�л�������Դid
	int mChangeBgResId;

	//�����л�����
	int mChangeBgCond;

	//�����л��غ���
	int mChangeBgRound;
};



#endif // FightModel_h__
