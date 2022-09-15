#include <Geode/Geode.hpp>

USE_GEODE_NAMESPACE();

GEODE_API bool geode_load(geode::Mod* m) {
	auto gdBase = geode::base::get();

#ifndef GEODE_IS_MACOS
	#warning This mod is only functional on the macOS version of Geometry Dash!
	return;
#endif

	m->patch(
		reinterpret_cast<void*>(gdBase + 0x18BFA),
		{ 0xEB }
	);
	m->patch(
		reinterpret_cast<void*>(gdBase + 0x18F25),
		{ 0xEB }
	);
	m->patch(
		reinterpret_cast<void*>(gdBase + 0x1B991),
		{ 0xEB }
	);

	// these two are in the same fn btw
	m->patch(
		reinterpret_cast<void*>(gdBase + 0x949CD),
		{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }
	);
	m->patch(
		reinterpret_cast<void*>(gdBase + 0x94B1D),
		{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }
	);

	return true;
}
