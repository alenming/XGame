//
//  EditBoxEx.h
//  LucyOL
//
//  Created by Paddy on 14-5-20.
//
//

#ifndef __LucyOL__EditBoxEx__
#define __LucyOL__EditBoxEx__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosGUI.h"

class EditBoxEx: public cocos2d::extension::EditBox
{
public:
    static EditBoxEx*		create(cocos2d::ui::TextField * pTextField);

	
    
protected:
    void					initWithTextField(cocos2d::ui::TextField * pTextField);
};

#endif /* defined(__LucyOL__EditBoxEx__) */
