#include "jobjectsconversions.hpp"
#include <list>
#include <sstream>


namespace {
    std::string _getStringFromMethod(JNIEnv *pEnv, jclass pClass, jobject pOjbect, const char * pFunctionName)
    {
        jmethodID getFun = pEnv->GetMethodID(pClass, pFunctionName, "()Ljava/lang/String;");
        auto jStr = reinterpret_cast<jstring>(pEnv->CallObjectMethod(pOjbect, getFun));
        return toString(pEnv, jStr);
    }

    int _getIntFromMethod(JNIEnv *pEnv, jclass pClass, jobject pOjbect, const char * pFunctionName)
    {
        jmethodID getFun = pEnv->GetMethodID(pClass, pFunctionName, "()I");
        return pEnv->CallIntMethod(pOjbect, getFun);
    }

    std::map<int, std::vector<cp::Goal>> _getPrioritiesToGoalsFromMethod(JNIEnv *pEnv, jclass pClass, jobject pOjbect, const char * pFunctionName)
    {
        jmethodID getFun = pEnv->GetMethodID(pClass, pFunctionName, "()[Lcom/contextualplanner/types/GoalWithPriority;");
        auto jGoals = reinterpret_cast<jobjectArray>(pEnv->CallObjectMethod(pOjbect, getFun));
        return toGoalsWithPriorities(pEnv, jGoals);
    }

    std::vector<cp::Goal> _getGoalArrayFromMethod(JNIEnv *pEnv, jclass pClass, jobject pOjbect, const char * pFunctionName)
    {
        jmethodID getFun = pEnv->GetMethodID(pClass, pFunctionName, "()[Lcom/contextualplanner/types/Goal;");
        auto jGoals = reinterpret_cast<jobjectArray>(pEnv->CallObjectMethod(pOjbect, getFun));
        return toGoals(pEnv, jGoals);
    }

    cp::Goal _getGoalWithPriorityFromMethod(JNIEnv *pEnv, jclass pClass, jobject pOjbect, const char * pFunctionName, int& pPriority)
    {
        jmethodID getFun = pEnv->GetMethodID(pClass, pFunctionName, "()Lcom/contextualplanner/types/GoalWithPriority;");
        auto jGoal = reinterpret_cast<jobject>(pEnv->CallObjectMethod(pOjbect, getFun));
        return toGoalWithPriority(pEnv, jGoal, pPriority);
    }
}


std::string toString(JNIEnv *env, jstring inputString) {
    if (env == nullptr)
        return "";
    const char *cstring;
    if (!(cstring = env->GetStringUTFChars(inputString, nullptr)))
        return "";
    std::string string = cstring;
    env->ReleaseStringUTFChars(inputString, cstring);
    return string;
}

std::map<int, std::vector<cp::Goal>> toGoalsWithPriorities(JNIEnv *env, jobjectArray jGoals) {
    std::map<int, std::vector<cp::Goal>> res;
    int size = env->GetArrayLength(jGoals);
    for (int i = 0; i < size; ++i) {
        shared_jobject goalJObj(env, env->GetObjectArrayElement(jGoals, i));
        int priority = 0;
        auto goal = toGoalWithPriority(env, goalJObj.get(), priority);
        res[priority].push_back(goal);
    }
    return res;
}

std::vector<cp::Goal> toGoals(JNIEnv *env, jobjectArray jGoals) {
    std::vector<cp::Goal> res;
    int size = env->GetArrayLength(jGoals);
    for (int i = 0; i < size; ++i) {
        shared_jobject goalJObj(env, env->GetObjectArrayElement(jGoals, i));
        auto goal = toGoal(env, goalJObj.get());
        res.push_back(goal);
    }
    return res;
}

jint toId(JNIEnv *env, jobject object) {
    jclass semanticMemoryClass = env->FindClass("com/contextualplanner/types/detail/DisposableWithId");
    jmethodID getIdFun = env->GetMethodID(semanticMemoryClass, "getId", "()I");
    return env->CallIntMethod(object, getIdFun);
}


