/*
 * Copyright (c) 2021 Ubique Innovation AG <https://www.ubique.ch>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  SPDX-License-Identifier: MPL-2.0
 */

#pragma once

#include "GpsStyleInfoInterface.h"
#include "TextureHolderInterface.h"
#include "Color.h"

class GpsStyleInfo : public GpsStyleInfoInterface {
public:
    GpsStyleInfo(const std::shared_ptr<::TextureHolderInterface> &pointTexture,
                 const std::shared_ptr<::TextureHolderInterface> &headingTexture,
                 const std::shared_ptr<::TextureHolderInterface> &courseTexture,
                 const Color &accuracyColor);

    virtual /*nullable*/ std::shared_ptr<::TextureHolderInterface> getPointTexture() override;

    virtual /*nullable*/ std::shared_ptr<::TextureHolderInterface> getHeadingTexture() override;

    virtual /*nullable*/ std::shared_ptr<::TextureHolderInterface> getCourseTexture() override;

    virtual ::Color getAccuracyColor() override;

private:
    std::shared_ptr<::TextureHolderInterface> pointTexture;
    std::shared_ptr<::TextureHolderInterface> headingTexture;
    std::shared_ptr<::TextureHolderInterface> courseTexture;
    Color accuracyColor;
};
