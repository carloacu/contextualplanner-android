#include <jni.h>
#include "../androidlog.hpp"
#include "../jobjectsconversions.hpp"
#include <contextualplanner/types/fact.hpp>


extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_util_FactKt_getPunctualFactPrefixCpp(
        JNIEnv *env, jclass /*clazz*/) {
    return env->NewStringUTF(cp::Fact::punctualPrefix.c_str());
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_util_FactKt_setPunctualFactPrefixCpp(
        JNIEnv *env, jclass /*clazz*/, jstring jPunctualPrefix) {
    auto newPunctualPrefix = toString(env, jPunctualPrefix);
    cp::Fact::punctualPrefix = newPunctualPrefix;
}
