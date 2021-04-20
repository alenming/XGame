#ifndef  _REYUNCONTROLLER_
#define  _REYUNCONTROLLER_

#include "cocos2d.h"
using namespace std;

enum PAY_TYPE
{
    ALIPAY,
    UNIONPAY,
    WEIXINPAY,
    YEEPAY,
    FREEPAY,
    APPSTORE
};
enum MONEY_TYPE_REYUN
{
    REYUN_MONEYTYPE_CNY,
    REYUN_MONEYTYPE_USD
};

class  ReyunController
{
public:
    ReyunController();
    virtual ~ReyunController();
    
    static void ReYunInit();
    
    static void ReYunRegister(std::string acountId);
    
    static void ReYunLogin(std::string acountId);
    
    static void ReYunPay(string transactionId,PAY_TYPE payTyep, MONEY_TYPE_REYUN moneyTyep, float moneyAmout);
    
    static void ReYunPaySuccessful(string transactionId,PAY_TYPE payTyep, MONEY_TYPE_REYUN moneyTyep, float moneyAmout);
};

#endif // _REYUNCONTROLLER_

