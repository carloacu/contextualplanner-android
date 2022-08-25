package com.contextualplanner.util.detail


/**
 * @brief Function to load the C++ library.<br/>
 * It's internal because the calls are hidden in the constructor of the objects.
 */
var isLoaded = false
internal fun ensureInitialized() = synchronized(isLoaded) {
    if (!isLoaded) {
        System.loadLibrary("contextualplanner-jni")
        isLoaded = true
    }
}

