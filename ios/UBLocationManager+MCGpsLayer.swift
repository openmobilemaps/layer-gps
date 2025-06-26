import CoreLocation
import UBLocation
import UIKit

private var binders: [LocationManagerLayerBinder] = []
private let bindersQueue = DispatchQueue(label: "ch.layergps.ublocationbindersQueue")

public extension UBLocationManager {
    func bindTo(layer: MCGpsLayer, canAskForPermission: Bool = false) {
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
    public init(layer: MCGpsLayer) {
        self.layer = layer
        super.init()
    }
}

extension LocationManagerLayerBinder: UBLocationManagerDelegate {
    public func locationManager(_ manager: UBLocationManager, requiresPermission permission: UBLocationManager.AuthorizationLevel) {
    }

    public var locationManagerFilterAccuracy: CLLocationAccuracy? { nil }

    public func locationManager(_: UBLocationManager, grantedPermission _: UBLocationManager.AuthorizationLevel, accuracy: UBLocationManager.AccuracyLevel) {
    }

    public func locationManager(permissionDeniedFor manager: UBLocationManager) {
        layer?.setMode(.DISABLED)
    }

    public func locationManager(_ manager: UBLocationManager, didUpdateLocations locations: [CLLocation]) {
        guard let location = locations.last else { return }

        layer?.nativeLayer.setMode(.STANDARD)
        layer?.nativeLayer.setDrawPoint(manager.accuracyLevel == .full)
        layer?.nativeLayer.setDrawHeading(manager.accuracyLevel == .full)

        layer?.nativeLayer
            .updatePosition(
                .init(
                    systemIdentifier: MCCoordinateSystemIdentifiers.epsg4326(),
                    x: location.coordinate.longitude,
                    y: location.coordinate.latitude,
                    z: location.altitude), horizontalAccuracyM: location.horizontalAccuracy)
    }

    public func locationManager(_: UBLocationManager, didFailWithError _: Error) {
        layer?.setMode(.DISABLED)
    }

    public func locationManager(_: UBLocationManager, didUpdateHeading newHeading: CLHeading) {
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
