// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from layer_gps.djinni

#pragma once

#include "Coord.h"
#include "LayerInterface.h"
#include <memory>

class GpsLayerCallbackInterface;
enum class GpsMode;
struct GpsStyleInfo;

class GpsLayerInterface {
public:
    virtual ~GpsLayerInterface() {}

    static std::shared_ptr<GpsLayerInterface> create(const GpsStyleInfo & styleInfo);

    virtual void setCallbackHandler(const std::shared_ptr<GpsLayerCallbackInterface> & handler) = 0;

    virtual void setMode(GpsMode mode) = 0;

    virtual GpsMode getMode() = 0;

    virtual void enableHeading(bool enable) = 0;

    virtual void updatePosition(const ::Coord & position, double horizontalAccuracyM) = 0;

    virtual void updateHeading(float angleHeading) = 0;

    virtual void setDrawPoint(bool enable) = 0;

    virtual void setDrawHeading(bool enable) = 0;

    virtual std::shared_ptr<::LayerInterface> asLayerInterface() = 0;
};
