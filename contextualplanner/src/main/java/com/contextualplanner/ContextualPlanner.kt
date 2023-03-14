package com.contextualplanner

import com.contextualplanner.types.*


/**
 * @brief Ask the planner to get the next action to do.
 * @param[in, out] problem Problem of the planner.
 * @param[in] domain Domain of the planner
 * @return A class containing the action to do and the goal that motivated to do this action, or null if no action was found.
 */
fun lookForAnActionToDo(problem: Problem, domain: Domain): OneStepOfPlannerResult? {
    val res = lookForAnActionToDoCpp(problem, domain)
    if (res.actionId == "")
        return null
    return res
}

private external fun lookForAnActionToDoCpp(problem: Problem, domain: Domain): OneStepOfPlannerResult


/**
 * @brief Notify that an action has been done.
 * @param oneStepOfPlannerResult Planner result step that motivated this action.
 * @param problem Problem to be notified.
 * @param domain Domain containing all the possible actions.
 */
fun notifyActionDone(oneStepOfPlannerResult: OneStepOfPlannerResult, problem: Problem, domain: Domain) {
    problem.notifyActionDone(oneStepOfPlannerResult, domain)
}
