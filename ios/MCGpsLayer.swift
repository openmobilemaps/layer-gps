/*
 * Copyright (c) 2021 Ubique Innovation AG <https://www.ubique.ch>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  SPDX-License-Identifier: MPL-2.0
 */

import Foundation
@_exported import LayerGpsSharedModule
import MapCore
import SwiftUI
import UIKit

public class MCGpsLayer: NSObject, @unchecked Sendable {
    open private(set) var nativeLayer: MCGpsLayerInterface!

    private var callbackHandler = MCGpsCallbackHandler()

    public var modeDidChangeCallback: ((_ mode: MCGpsMode) -> Void)? {
        didSet {
            callbackHandler.modeDidChangeCallback = modeDidChangeCallback
        }
    }

    public var onClickCallback: ((_ coordinate: MCCoord) -> Void)? {
        didSet {
            callbackHandler.onPointClickCallback = onClickCallback
        }
    }

    public init(
        style: MCGpsStyleInfoInterface = .defaultStyle,
        canAskForPermission: Bool = true,
        nativeLayerProvider: ((MCGpsStyleInfoInterface) -> MCGpsLayerInterface?) = MCGpsLayerInterface.create,
        layerIndex: Int? = nil
    ) {
        nativeLayer = nativeLayerProvider(style)
        self.layerIndex = layerIndex

        super.init()

        nativeLayer.setCallbackHandler(callbackHandler)
    }

    public func setMode(_ mode: MCGpsMode) {
        nativeLayer.setMode(mode)
    }

    public var mode: MCGpsMode {
        nativeLayer.getMode()
    }

    public func asLayerInterface() -> MCLayerInterface? {
        nativeLayer.asLayerInterface()
    }

    public var interface: MCLayerInterface? {
        return nativeLayer?.asLayerInterface()
    }

    public var layerIndex: Int?
}

extension MCGpsLayer: Layer, ObservableObject {}

public extension MCGpsStyleInfoInterface {
    static var defaultStyle: MCGpsStyleInfoInterface {

        guard let pointImage = UIImage(resource: .icGpsPoint).cgImage,
            let pointTexture = try? TextureHolder(pointImage),
            let headingImage = UIImage(resource: .icGpsDirection).cgImage,
            let headingTexture = try? TextureHolder(headingImage)
        else {
            fatalError("gps style assets not found")
        }

        guard let style = MCGpsStyleInfoInterface.create(pointTexture, headingTexture: headingTexture, courseTexture: nil, accuracyColor: UIColor(red: 112 / 255, green: 173 / 255, blue: 204 / 255, alpha: 0.2).mapCoreColor) else {
            fatalError("style not creatable")
        }

        return style
    }
}

private class MCGpsCallbackHandler: MCGpsLayerCallbackInterface {
    var modeDidChangeCallback: ((_ mode: MCGpsMode) -> Void)?
    var onPointClickCallback: ((_ coordinate: MCCoord) -> Void)?

    func modeDidChange(_ mode: MCGpsMode) {
        DispatchQueue.main.async {
            self.modeDidChangeCallback?(mode)
        }
    }

    func onPointClick(_ coordinate: MCCoord) {
        DispatchQueue.main.async {
            self.onPointClickCallback?(coordinate)
        }
    }
}
