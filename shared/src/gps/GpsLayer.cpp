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

GpsLayer::GpsLayer(const GpsStyleInfo &styleInfo) : styleInfo(styleInfo) {
}

void GpsLayer::setMode(GpsMode mode) {
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
            break;
        }
        case GpsMode::FOLLOW_AND_TURN: {
            drawLocation = true;
            followModeEnabled = true;
            rotationModeEnabled = true;
            break;
        }
    }

    mapInterface->invalidate();
}

void GpsLayer::updatePosition(const Coord &position, double horizontalAccuracyM) {
    if (position.x == 0 && position.y == 0 && position.z == 0) {
        setMode(GpsMode::DISABLED);
        return;
    }

    this->position = position;
    this->horizontalAccuracyM = horizontalAccuracyM;
    mapInterface->invalidate();
}

void GpsLayer::updateHeading(float angleHeading) {
    this->angleHeading = fmod(angleHeading + 360.0f, 360.0f);
    headingValid = true;
}

std::shared_ptr<::LayerInterface> GpsLayer::asLayerInterface() {
    return shared_from_this();
}

void GpsLayer::update() {

}

std::vector<std::shared_ptr<::RenderPassInterface>> GpsLayer::buildRenderPasses() {
    if (isHidden || !drawLocation || !centerObject) {
        return {};
    }

    std::vector<float> invariantModelMatrix = computeModelMatrix(true, 1.0);
    std::vector<float> accuracyModelMatrix = computeModelMatrix(false, horizontalAccuracyM);
    std::map<int, std::vector<std::shared_ptr<RenderObjectInterface>>> renderPassObjectMap;
    for (const auto &config : accuracyObject->getRenderConfig()) {
        renderPassObjectMap[config->getRenderIndex()].push_back(
                std::make_shared<RenderObject>(config->getGraphicsObject(), accuracyModelMatrix));
    }
    if (headingValid) {
        for (const auto &config : headingObject->getRenderConfig()) {
            renderPassObjectMap[config->getRenderIndex()].push_back(
                    std::make_shared<RenderObject>(config->getGraphicsObject(), invariantModelMatrix));
        }
    }
    for (const auto &config : centerObject->getRenderConfig()) {
        renderPassObjectMap[config->getRenderIndex()].push_back(
                std::make_shared<RenderObject>(config->getGraphicsObject(), invariantModelMatrix));
    }

    std::vector<std::shared_ptr<RenderPassInterface>> renderPasses;
    for (const auto &passEntry : renderPassObjectMap) {
        std::shared_ptr<RenderPass> renderPass = std::make_shared<RenderPass>(RenderPassConfig(passEntry.first),
                                                                              passEntry.second);
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
}

void GpsLayer::pause() {
    if (centerObject) centerObject->getQuadObject()->asGraphicsObject()->clear();
    if (headingObject) headingObject->getQuadObject()->asGraphicsObject()->clear();
    if (accuracyObject) accuracyObject->getQuadObject()->asGraphicsObject()->clear();
}

void GpsLayer::resume() {

    auto renderingContext = mapInterface->getRenderingContext();

    if (!centerObject->getQuadObject()->asGraphicsObject()->isReady()) {
        auto textureCenter = styleInfo.pointTexture;
        centerObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
        centerObject->getQuadObject()->loadTexture(textureCenter);
    }

    if (!headingObject->getQuadObject()->asGraphicsObject()->isReady()) {
        auto textureHeading = styleInfo.headingTexture;
        headingObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
        headingObject->getQuadObject()->loadTexture(textureHeading);
    }

    if (!accuracyObject->getQuadObject()->asGraphicsObject()->isReady()) {
        Color accuracyColor = styleInfo.accuracyColor;
        accuracyObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
        accuracyObject->setColor(accuracyColor);
    }
}

void GpsLayer::hide() {
    isHidden = true;
    if (mapInterface)
        mapInterface->invalidate();
}

void GpsLayer::show() {
    isHidden = false;
    if (mapInterface)
        mapInterface->invalidate();
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
        setMode(GpsMode::STANDARD);
    }
}

void GpsLayer::setupLayerObjects() {
    auto shaderFactory = mapInterface->getShaderFactory();
    auto objectFactory = mapInterface->getGraphicsObjectFactory();
    auto centerShader = shaderFactory->createAlphaShader();
    auto centerQuad = objectFactory->createQuad(centerShader->asShaderProgramInterface());
    centerObject = std::make_shared<Textured2dLayerObject>(centerQuad, centerShader, mapInterface);
    auto headingShader = shaderFactory->createAlphaShader();
    auto headingQuad = objectFactory->createQuad(centerShader->asShaderProgramInterface());
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
                centerObject->getQuadObject()->loadTexture(textureCenter);
                headingObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
                headingObject->getQuadObject()->loadTexture(textureHeading);
                accuracyObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
            }));
}

std::vector<float> GpsLayer::computeModelMatrix(bool scaleInvariant, double objectScaling) {
    std::vector<float> newMatrix(16, 0);
    Matrix::setIdentityM(newMatrix, 0);

    Coord renderCoord = mapInterface->getCoordinateConverterHelper()->convertToRenderSystem(position);

    double scaleFactor = scaleInvariant ? camera->mapUnitsFromPixels(1) * objectScaling : objectScaling;
    Matrix::scaleM(newMatrix, 0.0, scaleFactor, scaleFactor, 1.0);

    Matrix::rotateM(newMatrix, 0.0, -angleHeading, 0.0, 0.0, 1.0);

    //Matrix::translateM(newMatrix, 0, -renderCoord.x, -renderCoord.y, -renderCoord.z);

    return newMatrix;
}