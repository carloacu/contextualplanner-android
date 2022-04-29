#include <regex>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <memory>
#include "androidlog.hpp"
#include "contextualplanner-jni.hpp"
#include "jobjectsconversions.hpp"


namespace {
    std::map<jint, cp::Problem> _idToRobotPlannerProblem;
}

cp::Problem* idToProblemUnsafe(jint id) {
    auto it = _idToRobotPlannerProblem.find(id);
    if (it != _idToRobotPlannerProblem.end())
        return &it->second;
    return nullptr;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_contextualplanner_ProblemKt_newProblem(
        JNIEnv *env, jclass /*clazz*/) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jint>(env, [&]() {
        return protectByMutexWithReturn<jint>([&]() {
            jint newLocalMemory = findMissingKey(_idToRobotPlannerProblem);
            auto& newMemTracker = _idToRobotPlannerProblem[newLocalMemory];
            return newLocalMemory;
        });
    }, -1);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_ProblemKt_deleteProblem(
        JNIEnv *env, jclass /*clazz*/, jint stateId) {
    protectByMutex([&]() {
        _idToRobotPlannerProblem.erase(stateId);
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_ProblemKt_pushFrontGoal(
        JNIEnv *env, jclass /*clazz*/, jint id, jstring jGoal, jstring jGoalGroupId) {
    protectByMutex([&]() {
        auto goal = cvtoString(env, jGoal);
        auto goalGroupId = cvtoString(env, jGoalGroupId);
        auto* problemPtr = idToProblemUnsafe(id);
        if (problemPtr != nullptr)
            problemPtr->pushFrontGoal(cp::Goal(goal, goalGroupId));
    });
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_ProblemKt_printGoals(
        JNIEnv *env, jclass /*clazz*/, jint id) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto* problemPtr = idToProblemUnsafe(id);
            if (problemPtr != nullptr)
            {
                std::string res;
                auto& goals = problemPtr->goals();
                for (auto& goal : goals) {
                    res += goal.toStr();
                    auto& goalGroupId = goal.getGoalGroupId();
                    if (!goalGroupId.empty())
                        res += " groupId: " + goalGroupId;
                    res += "\n";
                }
                return env->NewStringUTF(res.c_str());
            }
            return env->NewStringUTF("");
        });
    }, nullptr);
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_ProblemKt_printFacts(
        JNIEnv *env, jclass /*clazz*/, jint id) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto* problemPtr = idToProblemUnsafe(id);
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


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_ProblemKt_pushBackGoal(
        JNIEnv *env, jclass /*clazz*/, jint id, jstring jGoal) {
    protectByMutex([&]() {
        auto goal = cvtoString(env, jGoal);
        auto* problemPtr = idToProblemUnsafe(id);
        if (problemPtr != nullptr)
            problemPtr->pushBackGoal(goal);
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_ProblemKt_addFact(
        JNIEnv *env, jclass /*clazz*/, jint id, jstring jFact) {
    protectByMutex([&]() {
        auto fact = cvtoString(env, jFact);
        auto* problemPtr = idToProblemUnsafe(id);
        if (problemPtr != nullptr)
            problemPtr->addFact(fact);
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_ProblemKt_removeFact(
        JNIEnv *env, jclass /*clazz*/, jint id, jstring jFact) {
    protectByMutex([&]() {
        auto fact = cvtoString(env, jFact);
        auto* problemPtr = idToProblemUnsafe(id);
        if (problemPtr != nullptr)
            problemPtr->removeFact(fact);
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_ProblemKt_modifyFacts(
        JNIEnv *env, jclass /*clazz*/, jint id, jstring jFacts) {
    protectByMutex([&]() {
        static const char sep = ',';
        auto facts = cp::SetOfFacts::fromStr(cvtoString(env, jFacts), sep);
        auto* problemPtr = idToProblemUnsafe(id);
        if (problemPtr != nullptr)
            problemPtr->modifyFacts(facts);
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_ProblemKt_addGoals(
        JNIEnv *env, jclass /*clazz*/, jint id, jobjectArray jGoals) {
    protectByMutex([&]() {
        auto* problemPtr = idToProblemUnsafe(id);
        if (problemPtr != nullptr)
            problemPtr->addGoals(toGoals(env, jGoals));
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_ProblemKt_removeFirstGoalsThatAreAlreadySatisfied(
        JNIEnv *env, jclass /*clazz*/, jint id) {
    protectByMutex([&]() {
        auto* problemPtr = idToProblemUnsafe(id);
        if (problemPtr != nullptr)
            problemPtr->removeFirstGoalsThatAreAlreadySatisfied();
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_ProblemKt_addVariableToValue(
        JNIEnv *env, jclass /*clazz*/, jint id, jstring jVariableName, jstring jVariableValue) {
    protectByMutex([&]() {
        auto variableName = cvtoString(env, jVariableName);
        auto variableValue = cvtoString(env, jVariableValue);
        std::map<std::string, std::string> variablesToValue;
        variablesToValue.emplace(variableName, variableValue);
        auto* problemPtr = idToProblemUnsafe(id);
        if (problemPtr != nullptr)
            problemPtr->addVariablesToValue(variablesToValue);
    });
}