ActionWithId toActionWithId(JNIEnv *env, jobject action)
{
    jclass actionClass = env->FindClass("com/contextualplanner/types/Action");
    static const char andSep = '&';
    std::string id = _getStringFromMethod(env, actionClass, action, "getId");
    cp::Action res(cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getPrecondition"), andSep),
                   cp::WorldModification(cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getEffect"), andSep)));
    res.preferInContext = cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getPreferInContext"), ',');
    res.effect.potentialFactsModifications = cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getPotentialEffect"), andSep);
    res.effect.goalsToAdd = _getPrioritiesToGoalsFromMethod(env, actionClass, action, "getGoalsToAdd");
    res.effect.goalsToAddInCurrentPriority = _getGoalArrayFromMethod(env, actionClass, action, "getGoalsToAddInCurrentPriority");
    return {id, std::move(res)};
}

cp::Inference toInference(JNIEnv *env, jobject jinference, std::string& inferenceId)
{
    jclass inferenceClass = env->FindClass("com/contextualplanner/types/Inference");
    inferenceId = _getStringFromMethod(env, inferenceClass, jinference, "getId");
    static const char sep = '&';
    return {cp::SetOfFacts::fromStr(_getStringFromMethod(env, inferenceClass, jinference, "getCondition"), sep),
                         cp::SetOfFacts::fromStr(_getStringFromMethod(env, inferenceClass, jinference, "getFactsToModify"), sep),
            _getPrioritiesToGoalsFromMethod(env, inferenceClass, jinference, "getGoalsWithPrioritiesToAdd")};
}


cp::OneStepOfPlannerResult toOneStepOfPlannerResult(JNIEnv *env, jobject jOneStepOfPlanner)
{
    jclass oneStepOfPlannerResultClass = env->FindClass("com/contextualplanner/types/OneStepOfPlannerResult");

    int fromGoalPriority = 0;
    cp::Goal fromGoal = _getGoalWithPriorityFromMethod(env, oneStepOfPlannerResultClass, jOneStepOfPlanner, "getFromGoal", fromGoalPriority);
    auto actionId = _getStringFromMethod(env, oneStepOfPlannerResultClass, jOneStepOfPlanner, "getActionId");
    return cp::OneStepOfPlannerResult(actionId, {}, fromGoal, fromGoalPriority);
}


cp::Goal toGoalWithPriority(JNIEnv *env, jobject goal, int& pPriority)
{
    jclass goalWithPriorityClass = env->FindClass("com/contextualplanner/types/GoalWithPriority");
    pPriority = _getIntFromMethod(env, goalWithPriorityClass, goal, "getPriority");

    jmethodID getFun = env->GetMethodID(goalWithPriorityClass, "getGoal", "()Lcom/contextualplanner/types/Goal;");
    return toGoal(env, env->CallObjectMethod(goal, getFun));
}


cp::Goal toGoal(JNIEnv *env, jobject goal)
{
    jclass goalClass = env->FindClass("com/contextualplanner/types/Goal");
    auto fact = _getStringFromMethod(env, goalClass, goal, "getFact");
    int maxTimeToKeepInactive = _getIntFromMethod(env, goalClass, goal, "getMaxTimeToKeepInactive");
    auto groupId = _getStringFromMethod(env, goalClass, goal, "getGroupId");
    cp::Goal res(fact, maxTimeToKeepInactive, groupId);
    return res;
}


jobject newJavaGoalWithPriority(JNIEnv *env, int pPriority, jobject jGoal)
{
    jclass goalWithPriorityClass = env->FindClass("com/contextualplanner/types/GoalWithPriority");
    jmethodID goalWithPriorityClassConstructor =
            env->GetMethodID(goalWithPriorityClass, "<init>",
                             "(ILcom/contextualplanner/types/Goal;)V");
    return env->NewObject(goalWithPriorityClass, goalWithPriorityClassConstructor,
                          pPriority, jGoal);
}


jobject newJavaGoal(JNIEnv *env, const cp::Goal& pGoal)
{
    jclass goalClass = env->FindClass("com/contextualplanner/types/Goal");
    jmethodID goalClassConstructor =
            env->GetMethodID(goalClass, "<init>",
                             "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)V");
    std::string condition = pGoal.conditionFactOptionalPtr() != nullptr ?
                            pGoal.conditionFactOptionalPtr()->toStr() : "";
    return env->NewObject(goalClass, goalClassConstructor,
                          env->NewStringUTF(pGoal.toStr().c_str()),
                          env->NewStringUTF(condition.c_str()),
                          pGoal.getMaxTimeToKeepInactive(),
                          env->NewStringUTF(pGoal.getGoalGroupId().c_str()));
}