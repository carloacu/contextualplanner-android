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
Java_com_contextualplanner_Problem_00024Companion_newProblem(
        JNIEnv *env, jobject /*object*/) {
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
Java_com_contextualplanner_Problem_disposeImplementation(
        JNIEnv *env, jobject object) {
    protectByMutex([&]() {
        _idToRobotPlannerProblem.erase(toId(env, object));
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_Problem_pushFrontGoal(
        JNIEnv *env, jobject object, jobject jGoal) {
    protectByMutex([&]() {
        int priority = 10;
        auto goal = toGoal(env, jGoal, &priority);
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            problemPtr->pushFrontGoal(goal, now, priority);
        }
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_Problem_pushBackGoal(
        JNIEnv *env, jobject object, jobject jGoal) {
    protectByMutex([&]() {
        int priority = 10;
        auto goal = toGoal(env, jGoal, &priority);
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            problemPtr->pushBackGoal(goal, now, priority);
        }
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_Problem_addFact(
        JNIEnv *env, jobject object, jstring jFact) {
    protectByMutex([&]() {
        auto fact = toString(env, jFact);
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr)
            problemPtr->addFact(fact);
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_Problem_removeFact(
        JNIEnv *env, jobject object, jstring jFact) {
    protectByMutex([&]() {
        auto fact = toString(env, jFact);
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr)
            problemPtr->removeFact(fact);
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_Problem_modifyFacts(
        JNIEnv *env, jobject object, jstring jFacts) {
    protectByMutex([&]() {
        static const char sep = ',';
        auto facts = cp::SetOfFacts::fromStr(toString(env, jFacts), sep);
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr)
            problemPtr->modifyFacts(facts);
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_Problem_addGoals(
        JNIEnv *env, jobject object, jobjectArray jGoals) {
    protectByMutex([&]() {
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            problemPtr->addGoals(toGoals(env, jGoals), now);
        }
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_Problem_removeFirstGoalsThatAreAlreadySatisfied(
        JNIEnv *env, jobject object) {
    protectByMutex([&]() {
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr)
            problemPtr->removeFirstGoalsThatAreAlreadySatisfied();
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_Problem_addVariableToValue(
        JNIEnv *env, jobject object, jstring jVariableName, jstring jVariableValue) {
    protectByMutex([&]() {
        auto variableName = toString(env, jVariableName);
        auto variableValue = toString(env, jVariableValue);
        std::map<std::string, std::string> variablesToValue;
        variablesToValue.emplace(variableName, variableValue);
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr)
            problemPtr->addVariablesToValue(variablesToValue);
    });
}



extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_Problem_printGoals(
        JNIEnv *env, jobject object, int goalNameMaxSize) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto* problemPtr = idToProblemUnsafe(toId(env, object));
            if (problemPtr != nullptr)
            {
                auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
                return env->NewStringUTF(problemPtr->printGoals(goalNameMaxSize, now).c_str());
            }
            return env->NewStringUTF("");
        });
    }, nullptr);
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_contextualplanner_Problem_printFacts(
        JNIEnv *env, jobject object) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jstring>(env, [&]() {
        return protectByMutexWithReturn<jstring>([&]() {
            auto* problemPtr = idToProblemUnsafe(toId(env, object));
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

