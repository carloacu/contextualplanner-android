#include "domain-jni.hpp"
#include "../contextualplanner-jni.hpp"
#include "../jobjectsconversions.hpp"


namespace {
    std::map<jint, cp::Domain> _idToRobotDomain;
}

const cp::Domain* idToDomainUnsafe(jint id) {
    auto it = _idToRobotDomain.find(id);
    if (it != _idToRobotDomain.end())
        return &it->second;
    return nullptr;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_contextualplanner_types_Domain_00024Companion_newDomain(
        JNIEnv *env, jobject /*object*/, jobjectArray jactions) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jint>(env, [&]() {
        return protectByMutexWithReturn<jint>([&]() {

            std::map<cp::ActionId, cp::Action> idToActions;
            int size = env->GetArrayLength(jactions);
            for (int i = 0; i < size; ++i) {
                shared_jobject action(env, env->GetObjectArrayElement(jactions, i));
                auto actionWithId = toActionWithId(env, action.get());
                idToActions.emplace(actionWithId.id, actionWithId.action);
            }

            jint newDomainId = findMissingKey(_idToRobotDomain);
            _idToRobotDomain.emplace(newDomainId, idToActions);
            return newDomainId;
        });
    }, -1);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Domain_addAction(
        JNIEnv *env, jobject object, jobject jaction) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto it = _idToRobotDomain.find(toId(env, object));
            if (it == _idToRobotDomain.end())
                return;
            auto actionWithId = toActionWithId(env, jaction);
            it->second.addAction(actionWithId.id, actionWithId.action);
        });
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Domain_removeAction(
        JNIEnv *env, jobject object, jstring jactionStr) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto it = _idToRobotDomain.find(toId(env, object));
            if (it == _idToRobotDomain.end())
                return;
            auto actionId = toString(env, jactionStr);
            it->second.removeAction(actionId);
        });
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Domain_disposeImplementation(
        JNIEnv *env, jobject object) {
    protectByMutex([&]() {
        _idToRobotDomain.erase(toId(env, object));
    });
}


