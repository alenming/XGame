

/* ����ѡ����Ʒ�����Ľ������ؼ� 

Ŀ¼��Widget/QuantitySlideBar.h
������QuantitySlideBar

***********�ӿ�˵��***********

������QuantitySlideBar::create();

1. ���û���ʱ�������ֵ���ص��ӿڣ�������Ĭ��Ϊ1
setMaxQuantity(int maxNum)

2. ������Ҫ�������������������仯���ı�ָ��͵��۵����Ͳ������ο�: �����������е��ܳ��ۼ۸������ı�	
initDivisorText(Text* pText, int unitData)

3. ������Ҫ�������һ�������ı���ߣ�����ʱ����һ�¸÷�������������Ĭ�ϲ����
isNeedStroke(bool isStroke, Color3B color, int sizeWidth);

*/

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosGUI.h"
#include "CocoStudio.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

class QuantitySlideBar : public Node
{
public:
	QuantitySlideBar();
	~QuantitySlideBar();

	CREATE_FUNC(QuantitySlideBar);
public:
	virtual bool init();
	
	//�ӿ�1. ��ȡ��ǰѡ�е���Ŀ
	int getCurrentQuantity();

	//�ӿ�2. ��������������ص���Ĭ����1
	void setMaxQuantity(int maxNum);

	//�ӿ�3. �����������������仯���ı�ָ��͵��۵����Ͳ���	
	//����1. �ı�ָ�룬 ����2. ���۵�
	void initDivisorText(Text* pText, int unitData);
	//���ı��Ƿ���Ҫ���
	void isNeedStroke(bool isStroke, Color3B color, int sizeWidth);

private:
	//ѡ���������
	void selectMaxValue(Ref* pSender, ui::Widget::TouchEventType type);

	//ѡ����С����
	void selectMinValue(Ref* pSender, ui::Widget::TouchEventType type);

	//�������
	void valueChanged(Ref* pSender, Control::EventType type);

	//�����������������仯����������
	void setDivisorText();

private:
	//����
	int m_iQuantity;	

	//���ֵ
	int m_iMaxQuantity;

	//�����ı�
	Text* m_numText;

	//������������ص��ⲿ�ı�ָ��
	Text* m_extenalText;
	int m_unitData;
	bool m_bIsNeedStroke;
	Color3B m_color;
	int m_iSizeWidth;
};