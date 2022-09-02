#include <regex>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <memory>
#include "../../androidlog.hpp"
#include "../../contextualplanner-jni.hpp"
#include "../../jobjectsconversions.hpp"
#include "../../types/problem-jni.hpp"
#include <contextualplanner/util/trackers/factschangedtracker.hpp>


namespace {
    class FactsAddedCollector {
    public:
        FactsAddedCollector(const cp::Problem& problem)
                : tracker(problem),
                  factsAdded(),
                  factsAddedConnection(),
                  factsRemovedConnection()
        {
        }

        ~FactsAddedCollector() {
            try {
                factsAddedConnection.disconnect();
            } catch (... ) {}
            try {
                factsRemovedConnection.disconnect();
            } catch (... ) {}
        }

        cp::FactsChangedTracker tracker;
        std::set<cp::Fact> factsAdded;
        cpstd::observable::Connection factsAddedConnection;
        cpstd::observable::Connection factsRemovedConnection;
    };

    std::map<jint, FactsAddedCollector> _idToFactsAddedCollector;

    FactsAddedCollector* _idToFactsAddedCollectorUnsafe(jint id) {
        auto it = _idToFactsAddedCollector.find(id);
        if (it != _idToFactsAddedCollector.end())
            return &it->second;
        return nullptr;
    }
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_contextualplanner_util_trackers_FactsAddedTracker_00024Companion_newFactsAddedTracker(
        JNIEnv *env, jobject /*object*/, jobject problemObject) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jint>(env, [&]() {
        return protectByMutexWithReturn<jint>([&]() {
            auto* problemPtr = idToProblemUnsafe(toId(env, problemObject));
            if (problemPtr != nullptr) {
                jint newObjectId = findMissingKey(_idToFactsAddedCollector);
                auto& factsAddedTracker = _idToFactsAddedCollector.emplace(newObjectId, *problemPtr).first->second;
                factsAddedTracker.factsAddedConnection = factsAddedTracker.tracker.onFactsAdded.connectUnsafe([&](const std::set<cp::Fact>& pFacts) {
                    factsAddedTracker.factsAdded.insert(pFacts.begin(), pFacts.end());
                });
                factsAddedTracker.factsRemovedConnection = factsAddedTracker.tracker.onFactsRemoved.connectUnsafe([&](const std::set<cp::Fact>& pFacts) {
                    for (auto& currFact : pFacts)
                        factsAddedTracker.factsAdded.erase(currFact);
                });
                return newObjectId;
            }
            return -1;
        });
    }, -1);
}


extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_contextualplanner_util_trackers_FactsAddedTracker_flushFactsAdded(
        JNIEnv *env, jobject jobject) {
    return convertCppExceptionsToJavaExceptionsAndReturnTheResult<jobjectArray>(env, [&]() {
        return protectByMutexWithReturn<jobjectArray>([&]() {
            jclass stringClass = env->FindClass("java/lang/String");
            jobjectArray result;
            auto* factsAddedCollectorPtr = _idToFactsAddedCollectorUnsafe(toId(env, jobject));
            if (factsAddedCollectorPtr != nullptr) {
                result = (jobjectArray)env->NewObjectArray(factsAddedCollectorPtr->factsAdded.size(), stringClass,
                                                           env->NewStringUTF(""));

                jsize arrayElt = 0;
                for (const auto& currFactAdded : factsAddedCollectorPtr->factsAdded) {
                    env->SetObjectArrayElement(result, arrayElt++,
                                               env->NewStringUTF(currFactAdded.toStr().c_str()));
                }
            } else {
                result = (jobjectArray)env->NewObjectArray(0, stringClass,env->NewStringUTF(""));
            }
            factsAddedCollectorPtr->factsAdded.clear();
            return result;
        });
    }, nullptr);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_contextualplanner_util_trackers_FactsAddedTracker_disposeImplementation(
        JNIEnv *env, jobject object) {
    protectByMutex([&]() {
        auto it = _idToFactsAddedCollector.find(toId(env, object));
        if (it != _idToFactsAddedCollector.end())
            _idToFactsAddedCollector.erase(it);
    });
}