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

	//����json��ʽ, �ѷ���
	static SpineAnimation* createWithFile(const char* skeletonDataFile, spAtlas* atlas, float scale = 1.0f);
	static SpineAnimation* createWithFile(const char* skeletonDataFile, const char* atlasFile, float scale = 1.0f);

	//���ض����Ƹ�ʽ
	static SkeletonAnimation* createWithBinaryFile(const std::string& skeletonBinaryFile, const std::string& atlasFile, float scale = 1.0f);
	static SpineAnimation* createFighterWithResId(int resId);
	static SpineAnimation* createSkillWithResId(int resId);

public:

	//ֹͣ��������
	void stopAnimation();

	//��ʼ��������
	void resumeAnimation();

};

#endif // SpineAnimation_h__
