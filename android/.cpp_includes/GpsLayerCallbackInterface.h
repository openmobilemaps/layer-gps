// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from layer_gps.djinni

#pragma once

struct GpsStyleInfo;

class GpsLayerCallbackInterface {
public:
    virtual ~GpsLayerCallbackInterface() {}

    virtual GpsStyleInfo getStyleInfo() = 0;
};
