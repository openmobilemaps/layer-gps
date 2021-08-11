package io.openmobilemaps.gps.providers

import android.annotation.SuppressLint
import android.content.Context
import android.location.Location
import android.location.LocationListener
import android.location.LocationManager
import io.openmobilemaps.gps.util.SingletonHolder
import java.util.*

@SuppressLint("MissingPermission")
internal class GpsOnlyLocationProvider private constructor(context: Context) : LocationProviderInterface {

	companion object : SingletonHolder<GpsOnlyLocationProvider, Context>(::GpsOnlyLocationProvider);

	private val locationManager = context.getSystemService(Context.LOCATION_SERVICE) as LocationManager
	private val locationUpdateListeners: MutableSet<LocationUpdateListener> = HashSet()

	private val locationChangeListener: LocationListener = object : LocationListenerAdapter() {
		override fun onLocationChanged(location: Location) {
			locationUpdateListeners.forEach {
				it.onLocationUpdate(location)
			}
		}
	}

	override fun registerLocationUpdateListener(locationUpdateListener: LocationUpdateListener) {
		locationUpdateListeners.add(locationUpdateListener)
		updateLocationUpdateRequest()
	}

	override fun unregisterLocationUpdateListener(locationUpdateListener: LocationUpdateListener) {
		locationUpdateListeners.remove(locationUpdateListener)
		if (locationUpdateListeners.isEmpty()) {
			locationManager.removeUpdates(locationChangeListener)
		} else {
			updateLocationUpdateRequest()
		}
	}

	override fun onListenerRequestParametersChanged() {
		updateLocationUpdateRequest()
	}

	override fun notifyLocationPermissionGranted() {
		updateLocationUpdateRequest()
	}

	private fun updateLocationUpdateRequest() {
		locationManager.removeUpdates(locationChangeListener)
		val shortestInterval = locationUpdateListeners.minOf { it.preferredInterval }
		locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, shortestInterval, 0f, locationChangeListener)
	}

}