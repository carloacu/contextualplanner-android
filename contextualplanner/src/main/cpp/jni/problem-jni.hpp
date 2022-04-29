#ifndef CONTEXTUALPLANNER_PROBLEM_JNI_HPP
#define CONTEXTUALPLANNER_PROBLEM_JNI_HPP

#include <jni.h>
#include <contextualplanner/contextualplanner.hpp>


cp::Problem* idToProblemUnsafe(jint id);


#endif // CONTEXTUALPLANNER_PROBLEM_JNI_HPP
