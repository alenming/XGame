#ifndef ShowFightPowerAction_h__
#define ShowFightPowerAction_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"

USING_NS_CC;
using namespace std;
using namespace ui;


class ShowFightPowerAction : public Node
{
public:
	ShowFightPowerAction();
	~ShowFightPowerAction();

	//ͨ��ս�����仯�仯��Ч
	//���÷�����create֮����ӵ����ֲ� ����: �仯ǰս�������仯��ս����
	static ShowFightPowerAction* create(int fightPower1, int fightPower2);

private:

	void initUI(int fightPower1, int fightPower2);
	void initActionBg(int dValue);
	void initNumUI(int fightPower1, int fightPower2);

	int getNumLength(int fightPower);
	void setNumber(int number, bool isGoalNumber);

	void updateNumber(float dt);

	void numberDoAction();
	void moveOut();

private:

	Node* m_actionBg;
	Node* m_dValueNode;
	Node* m_fightPowerNode;
	vector<Text*> m_vLabNum;
	vector<int> m_vCurNum;
	vector<int> m_vGoalNum;
	Text* m_txtDvalue;
	int m_length;
	float m_scrollTime;
};

#endif