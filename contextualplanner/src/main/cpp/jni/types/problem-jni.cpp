#include <sstream>
#include "../androidlog.hpp"
#include "../contextualplanner-jni.hpp"
#include "../jobjectsconversions.hpp"
#include "domain-jni.hpp"
#include "setofinferences-jni.hpp"
#include <contextualplanner/util/print.hpp>

namespace {
    class RobotPlannerProblem {
    public:
        RobotPlannerProblem()
                : problem(),
                  punctualFacts(),
                  factsAdded(),
                  punctualFactsConnection(),
                  factsAddedConnection(),
                  factsRemovedConnection()
        {
        }

        ~RobotPlannerProblem() {
            try {
                punctualFactsConnection.disconnect();
            } catch (... ) {}
            try {
                factsAddedConnection.disconnect();
            } catch (... ) {}
            try {
                factsRemovedConnection.disconnect();
            } catch (... ) {}
        }

        cp::Problem problem;
        std::set<cp::Fact> punctualFacts;
        std::set<cp::Fact> factsAdded;
        cpstd::observable::Connection punctualFactsConnection;
        cpstd::observable::Connection factsAddedConnection;
        cpstd::observable::Connection factsRemovedConnection;
    };
    std::map<jint, RobotPlannerProblem> _idToRobotPlannerProblem;

    RobotPlannerProblem* _idToPlannerProblemUnsafe(jint id) {
        auto it = _idToRobotPlannerProblem.find(id);
        if (it != _idToRobotPlannerProblem.end())
            return &it->second;
        return nullptr;
    }

    jobjectArray _flushFacts(
            JNIEnv *env,
            std::set<cp::Fact>& pFacts) {
        jclass stringClass = env->FindClass("java/lang/String");
        jobjectArray result;
        result = (jobjectArray)env->NewObjectArray(pFacts.size(), stringClass,
                                                   env->NewStringUTF(""));

        jsize arrayElt = 0;
        for (const auto& currFactAdded : pFacts) {
            env->SetObjectArrayElement(result, arrayElt++,
                                       env->NewStringUTF(currFactAdded.toStr().c_str()));
        }

        pFacts.clear();
        return result;
    }

    jobjectArray _convertGoalsToJava(
            JNIEnv *env,
            const std::map<int, std::vector<cp::Goal>>& goals) {
        std::vector<std::pair<int, cp::Goal>> prioritiesToGoal;
        for (auto itGoalsGroup = goals.end(); itGoalsGroup != goals.begin(); )
        {
            --itGoalsGroup;
            for (auto& currGoal : itGoalsGroup->second)
                prioritiesToGoal.emplace_back(itGoalsGroup->first, currGoal);
        }

        jclass goalClass = env->FindClass("com/contextualplanner/types/Goal");
        jmethodID goalClassConstructor =
                env->GetMethodID(goalClass, "<init>",
                                 "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)V");
        jclass goalWithPriorityClass = env->FindClass("com/contextualplanner/types/GoalWithPriority");
        jmethodID goalWithPriorityClassConstructor =
                env->GetMethodID(goalWithPriorityClass, "<init>",
                                 "(ILcom/contextualplanner/types/Goal;)V");
        jobjectArray result = (jobjectArray)env->NewObjectArray(prioritiesToGoal.size(), goalWithPriorityClass,
                                                                env->NewObject(goalWithPriorityClass, goalWithPriorityClassConstructor,
                                                                               0,
                                                                               env->NewObject(goalClass, goalClassConstructor,
                                                                                              env->NewStringUTF(""),
                                                                                              env->NewStringUTF(""),
                                                                                              -1, env->NewStringUTF(""))
                                                                               ));

        jsize arrayElt = 0;
        for (const auto& currPriorityToGoal : prioritiesToGoal) {
            env->SetObjectArrayElement(result, arrayElt++,
                                       newJavaGoalWithPriority(env, currPriorityToGoal.first, newJavaGoal(env, currPriorityToGoal.second)));
        }
        return result;
    }
}

