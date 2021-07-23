
package io.openmobilemaps.gps

import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleObserver
import io.openmobilemaps.gps.shared.gps.GpsLayerInterface
import io.openmobilemaps.gps.shared.gps.GpsStyleInfo
import io.openmobilemaps.mapscore.shared.map.LayerInterface
import io.openmobilemaps.mapscore.shared.map.coordinates.Coord

class GpsLayer(private var style: GpsStyleInfo, private var providerType: GpsProviderType) : LifecycleObserver {

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

}