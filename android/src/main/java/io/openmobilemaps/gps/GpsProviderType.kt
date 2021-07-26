package io.openmobilemaps.gps

import android.content.Context
import io.openmobilemaps.gps.providers.GpsOnlyLocationProvider
import io.openmobilemaps.gps.providers.LocationProviderInterface

enum class GpsProviderType {
	GPS_ONLY,
	GOOGLE_FUSED;

	fun getProvider(context: Context) : LocationProviderInterface = when(this) {
		GPS_ONLY -> GpsOnlyLocationProvider.getInstance(context)
		GOOGLE_FUSED -> TODO()
	}
}