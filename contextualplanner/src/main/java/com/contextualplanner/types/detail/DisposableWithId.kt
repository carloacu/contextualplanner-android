package com.contextualplanner.types.detail

/**
 * @brief A disposable class that store an id and do the dispose code only at the first call.
 */
abstract class DisposableWithId(
    val id: Int
) {
    var isDisposed = false
        private set

    fun dispose() {
        if (!isDisposed) {
            disposeImplementation()
            isDisposed = true
        }
    }

    protected abstract fun disposeImplementation()
}