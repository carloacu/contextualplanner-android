#ifndef CONTEXTUALPLANNER_DOMAIN_JNI_HPP
#define CONTEXTUALPLANNER_DOMAIN_JNI_HPP

#include <map>
#include <jni.h>
#include <contextualplanner/contextualplanner.hpp>
#include "../jobjectsconversions.hpp"



const cp::Domain* idToDomainUnsafe(jint id);


#endif // CONTEXTUALPLANNER_DOMAIN_JNI_HPP
