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

    std::vector<cp::Goal> _getGoalArrayFromMethod(JNIEnv *pEnv, jclass pClass, jobject pOjbect, const char * pFunctionName)
    {
        jmethodID getFun = pEnv->GetMethodID(pClass, pFunctionName, "()[Ljava/lang/String;");
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

std::vector<cp::Goal> toGoals(JNIEnv *env, jobjectArray jGoals) {
    std::vector<cp::Goal> res;
    int size = env->GetArrayLength(jGoals);
    for (int i = 0; i < size; ++i) {
        auto goalJStr = reinterpret_cast<jstring>(env->GetObjectArrayElement(
                jGoals, i));
        res.emplace_back(toString(env, goalJStr));
        env->DeleteLocalRef(goalJStr);
    }
    return res;
}

jint toId(JNIEnv *env, jobject object) {
    jclass semanticMemoryClass = env->FindClass("com/contextualplanner/DisposableWithId");
    jmethodID getIdFun = env->GetMethodID(semanticMemoryClass, "getId", "()I");
    return env->CallIntMethod(object, getIdFun);
}


PlannerAction toPlannerAction(JNIEnv *env, jobject action)
{
    jclass actionClass = env->FindClass("com/contextualplanner/Action");
    PlannerAction res;
    res.id = _getStringFromMethod(env, actionClass, action, "getId");
    static const char sep = ',';
    res.preferInContext = cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getPreferInContext"), sep);
    res.precondition = cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getPrecondition"), sep);
    res.effect = cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getEffect"), sep);
    res.potentialEffect = cp::SetOfFacts::fromStr(_getStringFromMethod(env, actionClass, action, "getPotentialEffect"), sep);
    res.goalsToAdd = _getGoalArrayFromMethod(env, actionClass, action, "getGoalsToAdd");
    return res;
}