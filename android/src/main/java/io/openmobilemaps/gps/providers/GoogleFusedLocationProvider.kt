/*
 * Copyright (c) 2021 Ubique Innovation AG <https://www.ubique.ch>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  SPDX-License-Identifier: MPL-2.0
 */

package io.openmobilemaps.gps.providers

import android.annotation.SuppressLint
import android.content.Context
import android.location.Location
import android.os.Looper
import com.google.android.gms.location.*
import com.google.android.gms.tasks.Task
import io.openmobilemaps.gps.util.SingletonHolder
import io.openmobilemaps.mapscore.shared.map.coordinates.Coord
import io.openmobilemaps.mapscore.shared.map.coordinates.CoordinateSystemIdentifiers

@SuppressLint("MissingPermission")
class GoogleFusedLocationProvider private constructor(context: Context) : LocationProviderInterface {

	companion object : SingletonHolder<GoogleFusedLocationProvider, Context>(::GoogleFusedLocationProvider) {
		const val DEFAULT_PRIORITY = LocationRequest.PRIORITY_HIGH_ACCURACY
		const val DEFAULT_INTERVAL_HIGH = 5 * 1000L
		const val DEFAULT_FASTEST_INTERVAL = 500L
	}

	private val fusedLocationClient: FusedLocationProviderClient
	private val locationCallback: LocationCallback
	private val locationRequest: LocationRequest
	private val locationUpdateListeners: MutableSet<LocationUpdateListener> = HashSet()
	private var lastLocation: Location? = null

	init {
		locationRequest = LocationRequest.create()
		locationRequest.priority = DEFAULT_PRIORITY
		locationRequest.interval = DEFAULT_INTERVAL_HIGH
		locationRequest.fastestInterval = DEFAULT_FASTEST_INTERVAL
		locationCallback = object : LocationCallback() {
			override fun onLocationResult(locationResult: LocationResult) {
				publishLocationUpdate(locationResult.lastLocation)
			}
		}
		fusedLocationClient = LocationServices.getFusedLocationProviderClient(context)

		try {
			fusedLocationClient.lastLocation.addOnSuccessListener { location ->
				if (location != null) {
					publishLocationUpdate(location)
				}
			}
		} catch (e: Exception) {
			// Ignore
		}
	}

	override fun registerLocationUpdateListener(locationUpdateListener: LocationUpdateListener) {
		val isFirstListener = locationUpdateListeners.isEmpty()
		locationUpdateListeners.add(locationUpdateListener)
		lastLocation?.let { locationUpdateListener.onLocationUpdate(it) }
		updateRequestParameters(isFirstListener)
	}

	override fun unregisterLocationUpdateListener(locationUpdateListener: LocationUpdateListener) {
		locationUpdateListeners.remove(locationUpdateListener)
		if (locationUpdateListeners.isEmpty()) {
			fusedLocationClient.removeLocationUpdates(locationCallback)
		} else {
			updateRequestParameters(false)
		}
	}

	override fun onListenerRequestParametersChanged() {
		updateRequestParameters(false)
	}

	@SuppressLint("MissingPermission")
	override fun notifyLocationPermissionGranted() {
		if (locationUpdateListeners.isNotEmpty()) {
			fusedLocationClient.removeLocationUpdates(locationCallback)
			fusedLocationClient.requestLocationUpdates(locationRequest, locationCallback, Looper.getMainLooper())
		}
	}

	@SuppressLint("MissingPermission")
	private fun updateRequestParameters(isFirstListener: Boolean) {
		var newPriority = Int.MAX_VALUE
		var newInterval = Long.MAX_VALUE
		for (listener in locationUpdateListeners) {
			val listenerPrio = listener.preferredPriority
			if (listenerPrio < newPriority) newPriority = listenerPrio
			val listenerIval = listener.preferredInterval
			if (listenerIval < newInterval) newInterval = listenerIval
		}
		val hasChanged = newPriority != Int.MAX_VALUE && newInterval != Long.MAX_VALUE &&
				(locationRequest.priority != newPriority || locationRequest.interval != newInterval)
		if (hasChanged) locationRequest.setPriority(newPriority).interval = newInterval

		if (locationUpdateListeners.isEmpty()) return

		if (hasChanged) {
			fusedLocationClient.removeLocationUpdates(locationCallback)
				.addOnCompleteListener { task: Task<Void?>? ->
					fusedLocationClient.requestLocationUpdates(
						locationRequest,
						locationCallback,
						Looper.getMainLooper()
					)
				}
		} else if (isFirstListener) {
			fusedLocationClient.requestLocationUpdates(locationRequest, locationCallback, Looper.getMainLooper())
		}
	}

	private fun publishLocationUpdate(location: Location?) {
		lastLocation = location
		location?.let {
			locationUpdateListeners.forEach { listener ->
				listener.onLocationUpdate(it)
			}
		}
	}

	override fun getLastLocation(): Coord? {
		return lastLocation?.let { Coord(CoordinateSystemIdentifiers.EPSG4326(), it.longitude, it.latitude, it.altitude) }
	}
}