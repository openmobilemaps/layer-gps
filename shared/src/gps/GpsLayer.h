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

#include "GpsLayerInterface.h"
#include "GpsLayerCallbackInterface.h"
#include "SimpleTouchInterface.h"
#include "CoordinateSystemIdentifiers.h"
#include "MapCamera2dInterface.h"
#include "Textured2dLayerObject.h"
#include "Circle2dLayerObject.h"
#include "GpsStyleInfo.h"
#include "GpsMode.h"

class GpsLayer : public GpsLayerInterface,
                 public LayerInterface,
                 public SimpleTouchInterface,
                 public std::enable_shared_from_this<GpsLayer> {
public:
    GpsLayer(const GpsStyleInfo & styleInfo);

    // GpsLayerInterface

    virtual void setMode(GpsMode mode) override;

    virtual void updatePosition(const ::Coord &position, double horizontalAccuracyM) override;

    virtual void updateHeading(float angleHeading) override;

    virtual std::shared_ptr<::LayerInterface> asLayerInterface() override;

    // LayerInterface

    virtual void update() override;

    virtual std::vector<std::shared_ptr<::RenderPassInterface>> buildRenderPasses() override;

    virtual void onAdded(const std::shared_ptr<MapInterface> &mapInterface) override;

    virtual void onRemoved() override;

    virtual void pause() override;

    virtual void resume() override;

    virtual void hide() override;

    virtual void show() override;

    // SimpleTouchInterface

    virtual bool onClickConfirmed(const ::Vec2F &posScreen) override;

    virtual bool onDoubleClick(const ::Vec2F &posScreen) override;

    virtual bool onMove(const ::Vec2F &deltaScreen, bool confirmed, bool doubleClick) override;

    virtual bool onTwoFingerClick(const ::Vec2F &posScreen1, const ::Vec2F &posScreen2) override;

    virtual bool onTwoFingerMove(const std::vector<::Vec2F> &posScreenOld, const std::vector<::Vec2F> &posScreenNew) override;

private:
    virtual void resetMode();

    virtual void setupLayerObjects();

    virtual std::vector<float> computeModelMatrix(bool scaleInvariant, double objectScaling);

    std::shared_ptr<Textured2dLayerObject> centerObject;
    std::shared_ptr<Textured2dLayerObject> headingObject;
    std::shared_ptr<Circle2dLayerObject> accuracyObject;

    std::shared_ptr<MapInterface> mapInterface;
    std::shared_ptr<MapCamera2dInterface> camera;
    std::atomic<bool> isHidden = false;

    Coord position = Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), 0, 0, 0);
    double horizontalAccuracyM;
    float angleHeading;

    GpsMode mode = GpsMode::DISABLED;
    bool drawLocation = false;
    bool headingValid = false;
    bool followModeEnabled = false;
    bool rotationModeEnabled = false;

    GpsStyleInfo styleInfo;
};
