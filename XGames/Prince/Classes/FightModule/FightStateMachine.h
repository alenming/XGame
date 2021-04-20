#ifndef FightStateMachine_h__
#define FightStateMachine_h__

#include "cocos2d.h"
#include <vector>

#define TO_POS_ROW(n)				(n % 3)
#define TO_POS_COL(n)				(n / 3)
#define TO_INDEX(row, col)		((col)*3+(row))

#define ROW_COUNT 3
#define COL_COUNT 3
#define INDEX_COUNT 9
#define MAX_ROUND_COUNT 20

USING_NS_CC;
using namespace std;

class IFightUI;
class AbstractSkill;
class FightModel;
class SkillResultParam;
class AILogic;
class StarSprite;

class FightStateMachine : public Ref
{

public:

	FightStateMachine();
	~FightStateMachine();

	//����
	static FightStateMachine* create();

public:

	//ս��״̬����ʶ
	enum STATE
	{
		FIGHT_START = 0,//ս����ʼ
		ROUND_BEFORE,	//ս���غ�֮ǰ���ڸý׶ε���UI��onRoundStart,��ֹ��غ�ǰ����������ͻ
		ROUND_START,	//�غϿ�ʼ
		TURN_START,		//�ִο�ʼ
		JUDGE_START,	//����ǰ״̬�ж�������Ŀ�꣩
		TARGET_SELECT,	//Ŀ��ѡ��
		ATTACK,			//����Ŀ��
		JUDGE_END,		//������״̬�ж�������Ŀ�꣬������Ŀ�꣩
		TURN_END,		//�ִν���
		TURN_CHANGE,	//���߳���
		ROUND_END,		//�غϽ���
		FIGHT_END,		//ս������
		WAIT			//�ȴ�
	};

	//�ҷ��͵з���ʶ
	enum E_SIDE
	{
		MY_SIDE,
		ENEMY_SIDE
	};

	enum WIN_CONDITION
	{
		ALL_ENEMY_DEAD = 1,
		ALL_MY_ALIVE_AND_WIN,
		ROUND_WIN_3,
		WIN_AND_MY_DEAD_LESS_THAN_OR_EQUAL_TO_1,
	};

	//״̬������
	void start();

	//״̬����ͣ
	void wait();

	//״̬���ָ�
	void resume();

	//����ս��UI
	void setFightUI(IFightUI* fightUI);

	//��ȡ��ǰ����ִ�еļ���
	AbstractSkill* getCurrentSkill();

	//���õ�ǰִ�еļ���
	void setCurrentSkill(AbstractSkill* skill);

	//�����ҷ�Ϊ�Զ�ս��
	void setAutoFight(bool autoFight);

	//����pvpս����pvpս�������߼���pve��ͬ
	void setPVP(bool isPVP);

	//�������غ���
	void setMaxRoundCount(int maxRound);

private:

	//״̬��ִ��
	void execute(float dt);

	//״̬��ִ���߼�
	void fightProcess();
	void onFightStart();
	void onRoundBefore();
	void onRoundStart();
	void onTurnStart();
	void onJudgeStart();
	void onTargetSelect();
	void onAttack();
	void onJudgeEnd();
	void onTurnEnd();
	void onTurnChange();
	void onRoundEnd();
	void onFightEnd();

	//�Ƿ�ȫ������
	bool isTurnsDone(E_SIDE side);

	//�ҷ��Ƿ�ȫ�����ֹ�
	bool isAllMyTurnsDone();

	//�ҷ��Ƿ�ȫ��δ���ֹ�
	bool isAllMyTurnsUnDone();

	//�з��Ƿ�ȫ�����ֹ�
	bool isAllEnemyTurnsDone();

	//�з��Ƿ�ȫ��δ���ֹ�
	bool isAllEnemyTurnsUnDone();

	//����״̬����ǰ״̬
	void setCurrentState(STATE state);

	//��ȡ״̬����ǰ״̬
	STATE getCurrentState();

	//�ж���λ�Ƿ��ڵ�ǰ�غ�׷����
	bool launchedRushTurn(E_SIDE side, int index);

	//�����λ�ѳ���
	void setRushTurn(E_SIDE side, int index);

	//�ж��Ƿ񷢶�����1׷��Ч��
	bool launchRushAttack();

	//�ж��Ƿ񷢶�����3��������Ч��
	bool launchDeadAttack();

	//�ж��Ƿ񷢶�����3����ǰ����Ч��
	bool launchBeforeAttack();

	//�ж��Ƿ񷢶�����3ս��ǰ����Ч��
	bool launchFightStartAttack();

	//�ж��Ƿ񷢶�����3�غ�ǰ����Ч��
	bool launchRoundStartAttack();

	//�ж��Ƿ񷢶�����3ָ���غϹ���Ч��
	bool launchRoundAttack();

	//�ж��Ƿ񷢶�����3�غ����ﵽN�ҵз�����ǰ����
	bool launchRoundBeforeEnemyAttack();

	//�ж��Ƿ񷢶�����3����������
	bool launchHitCountAttack();

	//�ж��Ƿ񷢶����鹥��
	bool launchStarSpriteAttack();

	//״̬���ȴ�
	void timerWait(float dt = 1.0f);

	//��ʱ����״̬��
	void timerResume(float dt);

	//�غϽ���ͳһ����Buff
	void onBuffRoundTick();

	//����������
	void addHitCount(SkillResultParam* resultParam);

	//���������
	void clearHitCount();

	//֪ͨUI���𹥻�
	void notifyAttack(AbstractSkill* skill);

private:

	//״̬����ǰ״̬
	STATE mCurrentState;

	//��һ��״̬�����ڻָ�״̬��ִ��
	STATE mPreviousState;

	//ʤ����
	E_SIDE mWinSide;

	//��ǰ���ַ�
	E_SIDE mCurrentSide;

	//��ǰ������id
	int mAttackerIndex;

	//�غ���ͳ��
	int mRoundCount;

	//�ִ���ͳ��
	int mTurnCount;

	//�ҷ���λ�Ƿ����
	vector<bool> mMyTurns;

	//�з���λ�Ƿ����
	vector<bool> mEnemyTurns;

	//�ҷ��ڸûغ����Ƿ񷢶�������
	vector<bool> mMyRushTurns;

	//�з��ڸûغ����Ƿ񷢶�������
	vector<bool> mEnemyRushTurns;

	//ս��UI
	IFightUI* mFightUI;

	//��ǰ���ּ���
	AbstractSkill* mCurrentSkill;

	//ս����ɫ����ģ��
	FightModel* mFightModel;

	//�з��Զ�ս��AI
	AILogic* mEnemyAILogic;

	//�ҷ��Զ�ս��AI
	AILogic* mMyAILogic;

	//�Ƿ���pvpս����pvpս����������
	bool mIsPVP;

	//PVEս�������غ��Ƿ񻻱߳��ֱ�ǣ��ѳ��ֹ�����Ϊtrue
	bool mIsChangeSide;

	//���ս���غϣ���������غ���û�н���ս������ǿ�ƽ���ս��
	int mMaxRoundCount;
};






#endif