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

#include "SimpleLayerInterface.h"
#include "GpsLayerInterface.h"
#include "GpsLayerCallbackInterface.h"
#include "SimpleTouchInterface.h"
#include "CoordinateSystemIdentifiers.h"
#include "MapCameraInterface.h"
#include "Textured2dLayerObject.h"
#include "Circle2dLayerObject.h"
#include "TextureHolderInterface.h"
#include "GpsStyleInfoInterface.h"
#include "GpsCourseInfo.h"
#include "GpsMode.h"
#include <mutex>

class GpsLayer : public GpsLayerInterface,
                 public SimpleLayerInterface,
                 public SimpleTouchInterface,
                 public MapCameraListenerInterface,
                 public std::enable_shared_from_this<GpsLayer> {
public:
    GpsLayer(const /*not-null*/ std::shared_ptr<GpsStyleInfoInterface> & styleInfo);

    // GpsLayerInterface

    virtual void setMode(GpsMode mode) override;

    virtual void setModeWithRotationReset(GpsMode mode, bool resetRotation) override;

    virtual void setResetRotationOnMapInteraction(bool resetRotation) override;

    virtual GpsMode getMode() override;

    virtual void enableHeading(bool enable) override;

    virtual void updatePosition(const ::Coord &position, double horizontalAccuracyM) override;

    virtual void updateHeading(float angleHeading) override;

    virtual std::shared_ptr<::LayerInterface> asLayerInterface() override;

    virtual void setDrawPoint(bool enable) override;

    virtual void setDrawHeading(bool enable) override;

    virtual void setCallbackHandler(const std::shared_ptr<GpsLayerCallbackInterface> & handler) override;

    virtual void setFollowInitializeZoom(std::optional<float> zoom) override;

    virtual void updateStyle(const /*not-null*/ std::shared_ptr<GpsStyleInfoInterface> & styleInfo) override;

    virtual void enablePointRotationInvariant(bool enable) override;

    virtual void enableCourse(bool enable) override;

    virtual void updateCourse(const GpsCourseInfo & courseInfo) override;

    virtual void setRenderPassIndex(int32_t index) override;

    // LayerInterface

    virtual void setMaskingObject(const std::shared_ptr<::MaskingObjectInterface> & maskingObject) override;

    virtual void update() override;

    virtual std::vector<std::shared_ptr<::RenderPassInterface>> buildRenderPasses() override;

    virtual void onAdded(const std::shared_ptr<MapInterface> &mapInterface, int32_t layerIndex) override;

    virtual void onRemoved() override;

    virtual void pause() override;

    virtual void resume() override;

    virtual void hide() override;

    virtual void show() override;

    // SimpleTouchInterface

    virtual bool onClickConfirmed(const ::Vec2F &posScreen) override;

    bool onMoveComplete() override;

    bool onTwoFingerMove(const std::vector<::Vec2F> &posScreenOld, const std::vector<::Vec2F> &posScreenNew) override;

    bool onTwoFingerMoveComplete() override;

    void clearTouch() override;

    // MapCamera2dListenerInterface

    void onVisibleBoundsChanged(const RectCoord &visibleBounds, double zoom) override {};

    void onRotationChanged(float angle) override {};

    void onMapInteraction() override;

    void onCameraChange(const std::vector<float> & viewMatrix, const std::vector<float> & projectionMatrix, const ::Vec3D & origin, float verticalFov, float horizontalFov, float width, float height, float focusPointAltitude, const ::Coord & focusPointPosition, float zoom) override {};

private:
    virtual void updatePosition(const Coord &position, double horizontalAccuracyM, bool isInitialFollow);

    virtual void resetMode();
    virtual void resetParameters();

    virtual void setupLayerObjects();

    virtual std::vector<float> computeModelMatrix(bool scaleInvariant, double objectScaling, bool rotationInvariant, bool useCourseAngle);

    virtual void resetAccInteraction();

    virtual QuadCoord getQuadCoord(std::shared_ptr<TextureHolderInterface> texture);

    std::atomic<bool> isHidden = false;

    std::optional<Coord> position = std::nullopt;
    double horizontalAccuracyMapUnits = 0;
    std::atomic_flag accuracyChanged = ATOMIC_FLAG_INIT;
    float angleHeading = 0;

    GpsMode mode = GpsMode::DISABLED;
    bool drawLocation = false;
    bool positionValid = false;
    bool headingEnabled = true;
    bool headingValid = false;

    bool followModeEnabled = false;
    bool rotationModeEnabled = false;

    bool drawCenterObjectEnabled = true;
    bool drawHeadingObjectEnabled = true;

    bool pointRotationInvariantEnabled = false;

    bool courseValid = false;
    bool courseEnabled = false;
    float angleCourse = 0;
    float courseScaling = 0;

    std::shared_ptr<GpsStyleInfoInterface> styleInfo;
    int64_t pointHeight = 0;
    int64_t pointWidth = 0;

    std::recursive_mutex animationMutex;
    std::shared_ptr<AnimationInterface> headingAnimation;
    std::shared_ptr<AnimationInterface> angleCourseAnimation;
    std::shared_ptr<AnimationInterface> courseScalingAnimation;

    std::shared_ptr<GpsLayerCallbackInterface> callbackHandler;

    std::shared_ptr<::MaskingObjectInterface> mask = nullptr;

    std::recursive_mutex interactionMutex;
    std::optional<Coord> lastCenter = std::nullopt;
    std::optional<double> lastRotation = std::nullopt;
    Vec2D accInteractionMove = Vec2D(0.0, 0.0);
    bool isPinchMove = false;
    double accRotation = 0.0;

    int renderPassIndex = 999;

    const static int32_t SUBDIVISION_FACTOR_3D_DEFAULT = 2;

    bool resetRotationOnInteraction;

    struct OutstandingPositionUpdate {
        Coord position;
        double horizontalAccuracyM;
        bool isInitialFollow;
    };

    std::optional<OutstandingPositionUpdate> outstandingUpdate;
protected:
    std::shared_ptr<MapInterface> mapInterface;

    std::shared_ptr<Textured2dLayerObject> centerObject;
    std::shared_ptr<Textured2dLayerObject> headingObject;
    std::shared_ptr<Textured2dLayerObject> courseObject;
    std::shared_ptr<Circle2dLayerObject> accuracyObject;

    std::optional<float> followInitializeZoom;
};
