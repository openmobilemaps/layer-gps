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
#include "CoordinatesUtil.h"

#define DEFAULT_ANIM_LENGTH 100
#define INTERACTION_THRESHOLD_MOVE_CM 0.5
#define INTERACTION_THRESHOLD_ROT_ANGLE 25

GpsLayer::GpsLayer(const /*not-null*/ std::shared_ptr<GpsStyleInfoInterface> & styleInfo) : styleInfo(styleInfo), resetRotationOnInteraction(true) {}


void GpsLayer::setMode(GpsMode mode) {
    GpsLayer::setModeWithRotationReset(mode, true);
}

void GpsLayer::setResetRotationOnMapInteraction(bool resetRotation) {
    resetRotationOnInteraction = resetRotation;
}

void GpsLayer::setModeWithRotationReset(GpsMode mode, bool resetRotation) {
    if (resetRotation) {
        resetParameters();
    }

    if (mode == this->mode) return;

    bool isInitialFollow = this->mode != GpsMode::FOLLOW && this->mode != GpsMode::FOLLOW_AND_TURN;
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
            if (positionValid && position) {
                updatePosition(*position, horizontalAccuracyMapUnits, isInitialFollow);
            }
            break;
        }
        case GpsMode::FOLLOW_AND_TURN: {
            drawLocation = true;
            followModeEnabled = true;
            rotationModeEnabled = true;
            if (positionValid && position) {
                updatePosition(*position, horizontalAccuracyMapUnits, isInitialFollow);
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
    auto lockSelfPtr = shared_from_this();
    auto mapInterface = lockSelfPtr ? lockSelfPtr->mapInterface : nullptr;
    auto camera = mapInterface ? mapInterface->getCamera() : nullptr;
    bool is3d = mapInterface && mapInterface->is3d();
    if (!camera) {
        outstandingUpdate = OutstandingPositionUpdate{position, horizontalAccuracyM, isInitialFollow};
        return;
    }

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

        if (isInitialFollow && followInitializeZoom) {
            camera->moveToCenterPositionZoom(newPosition, *followInitializeZoom, animated);
        } else {
            camera->moveToCenterPosition(newPosition, animated);
        }
    }

    this->position = newPosition;
    if (this->horizontalAccuracyMapUnits != horizontalAccuracyM) {
        accuracyChanged.clear();
    }
    this->horizontalAccuracyMapUnits = horizontalAccuracyM * mapInterface->getMapConfig().mapCoordinateSystem.unitToScreenMeterFactor;


    // only invalidate if the position is visible
    // if we are in follow or follow and turn mode the invalidation is triggered by the camera movement
    bool isFollowMode = mode == GpsMode::FOLLOW || mode == GpsMode::FOLLOW_AND_TURN;
    if (mapInterface && (!isFollowMode || camera->coordIsVisibleOnScreen(newPosition, 0.0f))) {
        mapInterface->invalidate();
    }
}

