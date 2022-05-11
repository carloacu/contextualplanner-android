package com.contextualplanner

import java.io.ByteArrayOutputStream
import java.io.IOException
import java.io.InputStream


/**
 * Function to load the C++ library.
 * It's internal because the calls are hidden in the constructor of the objects.
 */
var isLoaded = false
internal fun ensureInitialized() = synchronized(isLoaded) {
    if (!isLoaded) {
        System.loadLibrary("contextualplanner-jni")
        isLoaded = true
    }
}

/**
 * A disposable class that store an id and do the dispose code only at the first call.
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

    abstract fun disposeImplementation()
}

external fun replaceVariables(str: String, problem: Problem): String

external fun lookForAnActionToDo(problem: Problem, domain: Domain): String

fun notifyActionDone(action: Action, problem: Problem, domain: Domain) {
    notifyActionDone(action.id, problem, domain)
}

external fun notifyActionDone(idAction: String, problem: Problem, domain: Domain)


