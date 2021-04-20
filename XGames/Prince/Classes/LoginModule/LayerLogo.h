#ifndef LayerLogo_h__
#define LayerLogo_h__

#include "cocos2d.h"


class LayerLogo: public cocos2d::Layer
{

public:

	LayerLogo();
	virtual ~LayerLogo();
	
	CREATE_FUNC(LayerLogo);

public:

	virtual bool init();

	virtual void onEnter();

private:

	void finish();

};

#endif // LayerLogo_h__
