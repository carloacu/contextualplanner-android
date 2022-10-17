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
    bool _getBooleanFromMethod(JNIEnv *pEnv, jclass pClass, jobject pOjbect, const char * pFunctionName)
    {
        jmethodID getFun = pEnv->GetMethodID(pClass, pFunctionName, "()Z");
        return pEnv->CallBooleanMethod(pOjbect, getFun);
    }

    std::map<int, std::vector<cp::Goal>> _getGoalArrayFromMethod(JNIEnv *pEnv, jclass pClass, jobject pOjbect, const char * pFunctionName)
    {
        jmethodID getFun = pEnv->GetMethodID(pClass, pFunctionName, "()[Lcom/contextualplanner/types/Goal;");
        auto jGoals = reinterpret_cast<jobjectArray>(pEnv->CallObjectMethod(pOjbect, getFun));
        return toGoals(pEnv, jGoals);
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

std::map<int, std::vector<cp::Goal>> toGoals(JNIEnv *env, jobjectArray jGoals) {
    std::map<int, std::vector<cp::Goal>> res;
    int size = env->GetArrayLength(jGoals);
    for (int i = 0; i < size; ++i) {
        shared_jobject goalJObj(env, env->GetObjectArrayElement(jGoals, i));
        int priority = 0;
        auto goal = toGoal(env, goalJObj.get(), &priority);
        res[priority].push_back(goal);
    }
    return res;
}

jint toId(JNIEnv *env, jobject object) {
    jclass semanticMemoryClass = env->FindClass("com/contextualplanner/types/detail/DisposableWithId");
    jmethodID getIdFun = env->GetMethodID(semanticMemoryClass, "getId", "()I");
    return env->CallIntMethod(object, getIdFun);
}


PlannerAction toPlannerAction(JNIEnv *env, jobject action)
{
    jclass actionClass = env->FindClass("com/contextualplanner/types/Action");
    PlannerAction res;
    res.id = _getStringFromMethod(env, actionClass, action, "getId");
    res.preferInContext = cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getPreferInContext"), ',');
    static const char sep = '&';
    res.precondition = cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getPrecondition"), sep);
    res.effect = cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getEffect"), sep);
    res.potentialEffect = cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getPotentialEffect"), sep);
    res.goalsToAdd = _getGoalArrayFromMethod(env, actionClass, action, "getGoalsToAdd");
    return res;
}

cp::Inference toInference(JNIEnv *env, jobject jinference, std::string& inferenceId)
{
    jclass inferenceClass = env->FindClass("com/contextualplanner/types/Inference");
    inferenceId = _getStringFromMethod(env, inferenceClass, jinference, "getId");
    static const char sep = '&';
    return {cp::SetOfFacts::fromStr(_getStringFromMethod(env, inferenceClass, jinference, "getCondition"), sep),
                         cp::SetOfFacts::fromStr(_getStringFromMethod(env, inferenceClass, jinference, "getFactsToModify"), sep),
                         _getGoalArrayFromMethod(env, inferenceClass, jinference, "getGoalsToAdd")};
}

cp::Goal toGoal(JNIEnv *env, jobject goal, int* pPriority)
{
    jclass goalClass = env->FindClass("com/contextualplanner/types/Goal");
    auto fact = _getStringFromMethod(env, goalClass, goal, "getFact");
    int maxTimeToKeepInactive = _getIntFromMethod(env, goalClass, goal, "getMaxTimeToKeepInactive");
    auto groupId = _getStringFromMethod(env, goalClass, goal, "getGroupId");
    cp::Goal res(fact, maxTimeToKeepInactive, groupId);
    if (pPriority != nullptr)
        *pPriority = _getIntFromMethod(env, goalClass, goal, "getPriority");
    return res;
}


jobject newJavaGoal(JNIEnv *env, int pPriority, const cp::Goal& pGoal)
{
    jclass goalClass = env->FindClass("com/contextualplanner/types/Goal");
    jmethodID goalClassConstructor =
            env->GetMethodID(goalClass, "<init>",
                             "(ILjava/lang/String;Ljava/lang/String;ILjava/lang/String;)V");
    std::string condition = pGoal.conditionFactOptionalPtr() != nullptr ?
                            pGoal.conditionFactOptionalPtr()->toStr() : "";
    return env->NewObject(goalClass, goalClassConstructor,
                          pPriority,
                          env->NewStringUTF(pGoal.toStr().c_str()),
                          env->NewStringUTF(condition.c_str()),
                          pGoal.getMaxTimeToKeepInactive(),
                          env->NewStringUTF(pGoal.getGoalGroupId().c_str()));
}