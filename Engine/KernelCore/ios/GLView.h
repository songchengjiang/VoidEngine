//
//  OpenGLView.h
//  OpenGLESTest
//
//  Created by JasonCheng on 16/5/18.
//  Copyright © 2016年 JasonCheng. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "Prerequisites.h"
#import "KernelCore/ios/Viewer-ios.h"
#import "KernelCore/Application-ios.h"

@interface veGLView : UIView {
    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    CADisplayLink* _displayLink;
}

@property(nonatomic,readwrite) veViewerIOS *viewer;
@property(nonatomic,readwrite) veApplicationIOS *app;

- (void)startRendering;
- (void)stopRendering;
- (BOOL)makeContextCurrent;
- (void)swapBuffers;

@end
