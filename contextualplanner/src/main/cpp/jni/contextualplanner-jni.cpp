#include "contextualplanner-jni.hpp"
#include <regex>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <memory>
#include <contextualplanner/contextualplanner.hpp>
#include <contextualplanner/util/replacevariables.hpp>
#include "androidlog.hpp"
#include "types/domain-jni.hpp"
#include "types/problem-jni.hpp"


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
JNIEXPORT jobject JNICALL
Java_com_contextualplanner_ContextualPlannerKt_lookForAnActionToDo(
        JNIEnv *env, jclass /*clazz*/, jobject problemObject, jobject domainObject) {
    jclass actionAndGoalClass = env->FindClass(
            "com/contextualplanner/types/ActionAndGoal");
    jmethodID actionAndGoalClassConstructor =
            env->GetMethodID(actionAndGoalClass, "<init>",
                             "(Ljava/lang/String;Lcom/contextualplanner/types/Goal;)V");

    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jobject>(env, [&]() {
        return protectByMutexWithReturn<jobject>([&]() {
            auto *domainPtr = idToDomainUnsafe(toId(env, domainObject));
            auto *problemPtr = idToProblemUnsafe(toId(env, problemObject));
            if (domainPtr != nullptr && problemPtr != nullptr) {
                std::map<std::string, std::string> parameters;
                auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
                const cp::Goal* goal = nullptr;
                int priority = 0;
                auto action = cp::lookForAnActionToDo(parameters, *problemPtr, domainPtr->domain,
                                                      now, &goal, &priority, &problemPtr->historical);

                if (goal != nullptr)
                    return env->NewObject(actionAndGoalClass, actionAndGoalClassConstructor,
                                          env->NewStringUTF(action.c_str()),
                                          newJavaGoal(env, priority, *goal));
            }
            return env->NewObject(actionAndGoalClass, actionAndGoalClassConstructor,
                                  env->NewStringUTF(""),
                                  newJavaGoal(env, 0, cp::Goal("goal_name", -1, "")));
        });
    }, env->NewObject(actionAndGoalClass, actionAndGoalClassConstructor,
                      env->NewStringUTF(""),
                      newJavaGoal(env, 0, cp::Goal("goal_name", -1, ""))
                      ));
}

