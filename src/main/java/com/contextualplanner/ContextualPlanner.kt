package com.contextualplanner

import java.io.ByteArrayOutputStream
import java.io.IOException
import java.io.InputStream


/**
 * Function to load the C++ library.
 * It's internal because the calls are hidden in the constructor of the objects.
 */
var isLoaded = false
fun ensureInitialized() = synchronized(isLoaded) {
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
            disposeImplementation(id)
            isDisposed = true
        }
    }

    abstract fun disposeImplementation(id: Int)
}

fun readTextFile(inputStream: InputStream): String {
    val outputStream = ByteArrayOutputStream()
    val buf = ByteArray(1024)
    var len: Int
    try {
        while (inputStream.read(buf).also { len = it } != -1) {
            outputStream.write(buf, 0, len)
        }
        outputStream.close()
        inputStream.close()
    } catch (e: IOException) {
    }
    return outputStream.toString()
}

fun replaceVariables(str: String, problem: Problem): String {
    return replaceVariables(str, problem.id)
}

fun lookForAnActionToDo(problem: Problem, domain: Domain): String {
    return lookForAnActionToDo(problem.id, domain.id) ?: ""
}

fun notifyActionDone(action: Action, problem: Problem, domain: Domain) {
    notifyActionDone(action.id, problem.id, domain.id)
}


fun notifyActionDone(actionId: String, problem: Problem, domain: Domain) {
    notifyActionDone(actionId, problem.id, domain.id)
}


external fun parseCommand(command: String): Array<String>



private external fun replaceVariables(str: String, idProblem: Int): String

private external fun lookForAnActionToDo(idProblem: Int, idDomain: Int): String?

private external fun notifyActionDone(idAction: String, idProblem: Int, idDomain: Int)