void GpsLayer::updateHeading(float angleHeading) {
    auto lockSelfPtr = shared_from_this();
    if (!lockSelfPtr) return;

    headingValid = true;
    double currentAngle = fmod(this->angleHeading, 360.0);
    double newAngle = -angleHeading;
    newAngle = fmod(newAngle + 360.0, 360.0);
    if (abs(currentAngle - newAngle) > abs(currentAngle - (newAngle + 360.0))) {
        newAngle += 360.0;
    } else if (abs(currentAngle - newAngle) > abs(currentAngle - (newAngle - 360.0))) {
        newAngle -= 360.0;
    }

    auto mapInterface = lockSelfPtr->mapInterface;
    auto camera = mapInterface ? mapInterface->getCamera() : nullptr;
    if (!camera) {
        this->angleHeading = fmod(newAngle + 360.0f, 360.0f);
        return;
    }

    //do not constantly render frames for small heading adjustments
    auto const diff = std::abs(currentAngle - newAngle);
    if (diff < 0.2) { return; }

    std::lock_guard<std::recursive_mutex> lock(animationMutex);

    // do not bother animating the hading movement if the position is offscreen
    if (mode != GpsMode::FOLLOW_AND_TURN &&
        mapInterface &&
        this->position && positionValid &&
        !coordsutil::checkRectContainsCoord(camera->getVisibleRect(), *this->position, mapInterface->getCoordinateConverterHelper())) {

        this->angleHeading = fmod(newAngle + 360.0f, 360.0f);
        return;
    }


    if (headingAnimation) headingAnimation->cancel();
    headingAnimation = std::make_shared<DoubleAnimation>(DEFAULT_ANIM_LENGTH,
                                                         currentAngle,
                                                         newAngle,
                                                         InterpolatorFunction::Linear,
                                                         [=](double angleAnim) {
                                                             if (mode == GpsMode::FOLLOW_AND_TURN) {
                                                                 camera->setRotation(angleAnim, false);
                                                             }
                                                             this->angleHeading = fmod(angleAnim + 360.0, 360.0);
                                                             if (mapInterface) mapInterface->invalidate();
                                                         }, [=] {
                if (mode == GpsMode::FOLLOW_AND_TURN) {
                    camera->setRotation(newAngle, false);
                }
                this->angleHeading = fmod(newAngle + 360.0f, 360.0f);
                if (mapInterface) mapInterface->invalidate();
            });
    headingAnimation->start();

    if (mapInterface) mapInterface->invalidate();
}

void GpsLayer::enableCourse(bool enable) {
    courseEnabled = enable;
    if (mapInterface) mapInterface->invalidate();
}

void GpsLayer::updateCourse(const GpsCourseInfo & courseInfo) {
    auto lockSelfPtr = shared_from_this();
    auto mapInterface = lockSelfPtr ? lockSelfPtr->mapInterface : nullptr;

    courseValid = true;

    double currentAngle = fmod(this->angleCourse, 360.0);
    double newAngle = -courseInfo.angle;
    newAngle = fmod(newAngle + 360.0, 360.0);
    if (abs(currentAngle - newAngle) > abs(currentAngle - (newAngle + 360.0))) {
        newAngle += 360.0;
    } else if (abs(currentAngle - newAngle) > abs(currentAngle - (newAngle - 360.0))) {
        newAngle -= 360.0;
    }


    double currentScaling = this->courseScaling;
    double newScaling = courseInfo.scaling;

    auto const diffAngle = std::abs(currentAngle - newAngle);
    auto const diffScaling = std::abs(currentScaling - newScaling);

    if (diffAngle < 0.2 && diffScaling < 0.01) { return; }

    std::lock_guard<std::recursive_mutex> lock(animationMutex);

    if (angleCourseAnimation) angleCourseAnimation->cancel();
    angleCourseAnimation = std::make_shared<DoubleAnimation>(DEFAULT_ANIM_LENGTH,
                                                         currentAngle,
                                                         newAngle,
                                                         InterpolatorFunction::Linear,
                                                         [=](double angleAnim) {
                                                             this->angleCourse = fmod(angleAnim + 360.0, 360.0);
                                                             if (mapInterface) mapInterface->invalidate();
                                                         }, [=] {

                this->angleCourse = fmod(newAngle + 360.0f, 360.0f);
                if (mapInterface) mapInterface->invalidate();
            });
    angleCourseAnimation->start();

    if (courseScalingAnimation) courseScalingAnimation->cancel();
    courseScalingAnimation = std::make_shared<DoubleAnimation>(DEFAULT_ANIM_LENGTH,
                                                         currentScaling,
                                                         newScaling,
                                                         InterpolatorFunction::Linear,
                                                         [=](double scaleAnim) {
                                                             this->courseScaling = scaleAnim;
                                                             if (mapInterface) mapInterface->invalidate();
                                                         }, [=] {

                this->courseScaling = newScaling;
                if (mapInterface) mapInterface->invalidate();
            });
    courseScalingAnimation->start();

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
    auto mapInterface = this->mapInterface;
    auto renderingContext = mapInterface ? mapInterface->getRenderingContext() : nullptr;
    bool is3d = mapInterface && mapInterface->is3d();
    if (is3d && renderingContext) {
        if (accuracyObject && !accuracyChanged.test_and_set()) {
            // Due to the subdivision and the vertex position displacement on the unit sphere, a change in scale of the accuracy object needs a vertex recomputation
            accuracyObject->setPosition(Coord(CoordinateSystemIdentifiers::UnitSphere(), -M_PI, -M_PI_2, 0.0),
                                        horizontalAccuracyMapUnits);
            accuracyObject->getQuadObject()->asGraphicsObject()->setup(renderingContext);
        }
    }

    std::lock_guard<std::recursive_mutex> lock(animationMutex);

    std::vector<std::shared_ptr<AnimationInterface>> animations = {headingAnimation, angleCourseAnimation, courseScalingAnimation};

    for (auto &animation: animations) {
        if (animation) {
            if (animation->isFinished()) {
                animation = nullptr;
            } else {
                animation->update();
            }
        }
    }
}

