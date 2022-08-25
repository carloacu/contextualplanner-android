#include "domain-jni.hpp"
#include "../contextualplanner-jni.hpp"
#include "../jobjectsconversions.hpp"


namespace {
    std::map<jint, PlannerDomain> _idToRobotPlannerDomain;

    cp::Action plannerActionToAction(const PlannerAction& pPlannerAction) {
        cp::SetOfFacts effect = pPlannerAction.effect;
        effect.add(pPlannerAction.potentialEffect);
        return cp::Action(pPlannerAction.precondition, effect, pPlannerAction.preferInContext);
    }
}

const PlannerDomain* idToDomainUnsafe(jint id) {
    auto it = _idToRobotPlannerDomain.find(id);
    if (it != _idToRobotPlannerDomain.end())
        return &it->second;
    return nullptr;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_contextualplanner_types_Domain_00024Companion_newDomain(
        JNIEnv *env, jobject /*object*/, jobjectArray jactions) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jint>(env, [&]() {
        return protectByMutexWithReturn<jint>([&]() {

            std::map<cp::ActionId, PlannerAction> idToPlannerActions;
            std::map<cp::ActionId, cp::Action> idToActions;

            int size = env->GetArrayLength(jactions);
            for (int i = 0; i < size; ++i) {
                shared_jobject action(env, env->GetObjectArrayElement(jactions, i));
                auto plannerAction = toPlannerAction(env, action.get());
                idToActions.emplace(plannerAction.id, plannerActionToAction(plannerAction));
                idToPlannerActions.emplace(plannerAction.id, plannerAction);
            }

            jint newPlannerDomainId = findMissingKey(_idToRobotPlannerDomain);
            _idToRobotPlannerDomain.emplace(newPlannerDomainId, PlannerDomain(std::move(idToPlannerActions), idToActions));
            return newPlannerDomainId;
        });
    }, -1);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Domain_addAction(
        JNIEnv *env, jobject object, jobject jaction) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto it = _idToRobotPlannerDomain.find(toId(env, object));
            if (it == _idToRobotPlannerDomain.end())
                return;
            auto plannerAction = toPlannerAction(env, jaction);
            it->second.idToPlannerActions.emplace(plannerAction.id, plannerAction);
            auto action = plannerActionToAction(plannerAction);
            it->second.domain.addAction(plannerAction.id, action);
        });
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Domain_removeAction(
        JNIEnv *env, jobject object, jstring jactionStr) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto it = _idToRobotPlannerDomain.find(toId(env, object));
            if (it == _idToRobotPlannerDomain.end())
                return;
            auto actionId = toString(env, jactionStr);
            it->second.idToPlannerActions.erase(actionId);
            it->second.domain.removeAction(actionId);
        });
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Domain_disposeImplementation(
        JNIEnv *env, jobject object) {
    protectByMutex([&]() {
        _idToRobotPlannerDomain.erase(toId(env, object));
    });
}


