#include "../androidlog.hpp"
#include "../contextualplanner-jni.hpp"
#include "../jobjectsconversions.hpp"
#include "domain-jni.hpp"
#include <contextualplanner/util/print.hpp>

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
Java_com_contextualplanner_types_Problem_00024Companion_newProblem(
        JNIEnv *env, jobject /*object*/) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jint>(env, [&]() {
        return protectByMutexWithReturn<jint>([&]() {
            jint newLocalMemory = findMissingKey(_idToRobotPlannerProblem);
            _idToRobotPlannerProblem[newLocalMemory];
            return newLocalMemory;
        });
    }, -1);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_notifyActionDone(
        JNIEnv *env, jobject object, jstring jActionId, jobject domainObject) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        return protectByMutex([&]() {
            auto *domainPtr = idToDomainUnsafe(toId(env, domainObject));
            auto *problemPtr = idToProblemUnsafe(toId(env, object));
            if (domainPtr != nullptr && problemPtr != nullptr) {
                auto actionId = toString(env, jActionId);
                auto itAction = domainPtr->idToPlannerActions.find(actionId);
                if (itAction != domainPtr->idToPlannerActions.end())
                {
                    std::map<std::string, std::string> parameters;
                    auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
                    problemPtr->notifyActionDone(actionId, parameters, itAction->second.effect, now, &itAction->second.goalsToAdd);
                }
            }
        });
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_pushFrontGoal(
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
Java_com_contextualplanner_types_Problem_pushBackGoal(
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
Java_com_contextualplanner_types_Problem_removeGoals(
        JNIEnv *env, jobject object, jstring jGoalGroupStr) {
    protectByMutex([&]() {
        auto goalGroupStr = toString(env, jGoalGroupStr);
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            problemPtr->removeGoals(goalGroupStr, now);
        }
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_setGoalPriority(
        JNIEnv *env, jobject object, jstring jGoalStr,
        int pPriority,
        jboolean pPushFrontOrBottomInCaseOfConflictWithAnotherGoal) {
    protectByMutex([&]() {
        auto goalStr = toString(env, jGoalStr);
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            problemPtr->changeGoalPriority(goalStr, pPriority, pPushFrontOrBottomInCaseOfConflictWithAnotherGoal, now);
        }
    });
}


extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_contextualplanner_types_Problem_getGoals(
        JNIEnv *env, jobject object) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jobjectArray>(env, [&]() {
        return protectByMutexWithReturn<jobjectArray>([&]() {
            auto* problemPtr = idToProblemUnsafe(toId(env, object));
            jclass goalClass = env->FindClass("com/contextualplanner/types/Goal");
            jmethodID goalClassConstructor =
                    env->GetMethodID(goalClass, "<init>",
                                     "(ILjava/lang/String;Ljava/lang/String;ILjava/lang/String;)V");

            jobjectArray result;
            if (problemPtr != nullptr)
            {
                auto& goals = problemPtr->goals();

                std::vector<std::pair<int, cp::Goal>> prioritiesToGoal;
                for (auto itGoalsGroup = goals.end(); itGoalsGroup != goals.begin(); )
                {
                    --itGoalsGroup;
                    for (auto& currGoal : itGoalsGroup->second)
                        prioritiesToGoal.emplace_back(itGoalsGroup->first, currGoal);
                }

                result = (jobjectArray)env->NewObjectArray(prioritiesToGoal.size(), goalClass,
                                                           env->NewObject(goalClass, goalClassConstructor,
                                                                          0, env->NewStringUTF(""),
                                                                          env->NewStringUTF(""),
                                                                          -1, env->NewStringUTF("")));

                jsize arrayElt = 0;
                for (const auto& currPriorityToGoal : prioritiesToGoal) {
                    env->SetObjectArrayElement(result, arrayElt++,
                                               newJavaGoal(env, currPriorityToGoal.first, currPriorityToGoal.second));
                }
                return result;
            }
            result = (jobjectArray)env->NewObjectArray(0, goalClass, env->NewStringUTF(""));
            return result;
        });
    }, nullptr);
}






extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_addFact(
        JNIEnv *env, jobject object, jstring jFact) {
    protectByMutex([&]() {
        auto fact = toString(env, jFact);
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr) {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            problemPtr->addFact(fact, now);
        }
    });
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_contextualplanner_types_Problem_hasFact(
        JNIEnv *env, jobject object, jstring jFact) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jboolean>(env, [&]() {
        return protectByMutexWithReturn<jboolean>([&]() {
            auto fact = toString(env, jFact);
            auto* problemPtr = idToProblemUnsafe(toId(env, object));
            if (problemPtr != nullptr)
                return problemPtr->hasFact(fact);
            return false;
        });
    }, false);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_removeFact(
        JNIEnv *env, jobject object, jstring jFact) {
    protectByMutex([&]() {
        auto fact = toString(env, jFact);
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr) {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            problemPtr->removeFact(fact, now);
        }
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_modifyFacts(
        JNIEnv *env, jobject object, jstring jFacts) {
    protectByMutex([&]() {
        static const char sep = '&';
        auto facts = cp::SetOfFacts::fromStr(toString(env, jFacts), sep);
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr) {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            problemPtr->modifyFacts(facts, now);
        }
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_addGoals(
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
Java_com_contextualplanner_types_Problem_removeFirstGoalsThatAreAlreadySatisfied(
        JNIEnv *env, jobject object) {
    protectByMutex([&]() {
        auto* problemPtr = idToProblemUnsafe(toId(env, object));
        if (problemPtr != nullptr)
            problemPtr->removeFirstGoalsThatAreAlreadySatisfied();
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_addVariableToValue(
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
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_addInference(
        JNIEnv *env, jobject object, jobject jinference) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto* problemPtr = idToProblemUnsafe(toId(env, object));
            if (problemPtr != nullptr)
            {
                std::string inferenceId;
                auto inference = toInference(env, jinference, inferenceId);
                problemPtr->addInference(inferenceId, inference);
            }
        });
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_removeInference(
        JNIEnv *env, jobject object, jstring jinferenceId) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto* problemPtr = idToProblemUnsafe(toId(env, object));
            if (problemPtr != nullptr)
            {
                auto inferenceId = toString(env, jinferenceId);
                problemPtr->removeInference(inferenceId);
            }
        });
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_disposeImplementation(
        JNIEnv *env, jobject object) {
    protectByMutex([&]() {
        _idToRobotPlannerProblem.erase(toId(env, object));
    });
}