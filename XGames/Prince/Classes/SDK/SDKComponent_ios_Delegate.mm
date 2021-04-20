#import "SDKComponent_ios_Delegate.h"
#import "U8SDK+Validate.h"
#import "SDKComponent_ios.h"

using namespace std;

@interface SDKComponent_ios_Delegate ()

@end

@implementation SDKComponent_ios_Delegate

-(UIView *)GetView
{
    UIWindow* window = [[UIApplication sharedApplication] keyWindow];
    return window.rootViewController.view;
}

-(UIViewController *)GetViewController
{
    UIWindow* window = [[UIApplication sharedApplication] keyWindow];
    return window.rootViewController;
}

-(void) initDelegate
{
    [[U8SDK sharedInstance] setDelegate:self];
}

-(void) OnPlatformInit:(NSNotification*)notification
{
    [self SendCallback:"OnPlatformInit" withParams:notification.userInfo];
}

-(void) OnUserLogin:(NSNotification*)notification
{
    [[U8SDK sharedInstance] AccountValidate:notification.userInfo responseHandler:^(NSURLResponse *response, id data, NSError *err)
     {
         NSString* alertMsg = nil;
         
         if (err)
         {
             alertMsg = [NSString stringWithFormat:@"账号验证失败: %@", [err localizedDescription]];
         }
         else if (!data)
         {
             NSAssert([response isKindOfClass:[NSHTTPURLResponse class]], @"Invalid response type");
             
             NSHTTPURLResponse* httpResponse = (NSHTTPURLResponse*)response;
             alertMsg = [NSString stringWithFormat:@"账号验证失败: %@", [NSHTTPURLResponse localizedStringForStatusCode:httpResponse.statusCode]];
         }
         else
         {
             NSDictionary* json = (NSDictionary*)data;
             NSDictionary* jsonFieldData = [json valueForKey:@"data"];
             
             if ([[json valueForKey:@"state"] intValue] == 1)
             {
                 NSMutableDictionary* dict = [NSMutableDictionary dictionary];
                 
                 [dict setValuesForKeysWithDictionary:jsonFieldData];
                 
                 [self SendCallback:"OnU8LoginSuc" withParams:dict];
                 
                 // 结果回调;
                 // 需要把dict解析一下，转化为U8LoginResult类型的参数并传递;
                 U8LoginResult* result = U8LoginResult::create();
                 result->isSuc = true;
                 result->sdkUserId = string([[jsonFieldData valueForKey:@"sdkUserID"] UTF8String]);
                 result->sdkUsername = string([[jsonFieldData valueForKey:@"sdkUserName"] UTF8String]);
                 result->userId = string([[NSString stringWithFormat:@"%d", [[jsonFieldData valueForKey:@"userID"] intValue]] UTF8String]);
                 result->username = string([[jsonFieldData valueForKey:@"username"] UTF8String]);
                 result->token = string([[jsonFieldData valueForKey:@"token"] UTF8String]);
                 
                 // 转化参数;
                 SDKComponent_ios::getInstance()->OnLoginSuc(result);
             }
             else
             {
                 alertMsg =[json valueForKey:@"message"];
             }
         }
         
         if (alertMsg)
         {
             [self showAlertView:alertMsg];
         }
     }];
}

-(void) OnUserLogout:(NSNotification*)notification
{
    //self.lbl_username.text = @"未登录";
    //[self popToRootViewControllerAnimated:YES];
    SDKComponent_ios::getInstance()->OnLogout();
}

-(void) OnPayPaid:(NSNotification*)notification
{
    NSString* orderId = [notification.userInfo valueForKey:@"orderID"];
    std::string strOrderId = [orderId UTF8String];
    SDKComponent_ios::getInstance()->OnAppStorePaySuc(strOrderId);
 
    [[U8SDK sharedInstance] PayValidate:notification.userInfo responseHandler:^(NSURLResponse *response, id data, NSError *err)
     {
         NSString* alertMsg = nil;
         
         if (err)
         {
             alertMsg = [NSString stringWithFormat:@"订单验证失败: %@", [err localizedDescription]];
         }
         else if (!data)
         {
             NSAssert([response isKindOfClass:[NSHTTPURLResponse class]], @"Invalid response type");
             
             NSHTTPURLResponse* httpResponse = (NSHTTPURLResponse*)response;
             alertMsg = [NSString stringWithFormat:@"订单验证失败: empty response！%@", [NSHTTPURLResponse localizedStringForStatusCode:httpResponse.statusCode]];
         }
         else
         {
             NSDictionary* json = (NSDictionary*)data;
             NSDictionary* jsonFieldData = [json valueForKey:@"data"];
             
             if ([[json valueForKey:@"state"] intValue] == 1)
             {
                 NSMutableDictionary* dict = [NSMutableDictionary dictionary];
                 
                 [dict setValuesForKeysWithDictionary:jsonFieldData];
                 
                 [self SendCallback:"OnU8Pay" withParams:dict];
                 
                 NSString* transitionId = [notification.userInfo valueForKey:@"transactionIdentifier"];
                 std::string strTransitionId = [transitionId UTF8String];
                 
                 // [废弃]结果回调;
                 //SDKComponent_ios::getInstance()->OnPaySuc(strTransitionId);
                 
                 [[U8SDK sharedInstance].defaultPay finishTransactionId:transitionId];
                 
                 if ([[U8SDK sharedInstance].defaultPay respondsToSelector:@selector(closeIAP)])
                 {
                     [[U8SDK sharedInstance].defaultPay closeIAP];
                 }
                 
             }
             else
             {
                 alertMsg =[json valueForKey:@"message"];
             }
         }
         
         if (alertMsg)
         {
             [self showAlertView:alertMsg];
         }
     }];
}

-(void)OnEventCustom:(NSNotification *)notification
{
    NSString* name = [notification.userInfo valueForKey:@"name"];
    NSDictionary* params = [notification.userInfo valueForKey:@"params"];
    
    [self SendCallback:[name UTF8String] withParams:params];
}

-(void) SendCallback:(const char*)method withParams:(NSDictionary *)params
{
    NSString* jsStr = nil;
    
    if (params)
    {
        NSError* error;
        NSData* data = [NSJSONSerialization dataWithJSONObject:params options:kNilOptions error:&error];
        
        if (data)
        {
            jsStr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        }
    }
    
    [self SendCallback:method withMessage:jsStr];
}

-(void) SendCallback:(const char*)method withMessage:(NSString*)msg
{
    if (msg)
    {
        NSLog(@"Callback %s %@", method, msg);
    }
    else
    {
        NSLog(@"Callback %s", method);
    }
}

- (void)showAlertView:(NSString *)message{
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"" message:message delegate:self cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
    [alert show];
}

@end
