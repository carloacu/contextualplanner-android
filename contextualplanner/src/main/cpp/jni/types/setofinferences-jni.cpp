#include "../androidlog.hpp"
#include "../contextualplanner-jni.hpp"
#include "../jobjectsconversions.hpp"
#include "setofinferences-jni.hpp"
#include <contextualplanner/types/setofinferences.hpp>
#include <contextualplanner/util/print.hpp>

namespace {
    std::map<jint, std::shared_ptr<cp::SetOfInferences>> _idToSetOfInferences;
}

std::shared_ptr<cp::SetOfInferences> idToSetOfInferencesUnsafe(jint id) {
    auto it = _idToSetOfInferences.find(id);
    if (it != _idToSetOfInferences.end())
        return it->second;
    return nullptr;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_contextualplanner_types_SetOfInferences_00024Companion_newSetOfInferences(
        JNIEnv *env, jobject /*object*/) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jint>(env, [&]() {
        return protectByMutexWithReturn<jint>([&]() {
            jint newSetOfInferencesId = findMissingKey(_idToSetOfInferences);
            _idToSetOfInferences.emplace(newSetOfInferencesId, std::make_shared<cp::SetOfInferences>());
            return newSetOfInferencesId;
        });
    }, -1);
}



extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_SetOfInferences_addInference(
        JNIEnv *env, jobject object, jobject jinference) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto setOfInferencesPtr = idToSetOfInferencesUnsafe(toId(env, object));
            if (setOfInferencesPtr)
            {
                std::string inferenceId;
                auto inference = toInference(env, jinference, inferenceId);
                setOfInferencesPtr->addInference(inferenceId, inference);
            }
        });
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_SetOfInferences_removeInference(
        JNIEnv *env, jobject object, jstring jinferenceId) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto setOfInferencesPtr = idToSetOfInferencesUnsafe(toId(env, object));
            if (setOfInferencesPtr)
            {
                auto inferenceId = toString(env, jinferenceId);
                setOfInferencesPtr->removeInference(inferenceId);
            }
        });
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_SetOfInferences_disposeImplementation(
        JNIEnv *env, jobject object) {
    protectByMutex([&]() {
        auto it = _idToSetOfInferences.find(toId(env, object));
        if (it != _idToSetOfInferences.end())
            _idToSetOfInferences.erase(it);
    });
}