std::vector<std::shared_ptr<::RenderPassInterface>> GpsLayer::buildRenderPasses() {
    if (isHidden || !drawLocation || !positionValid) {
        return {};
    }

    auto mapInterface = this->mapInterface;
    bool is3d = mapInterface && mapInterface->is3d();

    std::vector<float> const &scaleInvariantModelMatrix = computeModelMatrix(true, 1.0, false, false);
    std::vector<float> const &accuracyModelMatrix = computeModelMatrix(false, is3d ? 1.0 : horizontalAccuracyMapUnits, false, false);
    std::vector<float> const &courseModelMatrix = computeModelMatrix(true, courseScaling, false, true);

    std::map<int, std::vector<std::shared_ptr<RenderObjectInterface>>> renderPassObjectMap;

    for (const auto &config : accuracyObject->getRenderConfig()) {
        renderPassObjectMap[config->getRenderIndex()].push_back(
                std::make_shared<RenderObject>(config->getGraphicsObject(), accuracyModelMatrix));
    }

    if (headingObject && headingEnabled && headingValid && drawHeadingObjectEnabled) {
        for (const auto &config : headingObject->getRenderConfig()) {
            renderPassObjectMap[config->getRenderIndex()].push_back(
                    std::make_shared<RenderObject>(config->getGraphicsObject(), scaleInvariantModelMatrix));
        }
    }

    if (courseObject && courseEnabled && courseValid) {
        for (const auto &config : courseObject->getRenderConfig()) {
            renderPassObjectMap[config->getRenderIndex()].push_back(
                    std::make_shared<RenderObject>(config->getGraphicsObject(), courseModelMatrix));
        }
    }

    if (centerObject && drawCenterObjectEnabled) {
        auto const &centerObjectModelMatrix = pointRotationInvariantEnabled ? computeModelMatrix(true, 1.0, pointRotationInvariantEnabled, false) : scaleInvariantModelMatrix;
        for (const auto &config : centerObject->getRenderConfig()) {
            renderPassObjectMap[config->getRenderIndex()].push_back(
                    std::make_shared<RenderObject>(config->getGraphicsObject(), centerObjectModelMatrix));
        }
    }

    std::vector<std::shared_ptr<RenderPassInterface>> renderPasses;
    for (const auto &passEntry : renderPassObjectMap) {
        std::shared_ptr<RenderPass> renderPass = std::make_shared<RenderPass>(RenderPassConfig(renderPassIndex, false),
                                                                              passEntry.second,
                                                                              mask, renderTarget);
        renderPasses.push_back(renderPass);
    }
    return renderPasses;
}

void GpsLayer::onAdded(const std::shared_ptr<MapInterface> &mapInterface, int32_t layerIndex) {
    this->mapInterface = mapInterface;
    mapInterface->getTouchHandler()->insertListener(shared_from_this(), layerIndex);
    mapInterface->getCamera()->addListener(shared_from_this());

    setupLayerObjects();

    if (auto outstandingUpdate = this->outstandingUpdate) {
        updatePosition(outstandingUpdate->position, outstandingUpdate->horizontalAccuracyM, outstandingUpdate->isInitialFollow);
        this->outstandingUpdate = std::nullopt;
    }

    mapInterface->invalidate();
}

