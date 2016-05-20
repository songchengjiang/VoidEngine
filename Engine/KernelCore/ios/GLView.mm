//
//  OpenGLView.m
//  OpenGLESTest
//
//  Created by JasonCheng on 16/5/18.
//  Copyright © 2016年 JasonCheng. All rights reserved.
//

#import "GLView.h"

@implementation veGLView

@synthesize application;

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (void)setupLayer {
    _eaglLayer = (CAEAGLLayer*) self.layer;
    _eaglLayer.opaque = YES;
}

- (void)setupContext {
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES3;
    _context = [[EAGLContext alloc] initWithAPI:api];
    if (!_context) {
        NSLog(@"Failed to initialize OpenGLES 3.0 context");
        exit(1);
    }
    
    if (![EAGLContext setCurrentContext:_context]) {
        NSLog(@"Failed to set current OpenGL context");
        exit(1);
    }
}

- (void)setupFrameBuffer {
    GLuint depthRenderBuffer, colorRenderBuffer, renderFrameBuffer;
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, self.frame.size.width, self.frame.size.height);
    
    glGenRenderbuffers(1, &colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
    
    glGenFramebuffers(1, &renderFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, renderFrameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

}

- (void)rendering:(CADisplayLink*)displayLink {
    //[self makeContextCurrent];
    //glClearColor(0, 104.0/255.0, 55.0/255.0, 1.0);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glEnable(GL_DEPTH_TEST);
    self.application->simulationFrame(_displayLink.duration);
    //[self swapBuffers];
}

- (void)startRendering {
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(rendering:)];
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)stopRendering {
    [_displayLink invalidate];
    _displayLink = nil;
}

- (BOOL)makeContextCurrent {
    if (!_context){
        [self setupContext];
        [self setupFrameBuffer];
    }
    //glBindFramebuffer(GL_FRAMEBUFFER, _renderFrameBuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    return YES;
}

- (void)swapBuffers {
    [_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGRect frame = [self frame];
    CGFloat width = frame.size.width - frame.origin.x;
    CGFloat height = frame.size.height - frame.origin.y;
    
    int i = 0;
    for (UITouch *touch in touches) {
        CGFloat x = [touch locationInView: [touch view]].x * self.contentScaleFactor / width;
        CGFloat y = 1.0 - [touch locationInView: [touch view]].y * self.contentScaleFactor / height;
        x = (x - 0.5) * 2.0;
        y = (y - 0.5) * 2.0;
        self.application->onTouchBegan(i, x, y);
        ++i;
    }
    self.application->onPushCurrentEvent();
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGRect frame = [self frame];
    CGFloat width = frame.size.width - frame.origin.x;
    CGFloat height = frame.size.height - frame.origin.y;
    
    int i = 0;
    for (UITouch *touch in touches) {
        CGFloat x = [touch locationInView: [touch view]].x * self.contentScaleFactor / width;
        CGFloat y = 1.0 - [touch locationInView: [touch view]].y * self.contentScaleFactor / height;
        x = (x - 0.5) * 2.0;
        y = (y - 0.5) * 2.0;
//#if defined(__IPHONE_9_0) && (__IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_9_0)
//        // running on iOS 9.0 or higher version
//        if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 9.0f) {
//            fs[i] = touch.force;
//            ms[i] = touch.maximumPossibleForce;
//        }
//#endif
        self.application->onTouchMove(i, x, y);
        ++i;
    }
    self.application->onPushCurrentEvent();
    
    //auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    //glview->handleTouchesMove(i, (intptr_t*)ids, xs, ys, fs, ms);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGRect frame = [self frame];
    CGFloat width = frame.size.width - frame.origin.x;
    CGFloat height = frame.size.height - frame.origin.y;
    
    int i = 0;
    for (UITouch *touch in touches) {
        CGFloat x = [touch locationInView: [touch view]].x * self.contentScaleFactor / width;
        CGFloat y = 1.0 - [touch locationInView: [touch view]].y * self.contentScaleFactor / height;
        x = (x - 0.5) * 2.0;
        y = (y - 0.5) * 2.0;
        self.application->onTouchEnd(i, x, y);
        ++i;
    }
    self.application->onPushCurrentEvent();
    
    //auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    //glview->handleTouchesEnd(i, (intptr_t*)ids, xs, ys);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
//    UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {0};
//    float xs[IOS_MAX_TOUCHES_COUNT] = {0.0f};
//    float ys[IOS_MAX_TOUCHES_COUNT] = {0.0f};
//    
//    int i = 0;
//    for (UITouch *touch in touches) {
//        ids[i] = touch;
//        xs[i] = [touch locationInView: [touch view]].x * self.contentScaleFactor;;
//        ys[i] = [touch locationInView: [touch view]].y * self.contentScaleFactor;;
//        ++i;
//    }
    
    //auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    //glview->handleTouchesCancel(i, (intptr_t*)ids, xs, ys);
}



- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    [self setupLayer];
    _context = nil;
    return self;
}

@end
