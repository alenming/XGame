#ifndef AniButton_h__
#define AniButton_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CCArmature.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;
using namespace cocostudio;

typedef std::function<void(Ref* sender, Widget::TouchEventType type)> AniButtonCallback;

class AniButton: public Button
{

public:
	/**
     * create a AniButton with 
     * @parent ���븸�ڵ㣨Ҳ��һ��button��
     * @callBack  ���ڵ㰴�µ���Ϣ��Ӧ�ص�
     * @fileName �����ļ���
     */
	static AniButton* create(Button *parent,string fileName,AniButtonCallback callBack = nullptr);

	void initWithButton(string fileName,AniButtonCallback callBack);
private:

	
	
};

#endif //
