#ifndef AnimationLayer_h__
#define AnimationLayer_h__

#include "UI/ModalLayer.h"


class AnimationLayer : public ModalLayer
{

public:

	AnimationLayer();
	virtual ~AnimationLayer();

	static AnimationLayer* create(const string& aniName, const string& actionName);

public:

	virtual bool init(const string& aniName, const string& actionName);


};



#endif // AnimationLayer_h__