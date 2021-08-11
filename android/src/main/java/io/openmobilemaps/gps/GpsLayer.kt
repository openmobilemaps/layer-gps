
package io.openmobilemaps.gps

import android.content.Context
import android.location.Location
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleObserver
import androidx.lifecycle.OnLifecycleEvent
import io.openmobilemaps.gps.providers.LocationProviderInterface
import io.openmobilemaps.gps.providers.LocationUpdateListener
import io.openmobilemaps.gps.shared.gps.GpsLayerInterface
import io.openmobilemaps.gps.shared.gps.GpsMode
import io.openmobilemaps.gps.shared.gps.GpsStyleInfo
import io.openmobilemaps.mapscore.shared.map.LayerInterface
import io.openmobilemaps.mapscore.shared.map.coordinates.Coord
import io.openmobilemaps.mapscore.shared.map.coordinates.CoordinateSystemIdentifiers

class GpsLayer(private val style: GpsStyleInfo, locationProvider: LocationProviderInterface) : LifecycleObserver,
	LocationUpdateListener {

	constructor(context: Context, style: GpsStyleInfo, providerType: GpsProviderType) : this (style, providerType.getProvider(context))

	private var locationProvider : LocationProviderInterface = locationProvider
	private val layerInterface = GpsLayerInterface.create(style)
	private var lifecycle: Lifecycle? = null

	fun registerLifecycle(lifecycle: Lifecycle) {
		this.lifecycle?.removeObserver(this)
		lifecycle.addObserver(this)
		this.lifecycle = lifecycle
	}

	fun asLayerInterface() : LayerInterface = layerInterface.asLayerInterface()

	fun updatePosition(position: Coord, horizontalAccuracyM: Double) {
		layerInterface.updatePosition(position, horizontalAccuracyM)
	}

	fun updateHeading(angleHeading: Float) {
		layerInterface.updateHeading(angleHeading)
	}

	fun setMode(mode: GpsMode) {
		if (layerInterface.getMode() != mode) {
			layerInterface.setMode(mode)
		}
	}

	fun setHeadingEnabled(enable: Boolean) {
		layerInterface.enableHeading(enable)
	}

	@OnLifecycleEvent(Lifecycle.Event.ON_START)
	fun onStart() {
		locationProvider.registerLocationUpdateListener(this)
	}

	@OnLifecycleEvent(Lifecycle.Event.ON_STOP)
	fun onStop() {
		locationProvider.unregisterLocationUpdateListener(this)
	}

	fun changeProviderType(context: Context, gpsProviderType: GpsProviderType) {
		locationProvider.unregisterLocationUpdateListener(this)
		locationProvider = gpsProviderType.getProvider(context)
		locationProvider.registerLocationUpdateListener(this)
	}

	override fun onLocationUpdate(newLocation: Location) {
		val coord = Coord(CoordinateSystemIdentifiers.EPSG4326(), newLocation.longitude, newLocation.latitude, newLocation.altitude)
		val accuracy = newLocation.accuracy.toDouble()
		layerInterface.updatePosition(coord, accuracy)
	}


}