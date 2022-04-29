#include "domain-jni.hpp"
#include <regex>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <memory>
#include "contextualplanner-jni.hpp"
#include "jobjectsconversions.hpp"


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
Java_com_contextualplanner_DomainKt_newDomain(
        JNIEnv *env, jclass /*clazz*/, jobjectArray jactions) {
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
Java_com_contextualplanner_DomainKt_addAction(
        JNIEnv *env, jclass /*clazz*/, jint domainId, jobject jaction) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto it = _idToRobotPlannerDomain.find(domainId);
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
Java_com_contextualplanner_DomainKt_removeAction(
        JNIEnv *env, jclass /*clazz*/, jint domainId, jstring jactionStr) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto it = _idToRobotPlannerDomain.find(domainId);
            if (it == _idToRobotPlannerDomain.end())
                return;
            auto actionId = cvtoString(env, jactionStr);
            it->second.idToPlannerActions.erase(actionId);
            it->second.domain.removeAction(actionId);
        });
    });
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_DomainKt_printActions(
        JNIEnv *env, jclass /*clazz*/, jint domainId) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto it = _idToRobotPlannerDomain.find(domainId);
            if (it == _idToRobotPlannerDomain.end())
                return env->NewStringUTF("");
            std::string res;
            for (auto& action : it->second.domain.actions())
            {
                if (action.second.effects.empty())
                    res += action.first;
                else
                    res += action.first + " -> " + action.second.effects.toStr(",");
                res += "\n";
            }
            return env->NewStringUTF(res.c_str());
        });
    }, nullptr);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_DomainKt_deleteDomain(
        JNIEnv *env, jclass /*clazz*/, jint id) {
    protectByMutex([&]() {
        _idToRobotPlannerDomain.erase(id);
    });
}


