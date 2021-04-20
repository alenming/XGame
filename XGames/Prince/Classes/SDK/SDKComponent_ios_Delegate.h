/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved.
 *
 *  FileName:		SDKComponent_ios_Delegate
 *  Description:	SDK_ios_delegate封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			1/19/2016
 *  Others: 
 *
 ******************************************************************/
#ifndef SDKComponent_ios_Delegate_h__
#define SDKComponent_ios_Delegate_h__

#include "../../SDK_IOS/Core/U8SDK/U8SDK.h"
#include "./U8SDKData_ios.h"
#import <UIKit/UIKit.h>

@interface SDKComponent_ios_Delegate : NSObject<U8SDKDelegate>

- (void) initDelegate;

- (UIView*) GetView;

@end


#endif // SDKComponent_ios_Delegate_h__