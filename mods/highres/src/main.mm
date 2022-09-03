#include <Geode/Geode.hpp>
#include <Cocoa/Cocoa.h>
#import <objc/runtime.h>

USE_GEODE_NAMESPACE();

@interface NeedGoodNamePlease : NSObject
@end

@implementation NeedGoodNamePlease

// i didn't stole this
void enableWantsBestResMode() {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-method-access"
	[[NSClassFromString(@"EAGLView") sharedEGLView] setWantsBestResolutionOpenGLSurface:YES];
#pragma clang diagnostic pop
}

static void(*s_originalInitWithFrame)(id, SEL, NSRect, NSOpenGLPixelFormat*);
- (id) initWithFrameHook:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format {
	s_originalInitWithFrame(self, @selector(initWithFrame:pixelFormat:), frameRect, format);
	enableWantsBestResMode();

// TODO: get scale factor automatically
// ([UIScreen scale] or something)
// auto scale_factor = 0.0f;

// TODO: fix scrollview being broken (probably glViewport) idk lol
// oh and also changing resolution but that's secondary

// cocos2d::CCEGLViewProtocol::setViewPortInPoints (0x29DA70) -> cocos2d::CCScrollLayerExt::preVisitWithClippingRect
// cocos2d::CCDirectory::setWinSize
// CCEGLViewProtocol -> getScaleX/scaleY
// CCEGLViewProtocol::setDesignResolutionSize(scaledResH, scaledResW, kResolutionFixedHeight);
// setContentScaleFactor
// [UIScreen scale] how do i get that though lol
// [self setFrameZoomFactor:2.0f];
// AppController::getDisplaySize -> mainScreen.frame
	return self;
}

+ (void)load {
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		Class class_ = NSClassFromString(@"EAGLView");

		// i stole this

		Method initWithFrameMethod = class_getInstanceMethod(class_, @selector(initWithFrame:pixelFormat:));
		Method swizzleInitWithFrameMethod = class_getInstanceMethod([self class], @selector(initWithFrameHook:pixelFormat:));
		s_originalInitWithFrame = (decltype(s_originalInitWithFrame))method_getImplementation(initWithFrameMethod);
		method_exchangeImplementations(initWithFrameMethod, swizzleInitWithFrameMethod);

//		GEODE_API_OBJC_SWIZZLE(initWithFrame:pixelFormat, initWithFrameHook:pixelFormat, s_originalInitWithFrame)
	});
}

@end

GEODE_API bool GEODE_CALL geode_load(Mod*) {
	@autoreleasepool {
		auto object = [[NeedGoodNamePlease alloc] init];
	}

	return true;
}