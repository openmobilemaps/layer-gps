// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from layer_gps.djinni

#pragma once

#include <functional>

enum class GpsMode : int {
    DISABLED = 0,
    STANDARD = 1,
    FOLLOW = 2,
    FOLLOW_AND_TURN = 3,
};

constexpr const char* toString(GpsMode e) noexcept {
    constexpr const char* names[] = {
        "disabled",
        "standard",
        "follow",
        "follow_and_turn",
    };
    return names[static_cast<int>(e)];
}

namespace std {

template <>
struct hash<::GpsMode> {
    size_t operator()(::GpsMode type) const {
        return std::hash<int>()(static_cast<int>(type));
    }
};

} // namespace std
