//
//  NewTtack.h
//  NewTtack
//
//  Created by yun on 16/1/11.
//  Copyright © 2016年 yun. All rights reserved.
//


#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface ReYunChannel : NSObject

// 开启数据统计
+ (void)initWithappKey:(NSString *)appKey withChannelId:(NSString *)channelId;

//注册成功后调用
+ (void)setRegisterWithAccountID:(NSString *)account;

//登陆成功后调用
+ (void)setLoginWithAccountID:(NSString *)account;

//开始付费时 调用（人民币单位是元）
+(void)setPaymentStart:(NSString *)transactionId paymentType:(NSString*)paymentType currentType:(NSString*)currencyType currencyAmount:(float)currencyAmount;

// 支付完成，付费分析,记录玩家充值的金额（人民币单位是元）
+(void)setPayment:(NSString *)transactionId paymentType:(NSString*)paymentType currentType:(NSString*)currencyType currencyAmount:(float)currencyAmount;

//自定义事件
+(void)setEvent:(NSString *)eventName;

//获取设备信息
+(NSString*)getDeviceId;


@end