void GpsLayer::onRemoved() {
    auto lockSelfPtr = shared_from_this();
    auto mapInterface = lockSelfPtr ? lockSelfPtr->mapInterface : nullptr;
    if (mapInterface) {
        mapInterface->getTouchHandler()->removeListener(shared_from_this());
        mapInterface->getCamera()->removeListener(shared_from_this());
    }
    mapInterface = nullptr;
}

void GpsLayer::pause() {
    if (centerObject) centerObject->getGraphicsObject()->clear();
    if (headingObject) headingObject->getGraphicsObject()->clear();
    if (accuracyObject) accuracyObject->getGraphicsObject()->clear();
    if (courseObject) courseObject->getGraphicsObject()->clear();

    if (mask) {
        auto obj = mask->asGraphicsObject();
        if (obj->isReady()) { obj->clear(); };
    }
}

void GpsLayer::resume() {
    auto lockSelfPtr = shared_from_this();
    auto mapInterface = lockSelfPtr ? lockSelfPtr->mapInterface : nullptr;
    auto renderingContext = mapInterface ? mapInterface->getRenderingContext() : nullptr;
    if (!renderingContext) {
        return;
    }

    if (centerObject && !centerObject->getGraphicsObject()->isReady()) {
        auto textureCenter = styleInfo->getPointTexture();
        centerObject->getGraphicsObject()->setup(renderingContext);
        centerObject->getQuadObject()->loadTexture(renderingContext, textureCenter);
    }

    if (headingObject && !headingObject->getGraphicsObject()->isReady()) {
        auto textureHeading = styleInfo->getHeadingTexture();
        headingObject->getGraphicsObject()->setup(renderingContext);
        headingObject->getQuadObject()->loadTexture(renderingContext, textureHeading);
    }

    if (!accuracyObject->getGraphicsObject()->isReady()) {
        Color accuracyColor = styleInfo->getAccuracyColor();
        accuracyObject->getGraphicsObject()->setup(renderingContext);
        accuracyObject->setColor(accuracyColor);
    }

    if (courseObject && !courseObject->getGraphicsObject()->isReady()) {
        auto textureCourse = styleInfo->getCourseTexture();
        courseObject->getGraphicsObject()->setup(renderingContext);
        courseObject->getQuadObject()->loadTexture(renderingContext, textureCourse);
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
    auto lockSelfPtr = shared_from_this();
    auto mapInterface = lockSelfPtr ? lockSelfPtr->mapInterface : nullptr;
    auto camera = mapInterface ? mapInterface->getCamera() : nullptr;
    auto conversionHelper = mapInterface ? mapInterface->getCoordinateConverterHelper() : nullptr;
    bool is3d = mapInterface && mapInterface->is3d();
    if (!camera || !conversionHelper) {
        return false;
    }

    if (callbackHandler && mapInterface && position) {
        Coord clickCoords = camera->coordFromScreenPosition(posScreen);
        if (clickCoords.systemIdentifier == -1 && clickCoords.x == 0 && clickCoords.y == 0  && clickCoords.z == 0 && is3d) {
            return false;
        }

        double angle = -(camera->getRotation() * M_PI / 180.0);
        double sinAng = std::sin(angle);
        double cosAng = std::cos(angle);

        Vec2F anchor(0.5, 0.5);
        float ratioLeftRight = std::clamp(anchor.x, 0.0f, 1.0f);
        float ratioTopBottom = std::clamp(anchor.y, 0.0f, 1.0f);
        float leftW = pointWidth * ratioLeftRight;
        float topH = pointHeight * ratioTopBottom;
        float rightW = pointWidth * (1.0f - ratioLeftRight);
        float bottomH = pointHeight * (1.0f - ratioTopBottom);

        Coord iconPos = conversionHelper->convert(clickCoords.systemIdentifier, *position);

        leftW = camera->mapUnitsFromPixels(leftW);
        topH = camera->mapUnitsFromPixels(topH);
        rightW = camera->mapUnitsFromPixels(rightW);
        bottomH = camera->mapUnitsFromPixels(bottomH);

        Vec2D clickPos = Vec2D(clickCoords.x - iconPos.x, clickCoords.y - iconPos.y);

        float newX = cosAng * clickPos.x - sinAng * clickPos.y;
        float newY = sinAng * clickPos.x + cosAng * clickPos.y;
        clickPos.x = newX;
        clickPos.y = newY;

        if (clickPos.x > -leftW && clickPos.x < rightW &&
            clickPos.y < topH && clickPos.y > -bottomH) {
            callbackHandler->onPointClick(*position);
            return true;
        }

    }

    return false;
}

bool GpsLayer::onMoveComplete() {
    resetAccInteraction();
    return false;
}

bool GpsLayer::onTwoFingerMove(const std::vector<::Vec2F> &posScreenOld, const std::vector<::Vec2F> &posScreenNew) {
    {
        std::lock_guard<std::recursive_mutex> lock(interactionMutex);
        isPinchMove = true;
    }
    return false;
}

bool GpsLayer::onTwoFingerMoveComplete() {
    resetAccInteraction();
    return false;
}

void GpsLayer::clearTouch() {
    resetAccInteraction();
}

void GpsLayer::resetMode() {
    if (mode != GpsMode::DISABLED) {
        setModeWithRotationReset(GpsMode::STANDARD, resetRotationOnInteraction);
    }
}

void GpsLayer::resetParameters() {
    auto lockSelfPtr = shared_from_this();
    auto mapInterface = lockSelfPtr ? lockSelfPtr->mapInterface : nullptr;
    auto camera = mapInterface ? mapInterface->getCamera() : nullptr;
    if (!camera) return;
    if (mode == GpsMode::FOLLOW_AND_TURN) camera->setRotation(angleHeading < (360 - angleHeading) ? 0 : 360, true);
}

QuadCoord GpsLayer::getQuadCoord(std::shared_ptr<TextureHolderInterface> texture) {
    auto mapInterface = this->mapInterface;
    bool is3d = mapInterface && mapInterface->is3d();

    float hWidth = texture->getImageWidth() * 0.5f * CoordinateSystemIdentifiers::unitToMeterFactor(mapInterface->getMapConfig().mapCoordinateSystem.identifier);// * mapInterface->getMapConfig().mapCoordinateSystem.unitToScreenMeterFactor;
    float hHeight = texture->getImageHeight() * 0.5f * CoordinateSystemIdentifiers::unitToMeterFactor(mapInterface->getMapConfig().mapCoordinateSystem.identifier);// * mapInterface->getMapConfig().mapCoordinateSystem.unitToScreenMeterFactor;

    Vec2F centerCoord = Vec2F(0.0, 0.0);
    if (is3d) {
        centerCoord.x = - M_PI;
        centerCoord.y = - M_PI_2;
    }

    return QuadCoord(Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), centerCoord.x - hWidth, centerCoord.y + hHeight, 0.0),
                     Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), centerCoord.x + hWidth, centerCoord.y + hHeight, 0.0),
                     Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), centerCoord.x + hWidth, centerCoord.y - hHeight, 0.0),
                     Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), centerCoord.x - hWidth, centerCoord.y - hHeight, 0.0));
}

