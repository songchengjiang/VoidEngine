//
//  OpenGLAppDelegate.m
//  OpenGLESTest
//
//  Created by JasonCheng on 16/5/18.
//  Copyright © 2016年 JasonCheng. All rights reserved.
//

#import "OpenGLAppDelegate.h"
#import "VoidEngine.h"
#import "KernelCore/ios/GLView.h"
#import "KernelCore/ios/Viewer-ios.h"
#import "TestCreator.h"

@implementation OpenGLAppDelegate

@synthesize window=_window;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    veConfiguration::VE_DEVICE_PIXEL_RATIO = [[UIScreen mainScreen] scale];
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    
    auto viewer = static_cast<veViewerIOS *>(veApplication::instance()->createViewer(int(screenBounds.size.width), int(screenBounds.size.height), "VoidEngineTest"));
    
    veGLView *glView = [[veGLView alloc] initWithFrame:screenBounds withViewer: viewer];
    
    _viewController = [[ViewController alloc] initWithNibName:nil bundle:nil];
    _viewController.view = glView;
    
    [self.window setRootViewController:_viewController];
    [self.window makeKeyAndVisible];
    
    TestCreator::createTest();
    veApplication::instance()->run();
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
     If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
     */
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    /*
     Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
     */
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    /*
     Called when the application is about to terminate.
     Save data if appropriate.
     See also applicationDidEnterBackground:.
     */
    veApplication::instance()->stop();
}

@end

