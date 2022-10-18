/*
 * this code was originally from the following url:
 * https://github.com/camila314/geode-mods/blob/main/fps-bypass/src/BypassCallback.h
 * ownership belongs to them
 * (copied to retargest latest geode)
 */

#include <Geode/Geode.hpp>
#include <Geode/Modify.hpp>
#include <Cocoa/Cocoa.h>

extern double GLOBAL_FPS;
extern bool GLOBAL_ENABLED;

@interface BypassCallback : NSObject
    + (BypassCallback*)sharedCallback;
    - (void)renderFired: (id)ok;
    - (void)physicsFired:(id)ok;
@end
