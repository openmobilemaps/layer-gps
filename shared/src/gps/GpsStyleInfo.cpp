/*
 * Copyright (c) 2021 Ubique Innovation AG <https://www.ubique.ch>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  SPDX-License-Identifier: MPL-2.0
 */

#include "GpsStyleInfo.h"

GpsStyleInfo::GpsStyleInfo(const std::shared_ptr<::TextureHolderInterface> &pointTexture,
                           const std::shared_ptr<::TextureHolderInterface> &headingTexture,
                           const std::shared_ptr<::TextureHolderInterface> &courseTexture, const Color &accuracyColor)
        : pointTexture(pointTexture),
          headingTexture(headingTexture),
          courseTexture(courseTexture),
          accuracyColor(accuracyColor) {}

std::shared_ptr<::TextureHolderInterface> GpsStyleInfo::getPointTexture() {
    return pointTexture;
}

std::shared_ptr<::TextureHolderInterface> GpsStyleInfo::getHeadingTexture() {
    return headingTexture;
}

std::shared_ptr<::TextureHolderInterface> GpsStyleInfo::getCourseTexture() {
    return courseTexture;
}

::Color GpsStyleInfo::getAccuracyColor() {
    return accuracyColor;
}
