
#ifndef MysteryShopScene_h_
#define MysteryShopScene_h_

#include "NewShopModule/ShopDef.h"
#include "MysteryShopLayer.h"

class MysteryShopScene:public Scene
{
public:
	~MysteryShopScene();
	 static Scene* createScene(SHOP_TYPE type);
protected:
	virtual bool init(SHOP_TYPE type) ;
	virtual void onEnter() override;
	virtual void onExit() override;

	MysteryShopScene();
private:

	MysteryShopLayer*  m_ShopLayer;
};
#endif //ShopScene_h_
