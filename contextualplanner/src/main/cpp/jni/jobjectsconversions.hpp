#ifndef CONTEXTUALPLANNER_JOBJECTSCONVERSIONS_HPP
#define CONTEXTUALPLANNER_JOBJECTSCONVERSIONS_HPP

#include <map>
#include <string>
#include <jni.h>
#include <contextualplanner/contextualplanner.hpp>

/**
 * All these jni conversions functions are called without thread protections.
 */


struct PlannerAction
{
    std::string id;
    cp::SetOfFacts preferInContext;
    cp::SetOfFacts precondition;
    cp::SetOfFacts effect;
    cp::SetOfFacts potentialEffect;
    std::map<int, std::vector<cp::Goal>> goalsToAdd;
};


/**
 * Class to store a jobject.
 * The reference of this jobject will be deleted from the jni environment at the destruction of this object.
 */
class shared_jobject {
public:
    shared_jobject(
            JNIEnv *env,
            jobject object)
            : _env(env),
              _object(object) {}
    ~shared_jobject() {
        _env->DeleteLocalRef(_object);
    }
    jobject get() { return _object; }

private:
    JNIEnv* _env;
    jobject _object;
};


std::string toString(JNIEnv *env, jstring inputString);

std::map<int, std::vector<cp::Goal>> toGoals(JNIEnv *env, jobjectArray jGoals);

jint toId(JNIEnv *env, jobject object);

PlannerAction toPlannerAction(JNIEnv *env, jobject action);

cp::Goal toGoal(JNIEnv *env, jobject goal, int* pPriority);

jobject newJavaGoal(JNIEnv *env, int pPriority, const cp::Goal& pGoal);


#endif // CONTEXTUALPLANNER_JOBJECTSCONVERSIONS_HPP
