#include "MSAutoReleasePool.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#import <UIKit/UIKit.h>
#endif



MSAutoReleasePool::MSAutoReleasePool(){
	pool = [[NSAutoreleasePool alloc] init];

}

MSAutoReleasePool::~MSAutoReleasePool(){
//	[pool drain];
	[(NSAutoreleasePool*)pool release];
}
