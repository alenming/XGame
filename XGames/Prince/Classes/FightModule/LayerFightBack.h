#ifndef LayerFightBack_h__
#define LayerFightBack_h__

#include "cocos2d.h"
#include <functional>

USING_NS_CC;
using namespace std;

//蒙板初始缩放
const float MASK_START_SCALE = 2.2f;

//特效缓动系数
const float EFFECT_EASE_COE = 2.0f;
//特效持续时间
const float EFFECT_DURATION = 0.3f;

//远景图缩放、旋转参数
const float BACK_IMG_START_SCALE = 1.1f;
const float BACK_IMG_END_SCALE = 1.0f;
const float BACK_IMG_ROTATE_X = 15.0f;

//地板前景图缩放、旋转参数
const float GROUND_FRONT_IMG_ROTATE_X = -10.0f;

//地板背景图缩放、旋转参数
const float GROUND_BACK_IMG_END_SCALE = 0.8f;
const float GROUND_BACK_IMG_ROTATE_X = -10.0f;

//地板旋转参数
const float GROUND_IMG_ROTATE_X = -30.0f;

//切换场景渐变时间
const float CHANGE_FADE_DURATION = 0.5f;

//图片名称
const string BG_NAME_B2 = "b2.png";
const string BG_NAME_B1 = "b1.png";
const string BG_NAME_F2 = "f2.png";
const string BG_NAME_F1 = "f1.png";
const string BG_NAME_F3 = "f3.png";

typedef function<void()> EffectStartCallBack;
typedef function<void()> EffectEndCallBack;
typedef function<void()> ChangeCallBack;

class LayerFightBack : public Layer
{

public:

	LayerFightBack();
	virtual ~LayerFightBack();

	//创建战斗背景层
	static LayerFightBack* create(int fightBgId, Node* frontParent = nullptr);

public:

	virtual bool init(int fightBgId, Node* frontParent);

	virtual void onEnter();

	virtual void onExit();

	//播放3d透视特效
	void start3dEffect(const EffectStartCallBack& effectStartCallBack = nullptr);

	//回播3d透视特效
	void return3dEffect(const EffectEndCallBack& effectEndCallBack = nullptr);

	//场景切换动画
	void changeWithFade(int fightBgId, const ChangeCallBack& callBack = nullptr);

	//场景切换接口
	void change(int fightBgId);

private:

	//战斗背景资源id
	int mFightBgId;

	//背景图
	Sprite* mBackImg;

	//地板图1
	Sprite* mGroundImg;

	//地板后景图1
	Sprite* mGroundBackImg;

	//地板后景图2
	Sprite* mGroundFrontImg;

	//前景图2
	Sprite* mGroundFrontImg2;

	//蒙板
	Sprite* mMask;

	//前景层父结点
	Node* mFrontParent;

};



#endif //LayerFightBack_h__