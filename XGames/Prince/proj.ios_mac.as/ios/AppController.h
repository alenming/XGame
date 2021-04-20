#import <UIKit/UIKit.h>

////// Mod Begin //////
extern UIView* videoView;
////// Mod End   //////

@class RootViewController;

@interface AppController : NSObject <UIApplicationDelegate> {
    UIWindow *window;
}

@property(nonatomic, readonly) RootViewController* viewController;

@end

