#ifndef CONTEXTUALPLANNER_DOMAIN_JNI_HPP
#define CONTEXTUALPLANNER_DOMAIN_JNI_HPP

#include <map>
#include <jni.h>
#include <contextualplanner/contextualplanner.hpp>
#include "jobjectsconversions.hpp"


struct PlannerDomain {

    PlannerDomain(std::map<cp::ActionId, PlannerAction> &&pIdToPlannerActions,
                  const std::map<cp::ActionId, cp::Action> &pActions)
            : idToPlannerActions(std::move(pIdToPlannerActions)),
              domain(pActions) {
    }

    std::map<cp::ActionId, PlannerAction> idToPlannerActions;
    cp::Domain domain;
};


const PlannerDomain* idToDomainUnsafe(jint id);


#endif // CONTEXTUALPLANNER_DOMAIN_JNI_HPP
