#ifndef HeroRelationController_h__
#define HeroRelationController_h__

#include "Communication/ICommListener.h"

class HeroRelationController : public ICommListener
{
public:
	HeroRelationController();
	virtual ~HeroRelationController();

public:

	virtual void onRecvMessage(int nCmdID, const char* szData);

private:

};

#endif