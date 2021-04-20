/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerAdvanced
 *  Description:	金币进阶封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			03/04/2016
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerAdvanced_h__
#define LayerAdvanced_h__

#include "SmithyDef.h"

class LayerAdvanced : public Layer
{
public:
	LayerAdvanced(void);
	~LayerAdvanced(void);

	static LayerAdvanced* create(Node* parent);

	virtual bool  init(Node* parent);

private:

	void  initUI();

private:

	UI_JJ_Gold  m_ui;

	Node* m_parent;
};

#endif // LayerAdvanced_h__
