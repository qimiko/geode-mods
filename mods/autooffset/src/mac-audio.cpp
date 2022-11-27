#include "mac-audio.hpp"

#include <Geode/Geode.hpp>
#include <CoreAudio/CoreAudio.h>

#include <optional>

class MacAudioHelper {
private:
	AudioDeviceID deviceId_{0};

	class AudioLatencyCache {
	private:
		uint32_t totalLatency_{0};
		AudioDeviceID deviceId_{0};

	public:
		AudioLatencyCache(AudioDeviceID deviceId, uint32_t totalLatency) : deviceId_{deviceId}, totalLatency_{totalLatency} { }

		uint32_t getTotalLatency() const {
			return totalLatency_;
		}

		AudioDeviceID getDeviceId() const {
			return deviceId_;
		}
	};

	// there is exactly one case when this changes during runtime
	std::optional<AudioLatencyCache> cachedLatency_{std::nullopt};
	static constexpr bool USE_CACHED_LATENCY = false;

	uint32_t getAudioDeviceLatency() {
		auto latency = 0u;
		AudioObjectPropertyAddress pa = {
			.mSelector = kAudioDevicePropertyLatency,
			.mScope = kAudioObjectPropertyScopeOutput,
			.mElement = 0
		};
		uint32_t size = sizeof(latency);
		auto err = AudioObjectGetPropertyData(deviceId_, &pa, 0, nullptr, &size, &latency);
		if (err != 0) {
			geode::log::error("getAudioDeviceLatency err: {}", err);
		}

		geode::log::debug("audioLatency {}", latency);

		return latency;
	}

	uint32_t getSafetyOffset() {
		auto offset = 0u;
		AudioObjectPropertyAddress pa = {
			.mSelector = kAudioDevicePropertySafetyOffset,
			.mScope = kAudioObjectPropertyScopeOutput,
			.mElement = 0
		};
		uint32_t size = sizeof(offset);
		auto err = AudioObjectGetPropertyData(deviceId_, &pa, 0, nullptr, &size, &offset);
		if (err != 0) {
			geode::log::error("getSafetyOffset err: {}", err);
		}

		geode::log::debug("safetyOffset {}", offset);

		return offset;
	}

	uint32_t getAudioStreamLatency() {
		auto latency = 0u;
		AudioObjectPropertyAddress pa = {
			.mSelector = kAudioStreamPropertyLatency,
			.mScope = kAudioObjectPropertyScopeOutput,
			.mElement = 0
		};
		uint32_t size = sizeof(latency);
		auto err = AudioObjectGetPropertyData(deviceId_, &pa, 0, nullptr, &size, &latency);
		if (err != 0) {
			geode::log::error("getAudioStreamLatency err: {}", err);
		}

		geode::log::debug("audioStreamLatency {}", latency);

		return latency;
	}

	uint32_t getDeviceBufferFrameSize() {
		auto latency = 0u;
		AudioObjectPropertyAddress pa = {
			.mSelector = kAudioDevicePropertyBufferFrameSize,
			.mScope = kAudioObjectPropertyScopeOutput,
			.mElement = 0
		};
		uint32_t size = sizeof(latency);
		auto err = AudioObjectGetPropertyData(deviceId_, &pa, 0, nullptr, &size, &latency);
		if (err != 0) {
			geode::log::error("getDeviceBufferFrameSize err: {}", err);
		}

		geode::log::debug("deviceBufferFrameSize {}", latency);

		return latency;
	}

	// find a way to get the audio device GD is using?
	// (it doesn't change during runtime)
	// either that or make GD change audio device during runtime
	static AudioDeviceID getDefaultAudioDeviceId() {
		AudioDeviceID deviceId = 0;
		AudioObjectPropertyAddress pa = {
			.mSelector = kAudioHardwarePropertyDefaultOutputDevice,
			.mScope = kAudioObjectPropertyScopeGlobal,
			.mElement = 0
		};
		uint32_t size = sizeof(deviceId);
		auto err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &pa, 0, nullptr, &size, &deviceId);
		if (err != 0) {
			geode::log::error("getAudioDeviceId err: {}", err);
		}

		return deviceId;
	}

	MacAudioHelper() {
		deviceId_ = getDefaultAudioDeviceId();
	}

	uint32_t computeTotalLatency() {
		return (
			getAudioDeviceLatency()
			+ getSafetyOffset()
			// appears to be = audioDeviceLatency
			// unsure if bugged or not (seems more accurate though)
			+ getAudioStreamLatency()
			// seems to report a constant 5.12ms
			// that is weird. not using lol
			// + getDeviceBufferFrameSize(deviceId)
		);
	}

public:
	uint32_t getTotalLatency() {
		if (cachedLatency_ && USE_CACHED_LATENCY) {
			// use cached latency only if device hasn't changed
			// in the future it'd be nice if I could listen to this?
			if (cachedLatency_->getDeviceId() == getDefaultAudioDeviceId()) {
				return cachedLatency_->getTotalLatency();
			}
		}

		auto computedLatency = computeTotalLatency();
		cachedLatency_ = AudioLatencyCache(computedLatency, deviceId_);
		geode::log::debug("computing totalLatency: {}0 µs", computedLatency);

		return computedLatency;
	}

	void refreshAudioDeviceId() {
		deviceId_ = getDefaultAudioDeviceId();
	}

	static MacAudioHelper& getHelper()
	{
			static MacAudioHelper instance;
			return instance;
	}

	MacAudioHelper(MacAudioHelper const&) = delete;
	void operator=(MacAudioHelper const&) = delete;

	// not entirely sure what units the returned values are in?
	// the division makes it seem slightly correct, at least
	static constexpr uint32_t LATENCY_CONVERSION_FACTOR = 100;
};

int getOutputLatencyMs() {
	auto deviceLatency =
		MacAudioHelper::getHelper().getTotalLatency() / MacAudioHelper::LATENCY_CONVERSION_FACTOR;

	return deviceLatency;
}
