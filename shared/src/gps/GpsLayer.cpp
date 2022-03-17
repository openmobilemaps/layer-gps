/*
 * Copyright (c) 2021 Ubique Innovation AG <https://www.ubique.ch>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  SPDX-License-Identifier: MPL-2.0
 */

#include <map>
#include "GpsLayer.h"
#include "MapInterface.h"
#include "MapConfig.h"
#include "LambdaTask.h"
#include "Matrix.h"
#include "RenderPass.h"
#include "RenderObject.h"
#include "CoordAnimation.h"
#include "DoubleAnimation.h"

#define DEFAULT_ANIM_LENGTH 100

GpsLayer::GpsLayer(const GpsStyleInfo &styleInfo) : styleInfo(styleInfo) {
}

void GpsLayer::setMode(GpsMode mode) {
    resetParameters();
    this->mode = mode;

    switch (mode) {
        case GpsMode::DISABLED: {
            drawLocation = false;
            followModeEnabled = false;
            rotationModeEnabled = false;
            break;
        }
        case GpsMode::STANDARD: {
            drawLocation = true;
            followModeEnabled = false;
            rotationModeEnabled = false;
            break;
        }
        case GpsMode::FOLLOW: {
            drawLocation = true;
            followModeEnabled = true;
            rotationModeEnabled = false;
            if (positionValid) {
                updatePosition(position, horizontalAccuracyM, true);
            }
            break;
        }
        case GpsMode::FOLLOW_AND_TURN: {
            drawLocation = true;
            followModeEnabled = true;
            rotationModeEnabled = true;
            if (positionValid) {
                updatePosition(position, horizontalAccuracyM, true);
                updateHeading(angleHeading);
            }
            break;
        }
    }

    if (mapInterface) mapInterface->invalidate();

    if (callbackHandler) callbackHandler->modeDidChange(this->mode);
}

GpsMode GpsLayer::getMode() {
    return this->mode;
}

void GpsLayer::enableHeading(bool enable) {
    headingEnabled = enable;
    if (mapInterface) mapInterface->invalidate();
}

void GpsLayer::updatePosition(const Coord &position, double horizontalAccuracyM) {
    updatePosition(position, horizontalAccuracyM, false);
}

void GpsLayer::updatePosition(const Coord &position, double horizontalAccuracyM, bool isInitialFollow) {
    if (!mapInterface) return;

    if ((position.x == 0 && position.y == 0 && position.z == 0)) {
        setMode(GpsMode::DISABLED);
        positionValid = false;
        return;
    }
    positionValid = true;

    Coord newPosition = mapInterface->getCoordinateConverterHelper()->convert(
            mapInterface->getMapConfig().mapCoordinateSystem.identifier, position);
    
    // ignore position altitude
    newPosition.z = 0.0;

    if (mode == GpsMode::FOLLOW || mode == GpsMode::FOLLOW_AND_TURN) {
        bool animated = position.systemIdentifier != CoordinateSystemIdentifiers::RENDERSYSTEM();

        if(isInitialFollow && followInitializeZoom) {
            camera->moveToCenterPositionZoom(newPosition, *followInitializeZoom, animated);
        } else {
            camera->moveToCenterPosition(newPosition, animated);
        }
    }

    this->position = newPosition;
    this->horizontalAccuracyM = horizontalAccuracyM;

    if (mapInterface) mapInterface->invalidate();
}

void GpsLayer::updateHeading(float angleHeading) {
    if (!mapInterface) return;

    headingValid = true;
    double currentAngle = fmod(this->angleHeading, 360.0);
    double newAngle = -angleHeading;
    newAngle = fmod(newAngle + 360.0, 360.0);
    if (abs(currentAngle - newAngle) > abs(currentAngle - (newAngle + 360.0))) {
        newAngle += 360.0;
    } else if (abs(currentAngle - newAngle) > abs(currentAngle - (newAngle - 360.0))) {
        newAngle -= 360.0;
    }

    std::lock_guard<std::recursive_mutex> lock(animationMutex);
    if (headingAnimation) headingAnimation->cancel();
    headingAnimation = std::make_shared<DoubleAnimation>(DEFAULT_ANIM_LENGTH,
                                                         currentAngle,
                                                         newAngle,
                                                         InterpolatorFunction::Linear,
                                                         [=](double angleAnim) {
                                                             if (mode == GpsMode::FOLLOW_AND_TURN) {
                                                                 camera->setRotation(angleAnim, false);
                                                             }
                                                             this->angleHeading = fmod(angleAnim + 360.0, 360.0);;
                                                             if (mapInterface) mapInterface->invalidate();
                                                         }, [=] {
                if (mode == GpsMode::FOLLOW_AND_TURN) {
                    camera->setRotation(newAngle, false);
                    this->angleHeading = fmod(newAngle + 360.0f, 360.0f);
                    if (mapInterface) mapInterface->invalidate();
                }
            });
    headingAnimation->start();

    if (mapInterface) mapInterface->invalidate();
}

