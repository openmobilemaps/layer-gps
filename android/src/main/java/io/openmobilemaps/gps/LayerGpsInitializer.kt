package io.openmobilemaps.gps

object LayerGpsInitializer {
	fun initialize() {
		System.loadLibrary("layergps")
	}
}