void GpsLayer::setupLayerObjects() {
    auto lockSelfPtr = shared_from_this();
    auto mapInterface = lockSelfPtr ? lockSelfPtr->mapInterface : nullptr;
    auto scheduler = mapInterface ? mapInterface->getScheduler() : nullptr;
    auto shaderFactory = mapInterface ? mapInterface->getShaderFactory() : nullptr;
    auto objectFactory = mapInterface ? mapInterface->getGraphicsObjectFactory() : nullptr;
    auto is3d = mapInterface && mapInterface->is3d();
    if (!scheduler || !shaderFactory || !objectFactory) {
        return;
    }

    // Center
    auto textureCenter = styleInfo->getPointTexture();
    if (textureCenter) {
        std::shared_ptr<AlphaShaderInterface> centerShader;
        if (is3d) {
            centerShader = shaderFactory->createUnitSphereAlphaShader();
        } else {
            centerShader = shaderFactory->createAlphaShader();
        }

        auto centerQuad = objectFactory->createQuad(centerShader->asShaderProgramInterface());
        if (is3d) {
            centerQuad->setSubdivisionFactor(0);
        }
#if DEBUG
        centerQuad->asGraphicsObject()->setDebugLabel("GpsLayer_centerQuad");
#endif
        centerObject = std::make_shared<Textured2dLayerObject>(centerQuad, centerShader, mapInterface, is3d);
        centerObject->setPositions(getQuadCoord(textureCenter));

        pointWidth = textureCenter->getImageWidth();
        pointHeight = textureCenter->getImageHeight();
    }
    
    // Accuracy
    accuracyObject = std::make_shared<Circle2dLayerObject>(mapInterface);
#if DEBUG
    accuracyObject->getGraphicsObject()->setDebugLabel("GpsLayer_accuracyObject");
#endif
    if (is3d) {
        accuracyObject->getQuadObject()->setSubdivisionFactor(SUBDIVISION_FACTOR_3D_DEFAULT);
        // Position at WGS84 0/0 in the UnitSphere system
        accuracyObject->setPosition(Coord(CoordinateSystemIdentifiers::UnitSphere(), - M_PI, - M_PI_2, 0.0), 1.0);
    } else {
        accuracyObject->setPosition(Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), 0.0, 0.0, 0.0), 1.0);
    }
    accuracyObject->setColor(styleInfo->getAccuracyColor());

    // Heading
    auto textureHeading = styleInfo->getHeadingTexture();
    if (textureHeading) {
        std::shared_ptr<AlphaShaderInterface> headingShader;
        if (is3d) {
            headingShader = shaderFactory->createUnitSphereAlphaShader();
        } else {
            headingShader = shaderFactory->createAlphaShader();
        }
        auto headingQuad = objectFactory->createQuad(headingShader->asShaderProgramInterface());
        headingObject = std::make_shared<Textured2dLayerObject>(headingQuad, headingShader, mapInterface, is3d);
        headingObject->setPositions(getQuadCoord(textureHeading));
#if DEBUG
        headingObject->getGraphicsObject()->setDebugLabel("GpsLayer_headingQuad");
#endif
        if (is3d) {
            headingObject->getQuadObject()->setSubdivisionFactor(0);
        }
    }
    
    // Course
    auto textureCourse = styleInfo->getCourseTexture();
    if (textureCourse) {
        std::shared_ptr<AlphaShaderInterface> courseShader;
        if (mapInterface->is3d()) {
            courseShader = shaderFactory->createUnitSphereAlphaShader();
        } else {
            courseShader = shaderFactory->createAlphaShader();
        }
        auto courseQuad = objectFactory->createQuad(courseShader->asShaderProgramInterface());
        courseObject = std::make_shared<Textured2dLayerObject>(courseQuad, courseShader, mapInterface, mapInterface->is3d());
        courseObject->setPositions(getQuadCoord(textureCourse));
#if DEBUG
        courseObject->getGraphicsObject()->setDebugLabel("GpsLayer_courseObject");
#endif
        if (is3d) {
            courseObject->getQuadObject()->setSubdivisionFactor(SUBDIVISION_FACTOR_3D_DEFAULT);
        }
    }

    auto renderingContext = mapInterface->getRenderingContext();
    if (!renderingContext) {
        return;
    }

    if (textureCenter) {
        centerObject->getGraphicsObject()->setup(renderingContext);
        centerObject->getQuadObject()->loadTexture(renderingContext, textureCenter);
    }

    accuracyObject->getGraphicsObject()->setup(renderingContext);
    
    if (textureHeading) {
        headingObject->getGraphicsObject()->setup(renderingContext);
        headingObject->getQuadObject()->loadTexture(renderingContext, textureHeading);
    }
    
    if (textureCourse) {
        courseObject->getGraphicsObject()->setup(renderingContext);
        courseObject->getQuadObject()->loadTexture(renderingContext, textureCourse);
    }
    
    mapInterface->invalidate();
}

