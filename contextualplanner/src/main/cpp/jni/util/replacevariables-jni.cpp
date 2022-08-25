#include "../contextualplanner-jni.hpp"
#include <regex>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <memory>
#include <contextualplanner/contextualplanner.hpp>
#include <contextualplanner/util/replacevariables.hpp>
#include "../androidlog.hpp"
#include "../types/domain-jni.hpp"
#include "../types/problem-jni.hpp"



extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_util_ReplaceVariablesKt_replaceVariables(
        JNIEnv *env, jclass /*clazz*/, jstring jStr, jobject problemObject) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto *problemPtr = idToProblemUnsafe(toId(env, problemObject));
            auto str = toString(env, jStr);
            if (problemPtr != nullptr) {
                cp::replaceVariables(str, problemPtr->variablesToValue());
            }
            return env->NewStringUTF(str.c_str());
        });
    }, nullptr);
}

