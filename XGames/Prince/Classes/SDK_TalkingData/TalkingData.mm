//
//  TalkingData.m
//  Prince
//
//  Created by zcjoy on 16/4/2.
//
//

#include "TalkingData.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "TalkingDataGA.h"
TDGAAccount* m_TDGAaccount = nullptr;
#endif

TalkingData::TalkingData(void)
{
}

TalkingData::~TalkingData(void)
{
    
}

void TalkingData::loginTalkingData(string userID, string SeverId,string accountName)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    NSString* nsAccount = [NSString stringWithCString:userID.c_str() encoding:NSUTF8StringEncoding];
    m_TDGAaccount = [TDGAAccount setAccount:nsAccount];
    if (m_TDGAaccount == nullptr)
        return;
    
    NSString* nsServerId = [NSString stringWithCString:SeverId.c_str() encoding:NSUTF8StringEncoding];
    [m_TDGAaccount setGameServer:nsServerId];
    NSString* nsAccountName = [NSString stringWithCString:accountName.c_str() encoding:NSUTF8StringEncoding];
    [m_TDGAaccount setAccountName:nsAccountName];
#endif
}

void TalkingData::setLevel(int level)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (m_TDGAaccount == nullptr)
        return;
    [m_TDGAaccount setLevel:level];
#endif
}

void TalkingData::onReward(double virtualCurrencyAmount, string reason)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (m_TDGAaccount == nullptr)
        return;
    
    NSString* nsReason = [NSString stringWithCString:reason.c_str() encoding:NSUTF8StringEncoding];
    [TDGAVirtualCurrency onReward:virtualCurrencyAmount reason:nsReason];
#endif
}

void TalkingData::onPurchase(string item, int itemNumber,double priceInVirtualCurrency)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (m_TDGAaccount == nullptr)
        return;
    
    NSString* nsItem = [NSString stringWithCString:item.c_str() encoding:NSUTF8StringEncoding];
    [TDGAItem onPurchase:nsItem itemNumber:itemNumber priceInVirtualCurrency:priceInVirtualCurrency];
#endif
}

void TalkingData::onUse(string item, int itemNumber)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (m_TDGAaccount == nullptr)
        return;
    
    NSString* nsItem = [NSString stringWithCString:item.c_str() encoding:NSUTF8StringEncoding];
    [TDGAItem onUse:nsItem itemNumber:itemNumber];
#endif
}

void TalkingData::onBegin(string missionId)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (m_TDGAaccount == nullptr)
        return;
    
    NSString* nsMissionId = [NSString stringWithCString:missionId.c_str() encoding:NSUTF8StringEncoding];
    [TDGAMission onBegin:nsMissionId];
#endif
}

void TalkingData::onCompleted(string missionId)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (m_TDGAaccount == nullptr)
        return;
    
    NSString* nsMissionId = [NSString stringWithCString:missionId.c_str() encoding:NSUTF8StringEncoding];
    [TDGAMission onCompleted:nsMissionId];
#endif
}

void TalkingData::onFailed(string missionId, string cause)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (m_TDGAaccount == nullptr)
        return;
    
    NSString* nsMissionId = [NSString stringWithCString:missionId.c_str() encoding:NSUTF8StringEncoding];
    NSString* nsCause = [NSString stringWithCString:cause.c_str() encoding:NSUTF8StringEncoding];
    [TDGAMission onFailed:nsMissionId failedCause:nsCause];
#endif 
}

void TalkingData::onEvent(string eventId, EventParamMap map)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    NSString* nsEventId = [NSString stringWithCString:eventId.c_str() encoding:NSUTF8StringEncoding];
    NSDictionary* dic = [[NSMutableDictionary alloc] init];
    
    int nMax = 10; // TalkingData IOS一次事件最多支持10个参数(20160403)
    int nCount = 0;
    for (auto iter = map.begin(); iter != map.end() && nCount < nMax; ++iter, ++nCount)
    {
        NSString* nsKey = [NSString stringWithCString:iter->first.c_str() encoding:NSUTF8StringEncoding];
        NSString* nsValue = [NSString stringWithCString:iter->second.c_str() encoding:NSUTF8StringEncoding];
        [dic setObject:nsValue forKey:nsKey];
    }
    [TalkingDataGA onEvent:nsEventId eventData:dic];
    [dic release];
#endif 
}
