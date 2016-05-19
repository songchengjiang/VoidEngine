//
//  OpenGLAppDelegate.h
//  OpenGLESTest
//
//  Created by JasonCheng on 16/5/18.
//  Copyright © 2016年 JasonCheng. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ViewController.h"

@interface OpenGLAppDelegate : NSObject <UIApplicationDelegate> {
    ViewController *_viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end
