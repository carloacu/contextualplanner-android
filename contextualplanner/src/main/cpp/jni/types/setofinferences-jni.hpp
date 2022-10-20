#ifndef CONTEXTUALPLANNER_SETOFINFERENCES_JNI_HPP
#define CONTEXTUALPLANNER_SETOFINFERENCES_JNI_HPP

#include <jni.h>
#include <contextualplanner/contextualplanner.hpp>


std::shared_ptr<cp::SetOfInferences> idToSetOfInferencesUnsafe(jint id);


#endif // CONTEXTUALPLANNER_SETOFINFERENCES_JNI_HPP
