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

    void
    _convertCppExceptionsToJavaExceptions(JNIEnv *env, const std::function<void()> &pFunction) {
        try {
            pFunction();
        } catch (const std::exception &e) {
            env->ThrowNew(env->FindClass("java/lang/RuntimeException"), e.what());
        }
    }

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


template jobject protectByMutexWithReturn<jobject>(const std::function<jobject()> &pFunction);

template jstring protectByMutexWithReturn<jstring>(const std::function<jstring()> &pFunction);

template jint protectByMutexWithReturn<jint>(const std::function<jint()> &pFunction);

template jintArray protectByMutexWithReturn<jintArray>(const std::function<jintArray()> &pFunction);


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
Java_com_contextualplanner_ContextualPlannerKt_lookForAnActionToDo(
        JNIEnv *env, jclass /*clazz*/, jint idProblem, jint idDomain) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto *domainPtr = idToDomainUnsafe(idDomain);
            auto *problemPtr = idToProblemUnsafe(idProblem);
            if (domainPtr != nullptr && problemPtr != nullptr) {
                std::map<std::string, std::string> parameters;
                auto action = cp::lookForAnActionToDo(parameters, *problemPtr, domainPtr->domain,
                                                      &problemPtr->historical);
                return env->NewStringUTF(action.c_str());
            }
            return env->NewStringUTF("");
        });
    }, nullptr);
}



extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_ContextualPlannerKt_notifyActionDone(
        JNIEnv *env, jclass /*clazz*/, jstring jActionId, jint idProblem, jint idDomain) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        return protectByMutex([&]() {
            auto *domainPtr = idToDomainUnsafe(idDomain);
            auto *problemPtr = idToProblemUnsafe(idProblem);
            if (domainPtr != nullptr && problemPtr != nullptr) {
                auto actionId = cvtoString(env, jActionId);
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


extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_contextualplanner_ContextualPlannerKt_parseCommand(
        JNIEnv *env, jclass /*clazz*/, jstring jcommand) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jobjectArray>(env, [&]() {

        std::string command = cvtoString(env, jcommand);
        auto fact = cp::Fact::fromStr(command);

        jobjectArray result;
        result = (jobjectArray) env->NewObjectArray(fact.parameters.size() + 1,
                                                    env->FindClass("java/lang/String"),
                                                    env->NewStringUTF(""));

        jsize arrayElt = 0;
        env->SetObjectArrayElement(result, arrayElt++,
                                   env->NewStringUTF(fact.name.c_str()));
        for (const auto& currParam : fact.parameters)
            env->SetObjectArrayElement(result, arrayElt++,
                                       env->NewStringUTF(currParam.toStr().c_str()));
        return result;
    }, nullptr);
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_ContextualPlannerKt_replaceVariables(
        JNIEnv *env, jclass /*clazz*/, jstring jStr, jint idProblem) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto *problemPtr = idToProblemUnsafe(idProblem);
            auto str = cvtoString(env, jStr);
            if (problemPtr != nullptr) {
                replaceVariables(str, *problemPtr);
            }
            return env->NewStringUTF(str.c_str());
        });
    }, nullptr);
}