package io.openmobilemaps.gps.providers

import android.location.Location
import com.google.android.gms.location.LocationRequest

interface LocationUpdateListener {
	val preferredPriority: Int
		get() = DEFAULT_PRIORITY

	val preferredInterval: Long
		get() = DEFAULT_INTERVAL

	fun onLocationUpdate(newLocation: Location)

	companion object {
		internal const val DEFAULT_PRIORITY = LocationRequest.PRIORITY_HIGH_ACCURACY
		internal const val DEFAULT_INTERVAL = 15 * 1000L
	}
}