std::vector<float> GpsLayer::computeModelMatrix(bool scaleInvariant, double objectScaling, bool rotationInvariant, bool useCourseAngle) {
    auto lockSelfPtr = shared_from_this();
    auto mapInterface = lockSelfPtr ? lockSelfPtr->mapInterface : nullptr;
    auto camera = mapInterface ? mapInterface->getCamera() : nullptr;
    auto conversionHelper = mapInterface ? mapInterface->getCoordinateConverterHelper() : nullptr;

    std::vector<float> newMatrix(16, 0);
    Matrix::setIdentityM(newMatrix, 0);

    auto currentPosition = position;
    if (!camera || !conversionHelper || !currentPosition) {
        return newMatrix;
    }

    bool is3d = mapInterface->is3d();

    if (is3d) {
        //return newMatrix;
        Matrix::mTranslated(newMatrix, 0, -1.0, 0.0, 0.0);
        // Rotate around y axis for longitudinal change ([0, 2*pi])
        Matrix::rotateM(newMatrix, 0, (currentPosition->x + M_PI) * 180.0f / M_PI, 0.0, 1.0, 0.0);
        // Rotate around z axis for latitudinal change ([-pi/2, pi/2])
        Matrix::rotateM(newMatrix, 0, (currentPosition->y + M_PI_2) * 180.0f / M_PI, 0.0, 0.0, 1.0);

        if (rotationInvariant) {
            Matrix::rotateM(newMatrix, 0.0, camera->getRotation(), 1.0, 0.0, 0.0);
        } else {
            if (useCourseAngle) {
                Matrix::rotateM(newMatrix, 0.0, angleCourse, 1.0, 0.0, 0.0);
            } else {
                Matrix::rotateM(newMatrix, 0.0, angleHeading, 1.0, 0.0, 0.0);
            }
        }

        Matrix::mTranslated(newMatrix, 0, 1.0, 0.0, 0.0);

        double scaleFactor = scaleInvariant
                             ? objectScaling * camera->mapUnitsFromPixels(1.0) / CoordinateSystemIdentifiers::unitToMeterFactor(
                        mapInterface->getMapConfig().mapCoordinateSystem.identifier)
                             : objectScaling;
        Matrix::scaleM(newMatrix, 0.0, scaleFactor, scaleFactor, scaleFactor);

    } else {
        double scaleFactor = scaleInvariant ? camera->mapUnitsFromPixels(1.0) * objectScaling : objectScaling;
        Matrix::scaleM(newMatrix, 0.0, scaleFactor, scaleFactor, 1.0);

        if (rotationInvariant) {
            Matrix::rotateM(newMatrix, 0.0, -camera->getRotation(), 0.0, 0.0, 1.0);
        } else {
            if (useCourseAngle) {
                Matrix::rotateM(newMatrix, 0.0, -angleCourse, 0.0, 0.0, 1.0);
            } else {
                Matrix::rotateM(newMatrix, 0.0, -angleHeading, 0.0, 0.0, 1.0);
            }
        }

        // Translate to final position in 2d space
        Coord renderCoord = mapInterface ? mapInterface->getCoordinateConverterHelper()->convertToRenderSystem(*position) :
                            Coord(CoordinateSystemIdentifiers::RENDERSYSTEM(), 0.0, 0.0, 0.0);
        std::vector<float> trMatrix(16, 0);
        Matrix::setIdentityM(trMatrix, 0);
        Matrix::translateM(trMatrix, 0, renderCoord.x, renderCoord.y, renderCoord.z);
        Matrix::multiplyMMC(newMatrix, 0, trMatrix, 0, newMatrix, 0);
    }

    return newMatrix;
}

