// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from layer_gps.djinni

package io.openmobilemaps.gps.shared.gps

import java.util.concurrent.atomic.AtomicBoolean

abstract class GpsLayerInterface {

    abstract fun setCallbackHandler(handler: GpsLayerCallbackInterface)

    abstract fun setMode(mode: GpsMode)

    abstract fun getMode(): GpsMode

    abstract fun enableHeading(enable: Boolean)

    abstract fun updatePosition(position: io.openmobilemaps.mapscore.shared.map.coordinates.Coord, horizontalAccuracyM: Double)

    abstract fun updateHeading(angleHeading: Float)

    abstract fun asLayerInterface(): io.openmobilemaps.mapscore.shared.map.LayerInterface

    companion object {
        @JvmStatic
        fun create(styleInfo: GpsStyleInfo): GpsLayerInterface {
            return CppProxy.create(styleInfo)
        }
    }

    private class CppProxy : GpsLayerInterface {
        private val nativeRef: Long
        private val destroyed: AtomicBoolean = AtomicBoolean(false)

        private constructor(nativeRef: Long) {
            if (nativeRef == 0L) error("nativeRef is zero")
            this.nativeRef = nativeRef
        }

        private external fun nativeDestroy(nativeRef: Long)
        fun _djinni_private_destroy() {
            val destroyed = this.destroyed.getAndSet(true)
            if (!destroyed) nativeDestroy(this.nativeRef)
        }
        protected fun finalize() {
            _djinni_private_destroy()
        }

        override fun setCallbackHandler(handler: GpsLayerCallbackInterface) {
            assert(!this.destroyed.get()) { error("trying to use a destroyed object") }
            native_setCallbackHandler(this.nativeRef, handler)
        }
        private external fun native_setCallbackHandler(_nativeRef: Long, handler: GpsLayerCallbackInterface)

        override fun setMode(mode: GpsMode) {
            assert(!this.destroyed.get()) { error("trying to use a destroyed object") }
            native_setMode(this.nativeRef, mode)
        }
        private external fun native_setMode(_nativeRef: Long, mode: GpsMode)

        override fun getMode(): GpsMode {
            assert(!this.destroyed.get()) { error("trying to use a destroyed object") }
            return native_getMode(this.nativeRef)
        }
        private external fun native_getMode(_nativeRef: Long): GpsMode

        override fun enableHeading(enable: Boolean) {
            assert(!this.destroyed.get()) { error("trying to use a destroyed object") }
            native_enableHeading(this.nativeRef, enable)
        }
        private external fun native_enableHeading(_nativeRef: Long, enable: Boolean)

        override fun updatePosition(position: io.openmobilemaps.mapscore.shared.map.coordinates.Coord, horizontalAccuracyM: Double) {
            assert(!this.destroyed.get()) { error("trying to use a destroyed object") }
            native_updatePosition(this.nativeRef, position, horizontalAccuracyM)
        }
        private external fun native_updatePosition(_nativeRef: Long, position: io.openmobilemaps.mapscore.shared.map.coordinates.Coord, horizontalAccuracyM: Double)

        override fun updateHeading(angleHeading: Float) {
            assert(!this.destroyed.get()) { error("trying to use a destroyed object") }
            native_updateHeading(this.nativeRef, angleHeading)
        }
        private external fun native_updateHeading(_nativeRef: Long, angleHeading: Float)

        override fun asLayerInterface(): io.openmobilemaps.mapscore.shared.map.LayerInterface {
            assert(!this.destroyed.get()) { error("trying to use a destroyed object") }
            return native_asLayerInterface(this.nativeRef)
        }
        private external fun native_asLayerInterface(_nativeRef: Long): io.openmobilemaps.mapscore.shared.map.LayerInterface

        companion object {
            @JvmStatic
            external fun create(styleInfo: GpsStyleInfo): GpsLayerInterface
        }
    }
}
