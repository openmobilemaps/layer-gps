package io.openmobilemaps.gps.style

import android.content.Context
import android.graphics.Bitmap
import android.graphics.drawable.Drawable
import androidx.core.content.ContextCompat
import io.openmobilemaps.gps.R
import io.openmobilemaps.gps.shared.gps.GpsStyleInfoInterface
import io.openmobilemaps.mapscore.graphics.BitmapTextureHolder
import io.openmobilemaps.mapscore.shared.graphics.common.Color

object GpsStyleInfoFactory {

    fun createDefaultStyle(context: Context): GpsStyleInfoInterface = GpsStyleInfoInterface.create(
        ContextCompat.getDrawable(context, R.drawable.ic_gps_point)?.let { BitmapTextureHolder(it) },
        ContextCompat.getDrawable(context, R.drawable.ic_gps_direction)?.let { BitmapTextureHolder(it) },
        null,
        Color(112 / 255f, 173 / 255f, 204 / 255f, 0.4f)
    )

    fun createGpsStyle(pointDrawable: Drawable?, headingDrawable: Drawable?, accuracyColor: Color): GpsStyleInfoInterface = GpsStyleInfoInterface.create(
        pointDrawable?.let { BitmapTextureHolder(it) },
        headingDrawable?.let { BitmapTextureHolder(it) },
        null,
        accuracyColor
    )

    fun createGpsStyle(pointBitmap: Bitmap?, headingBitmap: Bitmap?, accuracyColor: Color): GpsStyleInfoInterface = GpsStyleInfoInterface.create(
        pointBitmap?.let { BitmapTextureHolder(it) },
        headingBitmap?.let { BitmapTextureHolder(it) },
        null,
        accuracyColor
    )

}