void GpsLayer::setMaskingObject(const std::shared_ptr<::MaskingObjectInterface> &maskingObject) {
    auto lockSelfPtr = shared_from_this();
    auto mapInterface = lockSelfPtr ? lockSelfPtr->mapInterface : nullptr;
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

void GpsLayer::setCallbackHandler(const std::shared_ptr<GpsLayerCallbackInterface> &handler) {
    callbackHandler = handler;
}

void GpsLayer::updateStyle(const /*not-null*/ std::shared_ptr<GpsStyleInfoInterface> & styleInfo) {
    auto lockSelfPtr = shared_from_this();
    auto mapInterface = lockSelfPtr ? lockSelfPtr->mapInterface : nullptr;
    auto scheduler = mapInterface ? mapInterface->getScheduler() : nullptr;
    this->styleInfo = styleInfo;
    if (scheduler) {
        std::weak_ptr<GpsLayer> weakSelfPtr = std::dynamic_pointer_cast<GpsLayer>(shared_from_this());
        scheduler->addTask(std::make_shared<LambdaTask>(
                TaskConfig("GpsLayer_setup_objects", 0, TaskPriority::NORMAL, ExecutionEnvironment::GRAPHICS),
                [weakSelfPtr] {
                    if (auto selfPtr = weakSelfPtr.lock()) {
                        selfPtr->setupLayerObjects();
                    }
                }));
    }
}


void GpsLayer::enablePointRotationInvariant(bool enable) {
    pointRotationInvariantEnabled = enable;
}

void GpsLayer::onMapInteraction() {
    auto selfLockPtr = shared_from_this();
    auto mapInterface = selfLockPtr->mapInterface;
    auto camera = mapInterface ? mapInterface->getCamera() : nullptr;
    if (!camera) {
        return;
    }

    if (mode != GpsMode::FOLLOW && mode != GpsMode::FOLLOW_AND_TURN) {
        return;
    }

    Coord center = camera->getCenterPosition();
    double accDistanceUnits = 0.0;
    bool isPinch = false;
    {
        std::lock_guard<std::recursive_mutex> lock(interactionMutex);
        if (lastCenter) {
            accInteractionMove.x = accInteractionMove.x + (center.x - lastCenter->x);
            accInteractionMove.y = accInteractionMove.y + (center.y - lastCenter->y);
        }
        lastCenter = center;

        accDistanceUnits = sqrt(accInteractionMove.x * accInteractionMove.x + accInteractionMove.y * accInteractionMove.y);
        isPinch = isPinchMove;
    }
    double accDistanceCm = accDistanceUnits / camera->mapUnitsFromPixels(1) / camera->getScreenDensityPpi() * 2.54;
    if (accDistanceCm > INTERACTION_THRESHOLD_MOVE_CM * (isPinch ? 4.0 : 1.0)) {
        resetMode();
        resetAccInteraction();
        return;
    }

    if (mode != GpsMode::FOLLOW) {
        return;
    }

    double rotation = camera->getRotation();
    double accRotationDeg = 0.0;
    {
        std::lock_guard<std::recursive_mutex> lock(interactionMutex);
        if (lastRotation) {
            accRotation = accRotation + (rotation - *lastRotation);
        }
        lastRotation = rotation;
        accRotationDeg = accRotation;
    }

    if (abs(accRotationDeg) > INTERACTION_THRESHOLD_ROT_ANGLE) {
        resetMode();
        resetAccInteraction();
        return;
    }
}

void GpsLayer::resetAccInteraction() {
    {
        std::lock_guard<std::recursive_mutex> lock(interactionMutex);
        accInteractionMove.x = 0.0;
        accInteractionMove.y = 0.0;
        accRotation = 0.0;
        isPinchMove = false;
        lastCenter = std::nullopt;
        lastRotation = std::nullopt;
    }
    if (mode == GpsMode::FOLLOW || mode == GpsMode::FOLLOW_AND_TURN) {
        if (positionValid && position) {
            updatePosition(*position, horizontalAccuracyMapUnits, false);
        }
    }
    if (mode == GpsMode::FOLLOW_AND_TURN) {
        updateHeading(angleHeading);
    }
}

void GpsLayer::setRenderPassIndex(int32_t index) {
    this->renderPassIndex = index;
}
