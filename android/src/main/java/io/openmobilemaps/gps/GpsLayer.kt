/*
 * Copyright (c) 2021 Ubique Innovation AG <https://www.ubique.ch>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  SPDX-License-Identifier: MPL-2.0
 */

package io.openmobilemaps.gps

import android.content.Context
import android.location.Location
import androidx.lifecycle.DefaultLifecycleObserver
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleObserver
import androidx.lifecycle.LifecycleOwner
import io.openmobilemaps.gps.compass.CompassProvider
import io.openmobilemaps.gps.compass.CompassUpdateListener
import io.openmobilemaps.gps.providers.LocationProviderInterface
import io.openmobilemaps.gps.providers.LocationUpdateListener
import io.openmobilemaps.gps.shared.gps.GpsLayerCallbackInterface
import io.openmobilemaps.gps.shared.gps.GpsLayerInterface
import io.openmobilemaps.gps.shared.gps.GpsMode
import io.openmobilemaps.gps.shared.gps.GpsStyleInfo
import io.openmobilemaps.mapscore.shared.map.LayerInterface
import io.openmobilemaps.mapscore.shared.map.coordinates.Coord
import io.openmobilemaps.mapscore.shared.map.coordinates.CoordinateSystemIdentifiers

class GpsLayer(
	context: Context,
	style: GpsStyleInfo,
	initialLocationProvider: LocationProviderInterface?,
) : GpsLayerCallbackInterface(), LifecycleObserver, LocationUpdateListener, CompassUpdateListener {

	constructor(
		context: Context,
		style: GpsStyleInfo,
		providerType: GpsProviderType,
	) : this(context, style, providerType.getProvider(context))

	private var locationProvider: LocationProviderInterface? = initialLocationProvider
	private var compassProvider: CompassProvider? = CompassProvider.getInstance(context)
	private var layerInterface: GpsLayerInterface? = GpsLayerInterface.create(style).apply {
		setCallbackHandler(this@GpsLayer)
	}

	private var lifecycle: Lifecycle? = null
	private var isHeadingEnabled = true

	private var modeChangedListener: ((GpsMode) -> Unit)? = null
	private var onClickListener: ((Coord) -> Unit)? = null

	private val lifecycleObserver = object : DefaultLifecycleObserver {
		override fun onStart(owner: LifecycleOwner) {
			locationProvider?.registerLocationUpdateListener(this@GpsLayer)
			if (isHeadingEnabled) {
				compassProvider?.registerCompassUpdateListener(this@GpsLayer)
			}
		}

		override fun onStop(owner: LifecycleOwner) {
			locationProvider?.unregisterLocationUpdateListener(this@GpsLayer)
			compassProvider?.unregisterCompassUpdateListener(this@GpsLayer)
		}

		override fun onDestroy(owner: LifecycleOwner) {
			layerInterface = null
			locationProvider = null
			compassProvider = null
		}
	}

	fun registerLifecycle(lifecycle: Lifecycle) {
		this.lifecycle?.removeObserver(lifecycleObserver)
		lifecycle.addObserver(lifecycleObserver)
		this.lifecycle = lifecycle
	}

	fun asLayerInterface(): LayerInterface = requireLayerInterface().asLayerInterface()

	fun updatePosition(position: Coord, horizontalAccuracyM: Double) {
		requireLayerInterface().updatePosition(position, horizontalAccuracyM)
	}

	fun updateHeading(angleHeading: Float) {
		requireLayerInterface().updateHeading(angleHeading)
	}

	fun setMode(mode: GpsMode) {
		if (requireLayerInterface().getMode() != mode) {
			requireLayerInterface().setMode(mode)
		}
	}

	fun setHeadingEnabled(enable: Boolean) {
		isHeadingEnabled = enable
		requireLayerInterface().enableHeading(enable)
		val isLifecycleAtLeastStarted = lifecycle?.currentState?.isAtLeast(Lifecycle.State.STARTED) ?: false
		if (enable && isLifecycleAtLeastStarted) {
			compassProvider?.registerCompassUpdateListener(this)
		} else {
			compassProvider?.unregisterCompassUpdateListener(this)
		}
	}

	fun setFollowInitializeZoom(zoom: Float?) {
		requireLayerInterface().setFollowInitializeZoom(zoom)
	}

	fun changeProviderType(context: Context, gpsProviderType: GpsProviderType) {
		locationProvider?.unregisterLocationUpdateListener(this)
		val locationProvider = gpsProviderType.getProvider(context)
		locationProvider.registerLocationUpdateListener(this)
		this.locationProvider = locationProvider
	}

	fun setOnModeChangedListener(listener: ((GpsMode) -> Unit)?) {
		modeChangedListener = listener
	}

	fun setOnClickListener(listener: ((Coord) -> Unit)?) {
		onClickListener = listener
	}

	override fun onLocationUpdate(newLocation: Location) {
		val coord = Coord(CoordinateSystemIdentifiers.EPSG4326(), newLocation.longitude, newLocation.latitude, newLocation.altitude)
		val accuracy = newLocation.accuracy.toDouble()
		requireLayerInterface().updatePosition(coord, accuracy)
	}

	override fun onCompassUpdate(degrees: Float) {
		requireLayerInterface().updateHeading(degrees)
	}

	override fun modeDidChange(mode: GpsMode) {
		modeChangedListener?.invoke(mode)
	}

	override fun onPointClick(coordinate: Coord) {
		onClickListener?.invoke(coordinate)
	}

	private fun requireLayerInterface(): GpsLayerInterface =
		layerInterface ?: throw IllegalStateException("GpsLayer is already destroyed!")
	private fun requireCompassProvider(): CompassProvider =
		compassProvider ?: throw IllegalStateException("GpsLayer is already destroyed!")
}