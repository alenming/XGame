#ifndef LayerLoading_h__
#define LayerLoading_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

class LayerLoading: public cocos2d::Layer
{
public:
	LayerLoading();

	~LayerLoading();

	CREATE_FUNC(LayerLoading);

protected:
	virtual bool init();

	virtual void onEnter();

	virtual void onExit();

	void setBgImage();

	void setTips();

	void calculateBgImagePath();

	//执行"加载中"动作
	void doLoadingAction();

private:
	Layout* m_uiRoot;
	ImageView* m_imgLoading;
	Text*	m_txtTips;
	vector<string>  m_vBgPath;
};

#endif // LayerLoading_h__
