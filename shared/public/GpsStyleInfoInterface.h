// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from layer_gps.djinni

#pragma once

#include "Color.h"
#include "TextureHolderInterface.h"
#include <memory>
#include <optional>

class GpsStyleInfoInterface {
public:
    virtual ~GpsStyleInfoInterface() = default;

    static /*not-null*/ std::shared_ptr<GpsStyleInfoInterface> create(const /*nullable*/ std::shared_ptr<::TextureHolderInterface> & pointTexture, const /*nullable*/ std::shared_ptr<::TextureHolderInterface> & headingTexture, const /*nullable*/ std::shared_ptr<::TextureHolderInterface> & courseTexture, const ::Color & accuracyColor);

    virtual /*not-null*/ std::shared_ptr<::TextureHolderInterface> getPointTexture() = 0;

    virtual /*nullable*/ std::shared_ptr<::TextureHolderInterface> getHeadingTexture() = 0;

    virtual /*nullable*/ std::shared_ptr<::TextureHolderInterface> getCourseTexture() = 0;

    virtual ::Color getAccuracyColor() = 0;
};