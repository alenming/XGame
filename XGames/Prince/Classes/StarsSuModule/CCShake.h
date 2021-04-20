#ifndef __SHAKE_H__  
#define __SHAKE_H__  

#include "cocos2d.h"  
USING_NS_CC;
using namespace std;


class CCShake : public ActionInterval 
{  
public:  
	CCShake() {};  
	virtual ~CCShake() {};

	// Create the action with a time and a strength (same in x and y)  
	static CCShake *create(float _duration, float strength );
	// Create the action with a time and strengths (different in x and y)  
	static CCShake *createWithStrength(float _duration, float strength_x, float strength_y );  
	bool initWithDuration(float _duration, float strength_x, float strength_y );  

	virtual CCShake* reverse() const override;
	virtual CCShake *clone() const override;
	virtual void startWithTarget(Node *target) override;
	virtual void update(float time);
	virtual void stop();  

private:

	// Initial position of the shaked node 
	float m_initial_x, m_initial_y;  
	// Strength of the action  
	float m_strength_x, m_strength_y; 
};  

#endif //__SHAKE_H__  