void GpsLayer::setFollowInitializeZoom(std::optional<float> zoom) {
    followInitializeZoom = zoom;

    if (mapInterface) mapInterface->invalidate();
}

std::shared_ptr<::LayerInterface> GpsLayer::asLayerInterface() {
    return shared_from_this();
}

void GpsLayer::update() {
    std::lock_guard<std::recursive_mutex> lock(animationMutex);
    if (headingAnimation) {
        if (headingAnimation->isFinished()) {
            headingAnimation = nullptr;
        } else {
            headingAnimation->update();
        }
    }
}

std::vector<std::shared_ptr<::RenderPassInterface>> GpsLayer::buildRenderPasses() {
    if (isHidden || !drawLocation || !centerObject || !positionValid) {
        return {};
    }

    std::vector<float> invariantModelMatrix = computeModelMatrix(true, 1.0);
    std::vector<float> accuracyModelMatrix = computeModelMatrix(false, horizontalAccuracyM);
    std::map<int, std::vector<std::shared_ptr<RenderObjectInterface>>> renderPassObjectMap;
    
    for (const auto &config : accuracyObject->getRenderConfig()) {
        renderPassObjectMap[config->getRenderIndex()].push_back(
                std::make_shared<RenderObject>(config->getGraphicsObject(), accuracyModelMatrix));
    }

    if (headingEnabled && headingValid && drawHeadingObjectEnabled) {
        for (const auto &config : headingObject->getRenderConfig()) {
            renderPassObjectMap[config->getRenderIndex()].push_back(
                    std::make_shared<RenderObject>(config->getGraphicsObject(), invariantModelMatrix));
        }
    }

    if (drawCenterObjectEnabled) {
        for (const auto &config : centerObject->getRenderConfig()) {
            renderPassObjectMap[config->getRenderIndex()].push_back(
                    std::make_shared<RenderObject>(config->getGraphicsObject(), invariantModelMatrix));
        }
    }

    std::vector<std::shared_ptr<RenderPassInterface>> renderPasses;
    for (const auto &passEntry : renderPassObjectMap) {
        std::shared_ptr<RenderPass> renderPass = std::make_shared<RenderPass>(RenderPassConfig(passEntry.first),
                                                                              passEntry.second,
                                                                              mask);
        renderPasses.push_back(renderPass);
    }
    return renderPasses;
}

void GpsLayer::onAdded(const std::shared_ptr<MapInterface> &mapInterface) {
    this->mapInterface = mapInterface;
    this->camera = mapInterface->getCamera();
    mapInterface->getTouchHandler()->addListener(shared_from_this());

    setupLayerObjects();
    mapInterface->invalidate();
}

void GpsLayer::onRemoved() {
    mapInterface->getTouchHandler()->removeListener(shared_from_this());
    mapInterface = nullptr;
}

void GpsLayer::pause() {
    if (centerObject) centerObject->getQuadObject()->asGraphicsObject()->clear();
    if (headingObject) headingObject->getQuadObject()->asGraphicsObject()->clear();
    if (accuracyObject) accuracyObject->getQuadObject()->asGraphicsObject()->clear();
    if (mask) {
        if (mask->asGraphicsObject()->isReady()) mask->asGraphicsObject()->clear();
    }
}

void GpsLayer::resume() {

    auto renderingContext = mapInterface->getRenderingContext();

    if (!centerObject->getQuadObject()->asGraphicsObject()->isReady()) {
        auto textureCenter = styleInfo.pointTexture;
        centerObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
        centerObject->getQuadObject()->loadTexture(renderingContext, textureCenter);
    }

    if (!headingObject->getQuadObject()->asGraphicsObject()->isReady()) {
        auto textureHeading = styleInfo.headingTexture;
        headingObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
        headingObject->getQuadObject()->loadTexture(renderingContext, textureHeading);
    }

    if (!accuracyObject->getQuadObject()->asGraphicsObject()->isReady()) {
        Color accuracyColor = styleInfo.accuracyColor;
        accuracyObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
        accuracyObject->setColor(accuracyColor);
    }

    if (mask) {
        if (!mask->asGraphicsObject()->isReady()) mask->asGraphicsObject()->setup(renderingContext);
    }
}

void GpsLayer::hide() {
    isHidden = true;
    if (mapInterface) mapInterface->invalidate();
}

void GpsLayer::show() {
    isHidden = false;
    if (mapInterface) mapInterface->invalidate();
}

bool GpsLayer::onClickConfirmed(const Vec2F &posScreen) {
    resetMode();
    return false;
}

bool GpsLayer::onDoubleClick(const Vec2F &posScreen) {
    resetMode();
    return false;
}

bool GpsLayer::onTwoFingerClick(const Vec2F &posScreen1, const Vec2F &posScreen2) {
    resetMode();
    return false;
}

bool GpsLayer::onMove(const Vec2F &deltaScreen, bool confirmed, bool doubleClick) {
    resetMode();
    return false;
}

