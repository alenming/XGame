//
//  U8SDK+Validate.h
//  U8SDK+Validate
//
//  Created by dayong on 15-1-21.
//  Copyright (c) 2015年 u8sdk. All rights reserved.
//

#import "../../SDK_IOS/Core/U8SDK/U8SDK.h"

// U8SDK的核心类
@interface U8SDK (Validate)

// 账号验证方法
-(void) AccountValidate:(NSDictionary*)params responseHandler:(void (^)(NSURLResponse* response, id data, NSError* connectionError)) handler;

// 充值验证
-(void) PayValidate:(NSDictionary*)params responseHandler:(void(^)(NSURLResponse* response, id data, NSError* connectionError)) handler;

@end
