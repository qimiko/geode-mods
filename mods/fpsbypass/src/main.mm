/*
 * this code was originally from the following url:
 * https://github.com/camila314/geode-mods/blob/main/fps-bypass/src/main.mm
 * ownership belongs to them
 * (copied to retargest latest geode)
 */

#include <Geode/Geode.hpp>
#include <Cocoa/Cocoa.h>
#include "bypass-callback.h"

USE_GEODE_NAMESPACE();

double GLOBAL_FPS = 60.0;
bool GLOBAL_ENABLED = false;

// Stub for CCDirectorCaller that has the member we need
struct CCDirectorCaller_ {
	char a[0x10];
	NSTimer* renderTimer;
};

// Hook -[CCDirectorCaller setAnimationInterval] to use our physics timer
void CCDirectorCaller_setAnimationInterval(CCDirectorCaller_* caller, void*, double) {
	[caller->renderTimer invalidate];

	caller->renderTimer = [NSTimer timerWithTimeInterval:1.0/GLOBAL_FPS
										target:[BypassCallback sharedCallback]
										selector:@selector(physicsFired:)
										userInfo:nil
										repeats:YES];

	auto runLoop = [NSRunLoop currentRunLoop];

	[runLoop addTimer:caller->renderTimer forMode:NSDefaultRunLoopMode];
	[runLoop addTimer:caller->renderTimer forMode:NSEventTrackingRunLoopMode];
}

double getDisplayHz() {
	// in the future this should be gained from the application
	// this is bad if you play GD on a second screen
	// (have to figure out how to get displayID)
	auto displayId = CGMainDisplayID();
	auto displayMode = CGDisplayCopyDisplayMode(displayId);

	return CGDisplayModeGetRefreshRate(displayMode);
}

GEODE_API bool GEODE_DLL geode_load(Mod* m) {

	// Apply manual hook
	m->addHook<&CCDirectorCaller_setAnimationInterval, geode::core::meta::DefaultConv>(
		"setInteral", (void*)(geode::base::get() + 0x1da750));

	// must run on main thread
	Loader::get()->queueInGDThread([m](){
		auto hz = getDisplayHz();
		GLOBAL_FPS = hz;

		// setup physics timer
		AppDelegate::get()->setAnimationInterval(GLOBAL_FPS);

		// We need a rendering timer, too, running at 60fps.
		// in the future, make this the display refresh rate
		NSTimer* renderTimer = [NSTimer timerWithTimeInterval:1.0/hz
													target:[(id)NSClassFromString(@"CCDirectorCaller") sharedDirectorCaller]
													selector:@selector(timerFired:)
													userInfo:nil
													repeats:YES];


		[[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSDefaultRunLoopMode];
		[[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSEventTrackingRunLoopMode];

		GLOBAL_ENABLED = true;
	});
	return true;
}
