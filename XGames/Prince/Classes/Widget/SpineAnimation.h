#ifndef SpineAnimation_h__
#define SpineAnimation_h__

#include "spine/spine-cocos2dx.h"

class SpineAnimation: public spine::SkeletonAnimation
{

public:

	SpineAnimation();
	SpineAnimation(const char* skeletonDataFile, spAtlas* atlas, float scale = 1.0f);
	SpineAnimation(const char* skeletonDataFile, const char* atlasFile, float scale = 1.0f);
	virtual ~SpineAnimation ();

	//加载json格式, 已废弃
	static SpineAnimation* createWithFile(const char* skeletonDataFile, spAtlas* atlas, float scale = 1.0f);
	static SpineAnimation* createWithFile(const char* skeletonDataFile, const char* atlasFile, float scale = 1.0f);

	//加载二进制格式
	static SkeletonAnimation* createWithBinaryFile(const std::string& skeletonBinaryFile, const std::string& atlasFile, float scale = 1.0f);
	static SpineAnimation* createFighterWithResId(int resId);
	static SpineAnimation* createSkillWithResId(int resId);

public:

	//停止动画播放
	void stopAnimation();

	//开始动画播放
	void resumeAnimation();

};

#endif // SpineAnimation_h__
