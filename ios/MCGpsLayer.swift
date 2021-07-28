/*
 * Copyright (c) 2021 Ubique Innovation AG <https://www.ubique.ch>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  SPDX-License-Identifier: MPL-2.0
 */

import UIKit
import MapCore
import CoreLocation
@_exported import LayerGpsSharedModule

public class MCGpsLayer: NSObject {

    private var layer: MCGpsLayerInterface!

    private var callbackHandler = MCGpsCallbackHandler()

    private let locationManager = UBLocationManager.shared

    public var modeDidChangeCallback: ((_ mode: MCGpsMode) -> Void)? {
        didSet {
            callbackHandler.modeDidChangeCallback = modeDidChangeCallback
        }
    }

    public init(style: MCGpsStyleInfo = .defaultStyle) {
        layer = MCGpsLayerInterface.create(style)

        super.init()

        locationManager.startLocationMonitoring(for: [.location(background: false), .heading(background: false)],
                                                   delegate: self,
                                                   canAskForPermission: true)

        layer.setCallbackHandler(callbackHandler)
        
        setMode(.STANDARD)
    }

    public func setMode(_ mode: MCGpsMode) {
        layer.setMode(mode)
    }

    public var mode: MCGpsMode {
        layer.getMode()
    }

    public func asLayerInterface() -> MCLayerInterface? {
        layer.asLayerInterface()
    }
}

extension MCGpsLayer: UBLocationManagerDelegate {
    public func locationManager(_ manager: UBLocationManager, requiresPermission permission: UBLocationManager.AuthorizationLevel) {}

    public var locationManagerFilterAccuracy: CLLocationAccuracy? { nil }


    public func locationManager(_: UBLocationManager, grantedPermission _: UBLocationManager.AuthorizationLevel, accuracy: UBLocationManager.AccuracyLevel) {}


    public func locationManager(_: UBLocationManager, didUpdateLocations locations: [CLLocation]) {
        guard let location = locations.last else { return }
        layer.updatePosition(.init(systemIdentifier: MCCoordinateSystemIdentifiers.epsg4326(),
                                   x: location.coordinate.longitude,
                                   y: location.coordinate.latitude,
                                   z: 0), horizontalAccuracyM: location.horizontalAccuracy)

    }

    public func locationManager(_: UBLocationManager, didFailWithError _: Error) { }


    public func locationManager(_: UBLocationManager, didUpdateHeading newHeading: CLHeading) {
        var h: Float = Float(newHeading.trueHeading)

        switch UIDevice.current.orientation {
        case .landscapeRight:
            h += 90
        case .landscapeLeft:
            h -= 90
        default:
            break
        }

        layer.updateHeading(h)
    }

}


public extension MCGpsStyleInfo {
    static var defaultStyle: MCGpsStyleInfo {

        let pointImage = UIImage(named: "ic-gps-point", in: .module, compatibleWith: nil)!
        let pointTexture = try! TextureHolder(pointImage.cgImage!)

        let headingImage = UIImage(named: "ic-gps-direction", in: .module, compatibleWith: nil)!
        let headingTexture = try! TextureHolder(headingImage.cgImage!)

        return MCGpsStyleInfo(pointTexture: pointTexture,
                       pointSizePx: .init(x: 50, y: 50),
                       headingTexture: headingTexture,
                       headingSizePx: .init(x: 50, y: 50),
                       accuracyColor:  UIColor.blue.withAlphaComponent(0.5).mapCoreColor)
    }
}

private class MCGpsCallbackHandler: MCGpsLayerCallbackInterface {

    var modeDidChangeCallback: ((_ mode: MCGpsMode) -> Void)?

    func modeDidChange(_ mode: MCGpsMode) {
        DispatchQueue.main.async {
            self.modeDidChangeCallback?(mode)
        }
    }

}
