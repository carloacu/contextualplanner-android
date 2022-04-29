#ifndef CONTEXTUALPLANNER_CONTEXTUALPLANNER_JNI_HPP
#define CONTEXTUALPLANNER_CONTEXTUALPLANNER_JNI_HPP

#include <jni.h>
#include <map>
#include <functional>
#include <mutex>

static std::mutex _jniPlannerMutex;


template<typename T>
jint findMissingKey(const std::map<jint, T> &pIdToObj) {
    jint currentId = 0;
    for (const auto &currMemory : pIdToObj) {
        int nextId = currentId + 1;
        if (currMemory.first > nextId)
            return nextId;
        currentId = currMemory.first;
    }
    return currentId + 1;
}

/**
 * Convert C++ exception to java exception and as java exceptions don't stop the flow,
 * we need to return an object that corresponds to what the JNI wants. (that is why we have a default value)
 */
template<typename T>
T convertCppExceptionsToJavaExceptionsAndReturnTheResult(
        JNIEnv *env,
        const std::function<T()> &pFunction,
        const T &pDefaultReturn) {
    try {
        return pFunction();
    } catch (const std::exception &e) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), e.what());
    }
    return pDefaultReturn;
}

static void
convertCppExceptionsToJavaExceptions(JNIEnv *env, const std::function<void()> &pFunction) {
    try {
        pFunction();
    } catch (const std::exception &e) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), e.what());
    }
}

template<typename T>
T protectByMutexWithReturn(const std::function<T()> &pFunction) {
    std::lock_guard<std::mutex> lock(_jniPlannerMutex);
    return pFunction();
}

static void protectByMutex(const std::function<void()> &pFunction) {
    std::lock_guard<std::mutex> lock(_jniPlannerMutex);
    pFunction();
}


#endif // CONTEXTUALPLANNER_CONTEXTUALPLANNER_JNI_HPP
