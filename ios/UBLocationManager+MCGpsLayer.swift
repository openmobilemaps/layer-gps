import CoreLocation
import UBLocation
import UIKit

private var binders: [LocationManagerLayerBinder] = []
private let bindersQueue = DispatchQueue(
    label: "ch.layergps.ublocationbindersQueue")

extension UBLocationManager {
    public func bindTo(layer: MCGpsLayer, canAskForPermission: Bool = false) {
        let binder = LocationManagerLayerBinder(layer: layer)

        bindersQueue.sync {
            binders.append(binder)
        }

        startLocationMonitoring(
            for: [.location(background: false), .heading(background: false)],
            delegate: binder,
            canAskForPermission: canAskForPermission)

        layer.setMode(.STANDARD)
    }
}

public class LocationManagerLayerBinder: NSObject {
    weak var layer: MCGpsLayer?
    var originalMode: MCGpsMode?
    public init(layer: MCGpsLayer) {
        self.layer = layer
        super.init()
    }
}

extension LocationManagerLayerBinder: UBLocationManagerDelegate {
    public func locationManager(
        _ manager: UBLocationManager,
        requiresPermission permission: UBLocationManager.AuthorizationLevel
    ) {
    }

    public var locationManagerFilterAccuracy: CLLocationAccuracy? { nil }

    public func locationManager(
        _: UBLocationManager,
        grantedPermission _: UBLocationManager.AuthorizationLevel,
        accuracy: UBLocationManager.AccuracyLevel
    ) {
    }

    public func locationManager(permissionDeniedFor manager: UBLocationManager)
    {
        originalMode = layer?.mode
        layer?.setMode(.DISABLED)
    }

    public func locationManager(
        _ manager: UBLocationManager, didUpdateLocations locations: [CLLocation]
    ) {
        guard let location = locations.last else { return }

        layer?.nativeLayer.setDrawPoint(manager.accuracyLevel == .full)
        layer?.nativeLayer.setDrawHeading(manager.accuracyLevel == .full)

        // position might have been set to disabled after error
        if let originalMode {
            layer?.setMode(originalMode)
            self.originalMode = nil
        }

        layer?.nativeLayer.updatePosition(
            .init(
                systemIdentifier: MCCoordinateSystemIdentifiers.epsg4326(),
                x: location.coordinate.longitude,
                y: location.coordinate.latitude,
                z: location.altitude),
            horizontalAccuracyM: location.horizontalAccuracy)
    }

    public func locationManager(_: UBLocationManager, didFailWithError _: Error)
    {
        originalMode = layer?.mode
        layer?.setMode(.DISABLED)
    }

    public func locationManager(
        _: UBLocationManager, didUpdateHeading newHeading: CLHeading
    ) {
        var h = Float(newHeading.trueHeading)

        switch UIDevice.current.orientation {
        case .landscapeRight:
            h += 90
        case .landscapeLeft:
            h -= 90
        default:
            break
        }

        layer?.nativeLayer.updateHeading(h)
    }
}