cp::Problem* idToProblemUnsafe(jint id)
{
    auto it = _idToRobotPlannerProblem.find(id);
    if (it != _idToRobotPlannerProblem.end())
        return &it->second.problem;
    return nullptr;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_contextualplanner_types_Problem_00024Companion_newProblem(
        JNIEnv *env, jobject /*object*/) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jint>(env, [&]() {
        return protectByMutexWithReturn<jint>([&]() {
            jint newObjectId = findMissingKey(_idToRobotPlannerProblem);
            auto& robotPlannerProblem = _idToRobotPlannerProblem[newObjectId];
            robotPlannerProblem.punctualFactsConnection = robotPlannerProblem.problem.onPunctualFacts.connectUnsafe([&](const std::set<cp::Fact>& pFacts) {
                robotPlannerProblem.punctualFacts.insert(pFacts.begin(), pFacts.end());
            });
            robotPlannerProblem.factsAddedConnection = robotPlannerProblem.problem.onFactsAdded.connectUnsafe([&](const std::set<cp::Fact>& pFacts) {
                robotPlannerProblem.factsAdded.insert(pFacts.begin(), pFacts.end());
            });
            robotPlannerProblem.factsRemovedConnection = robotPlannerProblem.problem.onFactsRemoved.connectUnsafe([&](const std::set<cp::Fact>& pFacts) {
                for (auto& currFact : pFacts)
                    robotPlannerProblem.factsAdded.erase(currFact);
            });
            return newObjectId;
        });
    }, -1);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_notifyActionDone(
        JNIEnv *env, jobject object, jobject jOneStepPlannerResult, jobject domainObject) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        return protectByMutex([&]() {
            auto *domainPtr = idToDomainUnsafe(toId(env, domainObject));
            auto *plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
            if (domainPtr != nullptr && plannerProblemPtr != nullptr) {
                auto oneStepOfPlannerResult = toOneStepOfPlannerResult(env, jOneStepPlannerResult);
                auto itAction = domainPtr->actions().find(oneStepOfPlannerResult.actionInstance.actionId);
                if (itAction != domainPtr->actions().end())
                {
                    auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
                    plannerProblemPtr->problem.notifyActionDone(oneStepOfPlannerResult, itAction->second.effect.factsModifications, now,
                                                                &itAction->second.effect.goalsToAdd, &itAction->second.effect.goalsToAddInCurrentPriority);
                }
            }
        });
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_pushFrontGoal(
        JNIEnv *env, jobject object, jobject jGoalWithPriority) {
    protectByMutex([&]() {
        int priority = 0;
        auto goal = toGoalWithPriority(env, jGoalWithPriority, priority);
        auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
        if (plannerProblemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            plannerProblemPtr->problem.pushFrontGoal(goal, now, priority);
        }
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_pushBackGoal(
        JNIEnv *env, jobject object, jobject jGoalWithPriority) {
    protectByMutex([&]() {
        int priority = 0;
        auto goal = toGoalWithPriority(env, jGoalWithPriority, priority);
        auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
        if (plannerProblemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            plannerProblemPtr->problem.pushBackGoal(goal, now, priority);
        }
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_removeGoals(
        JNIEnv *env, jobject object, jstring jGoalGroupStr) {
    protectByMutex([&]() {
        auto goalGroupStr = toString(env, jGoalGroupStr);
        auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
        if (plannerProblemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            plannerProblemPtr->problem.removeGoals(goalGroupStr, now);
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
        auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
        if (plannerProblemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            plannerProblemPtr->problem.changeGoalPriority(goalStr, pPriority, pPushFrontOrBottomInCaseOfConflictWithAnotherGoal, now);
        }
    });
}


extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_contextualplanner_types_Problem_getGoals(
        JNIEnv *env, jobject object) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jobjectArray>(env, [&]() {
        return protectByMutexWithReturn<jobjectArray>([&]() {
            auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
            if (plannerProblemPtr != nullptr)
                return _convertGoalsToJava(env, plannerProblemPtr->problem.goals());
            return _convertGoalsToJava(env, {});
        });
    }, nullptr);
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_contextualplanner_types_Problem_getNotSatisfiedGoals(
        JNIEnv *env, jobject object) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jobjectArray>(env, [&]() {
        return protectByMutexWithReturn<jobjectArray>([&]() {
            auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
            if (plannerProblemPtr != nullptr)
                return _convertGoalsToJava(env, plannerProblemPtr->problem.getNotSatisfiedGoals());
            return _convertGoalsToJava(env, {});
        });
    }, nullptr);
}



extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_addFact(
        JNIEnv *env, jobject object, jstring jFact) {
    protectByMutex([&]() {
        auto fact = toString(env, jFact);
        auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
        if (plannerProblemPtr != nullptr) {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            plannerProblemPtr->problem.addFact(fact, now);
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
            auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
            if (plannerProblemPtr != nullptr)
                return plannerProblemPtr->problem.hasFact(fact);
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
        auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
        if (plannerProblemPtr != nullptr) {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            plannerProblemPtr->problem.removeFact(fact, now);
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
        auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
        if (plannerProblemPtr != nullptr) {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            plannerProblemPtr->problem.modifyFacts(facts, now);
        }
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_addGoals(
        JNIEnv *env, jobject object, jobjectArray jGoals) {
    protectByMutex([&]() {
        auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
        if (plannerProblemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            auto goals = toGoalsWithPriorities(env, jGoals);
            plannerProblemPtr->problem.addGoals(goals, now);
        }
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_removeFirstGoalsThatAreAlreadySatisfied(
        JNIEnv *env, jobject object) {
    protectByMutex([&]() {
        auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
        if (plannerProblemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            plannerProblemPtr->problem.removeFirstGoalsThatAreAlreadySatisfied(now);
        }
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
        auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
        if (plannerProblemPtr != nullptr)
        {
            auto now = std::make_unique<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            plannerProblemPtr->problem.addVariablesToValue(variablesToValue, now);
        }
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_addSetOfInferences(
        JNIEnv *env, jobject object, jobject jsetOfInferences) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
            auto setOfInferencesId = toId(env, jsetOfInferences);
            auto setOfInferencesPtr = idToSetOfInferencesUnsafe(setOfInferencesId);
            if (plannerProblemPtr != nullptr && setOfInferencesPtr)
            {
                std::stringstream setOfInferencesIdSS;
                setOfInferencesIdSS << setOfInferencesId;
                plannerProblemPtr->problem.addSetOfInferences(setOfInferencesIdSS.str(), setOfInferencesPtr);
            }
        });
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_removeSetOfInferences(
        JNIEnv *env, jobject object, jobject jsetOfInferences) {
    convertCppExceptionsToJavaExceptions(env, [&]() {
        protectByMutex([&]() {
            auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
            auto setOfInferencesId = toId(env, jsetOfInferences);
            auto setOfInferencesPtr = idToSetOfInferencesUnsafe(setOfInferencesId);
            if (plannerProblemPtr != nullptr && setOfInferencesPtr)
            {
                std::stringstream setOfInferencesIdSS;
                setOfInferencesIdSS << setOfInferencesId;
                plannerProblemPtr->problem.removeSetOfInferences(setOfInferencesIdSS.str());
            }
        });
    });
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_contextualplanner_types_Problem_areFactsTrue(
        JNIEnv *env, jobject object, jstring jsetOfFactsStr) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jboolean>(env, [&]() {
        return protectByMutexWithReturn<jboolean>([&]() {
            auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
            if (plannerProblemPtr != nullptr)
            {
                auto setOfFactsStr = toString(env, jsetOfFactsStr);
                return plannerProblemPtr->problem.areFactsTrue(cp::SetOfFacts::fromStr(setOfFactsStr, '&'));
            }
            return false;
        });
    }, false);
}


extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_contextualplanner_types_Problem_flushPunctualFacts(
        JNIEnv *env, jobject object) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jobjectArray>(env, [&]() {
        return protectByMutexWithReturn<jobjectArray>([&]() {
            auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
            if (plannerProblemPtr != nullptr) {
                return _flushFacts(env, plannerProblemPtr->punctualFacts);
            }
            jclass stringClass = env->FindClass("java/lang/String");
            return (jobjectArray)env->NewObjectArray(0, stringClass,env->NewStringUTF(""));
        });
    }, nullptr);
}


extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_contextualplanner_types_Problem_flushFactsAdded(
        JNIEnv *env, jobject object) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jobjectArray>(env, [&]() {
        return protectByMutexWithReturn<jobjectArray>([&]() {
            auto* plannerProblemPtr = _idToPlannerProblemUnsafe(toId(env, object));
            if (plannerProblemPtr != nullptr) {
                return _flushFacts(env, plannerProblemPtr->factsAdded);
            }
            jclass stringClass = env->FindClass("java/lang/String");
            return (jobjectArray)env->NewObjectArray(0, stringClass,env->NewStringUTF(""));
        });
    }, nullptr);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_types_Problem_disposeImplementation(
        JNIEnv *env, jobject object) {
    protectByMutex([&]() {
        auto it = _idToRobotPlannerProblem.find(toId(env, object));
        if (it != _idToRobotPlannerProblem.end())
            _idToRobotPlannerProblem.erase(it);
    });
}