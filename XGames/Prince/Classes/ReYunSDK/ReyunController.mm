#include "ReyunController.h"
#include "ReYunChannel.h"
#import <AdSupport/AdSupport.h>

ReyunController::ReyunController()
{
    
}

ReyunController::~ReyunController()
{
    
}

void ReyunController::ReYunInit()
{
    NSDictionary *reyunconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"REYUN"];
    if (reyunconfig == nil)
    {
        return;
    }
    NSMutableDictionary* reyunparam = reyunconfig ? [NSMutableDictionary dictionaryWithDictionary:reyunconfig] : [NSMutableDictionary dictionary];
    
    NSString* reyunstate = [reyunparam objectForKey:@"state"];
    
    if (reyunstate != nil)
    {
        int state = [reyunstate intValue];
        if (state == 1)
        {
            NSString* reyunappkey = [reyunparam objectForKey:@"appkey"];
            NSString* reyundefault = [reyunparam objectForKey:@"default"];
            [ReYunChannel initWithappKey:reyunappkey withChannelId:reyundefault];
        }
    }

}

void ReyunController::ReYunRegister(std::string acountId)
{
    NSDictionary *reyunconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"REYUN"];
    if (reyunconfig == nil)
    {
        return;
    }
    NSMutableDictionary* reyunparam = reyunconfig ? [NSMutableDictionary dictionaryWithDictionary:reyunconfig] : [NSMutableDictionary dictionary];
    NSString* reyunstate = [reyunparam objectForKey:@"state"];
    
    if (reyunstate != nil)
    {
        int state = [reyunstate intValue];
        if (state == 1)
        {
            NSString* account = [NSString stringWithCString:acountId.c_str() encoding:[NSString defaultCStringEncoding]];
            [ReYunChannel setRegisterWithAccountID:account];
        }
    }
}

void ReyunController::ReYunLogin(std::string acountId)
{
    NSDictionary *reyunconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"REYUN"];
    if (reyunconfig == nil)
    {
        return;
    }
    NSMutableDictionary* reyunparam = reyunconfig ? [NSMutableDictionary dictionaryWithDictionary:reyunconfig] : [NSMutableDictionary dictionary];
    NSString* reyunstate = [reyunparam objectForKey:@"state"];
    
    if (reyunstate != nil)
    {
        int state = [reyunstate intValue];
        if (state == 1)
        {	
			NSString* account = [NSString stringWithCString:acountId.c_str() encoding:[NSString defaultCStringEncoding]];
            [ReYunChannel setLoginWithAccountID:account];
        }
    }

}

void ReyunController::ReYunPay(string transactionId,PAY_TYPE payTyep, MONEY_TYPE_REYUN moneyTyep, float moneyAmout)
{
    NSDictionary *reyunconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"REYUN"];
    if (reyunconfig == nil)
    {
        return;
    }
    NSMutableDictionary* reyunparam = reyunconfig ? [NSMutableDictionary dictionaryWithDictionary:reyunconfig] : [NSMutableDictionary dictionary];
    NSString* reyunstate = [reyunparam objectForKey:@"state"];
    
    if (reyunstate != nil)
    {
        if (!transactionId.empty())
        {
            //NSString* nsTransactionId = (NSString*)transactionId;
            
            NSString* str = [NSString stringWithCString:transactionId.c_str() encoding:[NSString defaultCStringEncoding]];
            
            NSString* nsPayType = @"";
            if (payTyep == ALIPAY)
            {
                nsPayType = @"alipay";
            }else if(payTyep == UNIONPAY)
            {
                nsPayType = @"unionpay";
            }else if(payTyep == WEIXINPAY)
            {
                nsPayType = @"weixinpay";
            }else if(payTyep == YEEPAY)
            {
                nsPayType = @"yeepay";
            }else if(payTyep == FREEPAY)
            {
                nsPayType = @"FREE";
            }else if(payTyep == APPSTORE)
            {
                nsPayType = @"appstore";
            }
            
            NSString* nsMoneyType = @"";
            if (moneyTyep == REYUN_MONEYTYPE_CNY)
            {
                nsMoneyType = @"CNY";
            }else if(moneyTyep == REYUN_MONEYTYPE_USD)
            {
                nsMoneyType = @"USD";
            }
            
            [ReYunChannel setPaymentStart:str paymentType:nsPayType currentType:nsMoneyType currencyAmount:moneyAmout];
        }
    }

}

void ReyunController::ReYunPaySuccessful(string transactionId,PAY_TYPE payTyep, MONEY_TYPE_REYUN moneyTyep, float moneyAmout)
{
    NSDictionary *reyunconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"REYUN"];
    if (reyunconfig == nil)
    {
        return;
    }
    NSMutableDictionary* reyunparam = reyunconfig ? [NSMutableDictionary dictionaryWithDictionary:reyunconfig] : [NSMutableDictionary dictionary];
    NSString* reyunstate = [reyunparam objectForKey:@"state"];
    
    if (reyunstate != nil)
    {
        if (!transactionId.empty())
        {
            if (!transactionId.empty())
            {
                NSString* str = [NSString stringWithCString:transactionId.c_str() encoding:[NSString defaultCStringEncoding]];
                NSString* nsPayType = @"";
                if (payTyep == ALIPAY)
                {
                    nsPayType = @"alipay";
                }else if(payTyep == UNIONPAY)
                {
                    nsPayType = @"unionpay";
                }else if(payTyep == WEIXINPAY)
                {
                    nsPayType = @"weixinpay";
                }else if(payTyep == YEEPAY)
                {
                    nsPayType = @"yeepay";
                }else if(payTyep == FREEPAY)
                {
                    nsPayType = @"FREE";
                }else if(payTyep == APPSTORE)
                {
                    nsPayType = @"appstore";
                }
                
                NSString* nsMoneyType = @"";
                if (moneyTyep == REYUN_MONEYTYPE_CNY)
                {
                    nsMoneyType = @"CNY";
                }else if(moneyTyep == REYUN_MONEYTYPE_USD)
                {
                    nsMoneyType = @"USD";
                }
                
                [ReYunChannel setPayment:str paymentType:nsPayType currentType:nsMoneyType currencyAmount:moneyAmout];
            }
        }
    }

}