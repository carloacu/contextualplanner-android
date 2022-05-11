#include "contextualplanner-jni.hpp"
#include <regex>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <memory>
#include <contextualplanner/contextualplanner.hpp>
#include "androidlog.hpp"
#include "domain-jni.hpp"
#include "problem-jni.hpp"



namespace {

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
}


template jstring protectByMutexWithReturn<jstring>(const std::function<jstring()> &pFunction);

template jint protectByMutexWithReturn<jint>(const std::function<jint()> &pFunction);


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
#ifdef COUT_TO_ANDROID_LOG
    // Also initialize the forwarding of logs to Android.
    std::cout.rdbuf(new forward_to_android);
    // TODO: write this in a way that does not leak and does not conflict with other libraries
#endif // COUT_TO_ANDROID_LOG
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_ContextualPlannerKt_replaceVariables(
        JNIEnv *env, jclass /*clazz*/, jstring jStr, jobject problemObject) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto *problemPtr = idToProblemUnsafe(toId(env, problemObject));
            auto str = toString(env, jStr);
            if (problemPtr != nullptr) {
                replaceVariables(str, *problemPtr);
            }
            return env->NewStringUTF(str.c_str());
        });
    }, nullptr);
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_ContextualPlannerKt_lookForAnActionToDo(
        JNIEnv *env, jclass /*clazz*/, jobject problemObject, jobject domainObject) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto *domainPtr = idToDomainUnsafe(toId(env, domainObject));
            auto *problemPtr = idToProblemUnsafe(toId(env, problemObject));
            if (domainPtr != nullptr && problemPtr != nullptr) {
                std::map<std::string, std::string> parameters;
                auto action = cp::lookForAnActionToDo(parameters, *problemPtr, domainPtr->domain,
                                                      &problemPtr->historical);
                return env->NewStringUTF(action.c_str());
            }
            return env->NewStringUTF("");
        });
    }, env->NewStringUTF(""));
}



extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_ContextualPlannerKt_notifyActionDone(
        JNIEnv *env, jclass /*clazz*/, jstring jActionId, jobject problemObject, jobject domainObject) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        return protectByMutex([&]() {
            auto *domainPtr = idToDomainUnsafe(toId(env, domainObject));
            auto *problemPtr = idToProblemUnsafe(toId(env, problemObject));
            if (domainPtr != nullptr && problemPtr != nullptr) {
                auto actionId = toString(env, jActionId);
                auto itAction = domainPtr->idToPlannerActions.find(actionId);
                if (itAction != domainPtr->idToPlannerActions.end())
                {
                    std::map<std::string, std::string> parameters;
                    problemPtr->notifyActionDone(actionId, parameters, itAction->second.effect, &itAction->second.goalsToAdd);
                }
            }
        });
    });
}

