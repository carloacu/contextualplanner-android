#include <regex>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <memory>
#include "../androidlog.hpp"
#include "../contextualplanner-jni.hpp"
#include "../jobjectsconversions.hpp"
#include "../problem-jni.hpp"
#include <contextualplanner/trackers/goalsremovedtracker.hpp>

namespace {
    class GoalsRemovedCollector {
    public:
        GoalsRemovedCollector(const cp::Problem& problem)
                : tracker(problem),
                  goalsRemoved(),
                  goalsRemovedConnection()
        {
        }

        ~GoalsRemovedCollector() {
            try {
                goalsRemovedConnection.disconnect();
            } catch (... ) {}
        }

        cp::GoalsRemovedTracker tracker;
        std::set<std::string> goalsRemoved;
        cpstd::observable::Connection goalsRemovedConnection;
    };

    std::map<jint, GoalsRemovedCollector> _idToGoalsRemovedCollector;

    GoalsRemovedCollector* _idToGoalsRemovedCollectorUnsafe(jint id) {
        auto it = _idToGoalsRemovedCollector.find(id);
        if (it != _idToGoalsRemovedCollector.end())
            return &it->second;
        return nullptr;
    }
}



extern "C"
JNIEXPORT jint JNICALL
Java_com_contextualplanner_trackers_GoalsRemovedTracker_00024Companion_newGoalsRemovedTracker(
        JNIEnv *env, jobject /*object*/, jobject problemObject) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jint>(env, [&]() {
        return protectByMutexWithReturn<jint>([&]() {
            auto* problemPtr = idToProblemUnsafe(toId(env, problemObject));
            if (problemPtr != nullptr) {
                jint newObjectId = findMissingKey(_idToGoalsRemovedCollector);
                auto& goalsRemovedTracker = _idToGoalsRemovedCollector.emplace(newObjectId, *problemPtr).first->second;
                goalsRemovedTracker.goalsRemovedConnection = goalsRemovedTracker.tracker.onGoalsRemoved.connectUnsafe([&](const std::set<std::string>& pGoalsRemoved) {
                    goalsRemovedTracker.goalsRemoved.insert(pGoalsRemoved.begin(), pGoalsRemoved.end());
                });
                return newObjectId;
            }
            return -1;
        });
    }, -1);
}


extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_contextualplanner_trackers_GoalsRemovedTracker_flushGoalsRemoved(
        JNIEnv *env, jobject jobject) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jobjectArray>(env, [&]() {
        return protectByMutexWithReturn<jobjectArray>([&]() {
            jclass stringClass = env->FindClass("java/lang/String");
            jobjectArray result;
            auto* goalsRemovedCollectorPtr = _idToGoalsRemovedCollectorUnsafe(toId(env, jobject));
            if (goalsRemovedCollectorPtr != nullptr) {
                result = (jobjectArray)env->NewObjectArray(goalsRemovedCollectorPtr->goalsRemoved.size(), stringClass,
                                                           env->NewStringUTF(""));

                jsize arrayElt = 0;
                for (const auto& currGoalRemoved : goalsRemovedCollectorPtr->goalsRemoved) {
                    env->SetObjectArrayElement(result, arrayElt++,
                                               env->NewStringUTF(currGoalRemoved.c_str()));
                }
            } else {
                result = (jobjectArray)env->NewObjectArray(0, stringClass,env->NewStringUTF(""));
            }
            goalsRemovedCollectorPtr->goalsRemoved.clear();
            return result;
        });
    }, nullptr);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_trackers_GoalsRemovedTracker_disposeImplementation(
        JNIEnv *env, jobject object) {
    protectByMutex([&]() {
        _idToGoalsRemovedCollector.erase(toId(env, object));
    });
}