bool GpsLayer::onTwoFingerMove(const std::vector<::Vec2F> &posScreenOld, const std::vector<::Vec2F> &posScreenNew) {
    resetMode();
    return false;
}

void GpsLayer::resetMode() {
    if (mode != GpsMode::DISABLED) {
        resetParameters();
        setMode(GpsMode::STANDARD);
    }
}

void GpsLayer::resetParameters() {
    if (mode == GpsMode::FOLLOW_AND_TURN) camera->setRotation(angleHeading < (360 - angleHeading) ? 0 : 360, true);
}

void GpsLayer::setupLayerObjects() {
    auto shaderFactory = mapInterface->getShaderFactory();
    auto objectFactory = mapInterface->getGraphicsObjectFactory();
    auto centerShader = shaderFactory->createAlphaShader();
    auto centerQuad = objectFactory->createQuad(centerShader->asShaderProgramInterface());
    centerObject = std::make_shared<Textured2dLayerObject>(centerQuad, centerShader, mapInterface);
    auto headingShader = shaderFactory->createAlphaShader();
    auto headingQuad = objectFactory->createQuad(headingShader->asShaderProgramInterface());
    headingObject = std::make_shared<Textured2dLayerObject>(headingQuad, headingShader, mapInterface);
    accuracyObject = std::make_shared<Circle2dLayerObject>(mapInterface);

    auto textureCenter = styleInfo.pointTexture;
    float hWidthCenter = textureCenter->getImageWidth() * 0.5f;
    float hHeightCenter = textureCenter->getImageHeight() * 0.5f;
    centerObject->setPositions(QuadCoord(Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), -hWidthCenter, -hHeightCenter, 0.0),
                                         Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), +hWidthCenter, -hHeightCenter, 0.0),
                                         Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), +hWidthCenter, +hHeightCenter, 0.0),
                                         Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), -hWidthCenter, +hHeightCenter, 0.0)));
    auto textureHeading = styleInfo.headingTexture;
    float hWidthHeading = textureHeading->getImageWidth() * 0.5f;
    float hHeightHeading = textureHeading->getImageHeight() * 0.5f;
    headingObject->setPositions(QuadCoord(Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), -hWidthHeading, -hHeightHeading, 0.0),
                                          Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), +hWidthHeading, -hHeightHeading, 0.0),
                                          Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), +hWidthHeading, +hHeightHeading, 0.0),
                                          Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), -hWidthHeading, +hHeightHeading,
                                                0.0)));
    accuracyObject->setColor(styleInfo.accuracyColor);
    accuracyObject->setPosition(Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), 0.0, 0.0, 0.0), 1.0);

    auto renderingContext = mapInterface->getRenderingContext();

    mapInterface->getScheduler()->addTask(std::make_shared<LambdaTask>(
            TaskConfig("GpsLayer_setup_objects", 0, TaskPriority::NORMAL, ExecutionEnvironment::GRAPHICS),
            [=] {
                centerObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
                centerObject->getQuadObject()->loadTexture(renderingContext, textureCenter);
                headingObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
                headingObject->getQuadObject()->loadTexture(renderingContext, textureHeading);
                accuracyObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
            }));
}

std::vector<float> GpsLayer::computeModelMatrix(bool scaleInvariant, double objectScaling) {
    std::vector<float> newMatrix(16, 0);
    Matrix::setIdentityM(newMatrix, 0);

    double scaleFactor = scaleInvariant ? camera->mapUnitsFromPixels(1) * objectScaling : objectScaling;
    Matrix::scaleM(newMatrix, 0.0, scaleFactor, scaleFactor, 1.0);

    Matrix::rotateM(newMatrix, 0.0, -angleHeading, 0.0, 0.0, 1.0);

    Coord renderCoord = mapInterface->getCoordinateConverterHelper()->convertToRenderSystem(position);
    std::vector<float> trMatrix(16, 0);
    Matrix::setIdentityM(trMatrix, 0);
    Matrix::translateM(trMatrix, 0, renderCoord.x, renderCoord.y, renderCoord.z);

    Matrix::multiplyMMC(newMatrix, 0, trMatrix, 0, newMatrix, 0);
    return newMatrix;
}

void GpsLayer::setMaskingObject(const std::shared_ptr<::MaskingObjectInterface> & maskingObject) {
    this->mask = maskingObject;
    if (mapInterface) {
        if (mask) {
            if (!mask->asGraphicsObject()->isReady()) mask->asGraphicsObject()->setup(mapInterface->getRenderingContext());
        }
        mapInterface->invalidate();
    }
}

void GpsLayer::setDrawPoint(bool enable) {
    drawCenterObjectEnabled = enable;
}

void GpsLayer::setDrawHeading(bool enable) {
    drawHeadingObjectEnabled = enable;
}

void GpsLayer::setCallbackHandler(const std::shared_ptr<GpsLayerCallbackInterface> & handler) {
    callbackHandler = handler;
}
