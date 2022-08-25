#include "../contextualplanner-jni.hpp"
#include <contextualplanner/util/print.hpp>
#include "../androidlog.hpp"
#include "../jobjectsconversions.hpp"
#include "../types/problem-jni.hpp"



extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_util_PrintKt_printGoals(
        JNIEnv *env, jclass /*clazz*/, int goalNameMaxSize, jobject problemObject) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto* problemPtr = idToProblemUnsafe(toId(env, problemObject));
            if (problemPtr != nullptr)
            {
                auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
                return env->NewStringUTF(cp::printGoals(goalNameMaxSize, *problemPtr, now).c_str());
            }
            return env->NewStringUTF("");
        });
    }, nullptr);
}



extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_util_PrintKt_printFacts(
        JNIEnv *env, jclass /*clazz*/, jobject problemObject) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto* problemPtr = idToProblemUnsafe(toId(env, problemObject));
            if (problemPtr != nullptr)
            {
                std::string res;
                auto& facts = problemPtr->facts();
                for (auto& fact : facts) {
                    if (!res.empty())
                        res += ", ";
                    res += fact.toStr();
                }
                return env->NewStringUTF(res.c_str());
            }
            return env->NewStringUTF("");
        });
    }, nullptr);
}