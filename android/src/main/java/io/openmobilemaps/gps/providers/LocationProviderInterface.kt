package io.openmobilemaps.gps.providers

interface LocationProviderInterface {
	fun registerLocationUpdateListener(locationUpdateListener: LocationUpdateListener)
	fun unregisterLocationUpdateListener(locationUpdateListener: LocationUpdateListener)
	fun onListenerRequestParametersChanged()
	fun notifyLocationPermissionGranted()
}
