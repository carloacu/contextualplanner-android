package com.contextualplanner

import com.contextualplanner.types.*


/**
 * @brief Ask the planner to get the next action to do.
 * @param[in, out] pProblem Problem of the planner.
 * @param[in] pDomain Domain of the planner
 * @return A class containing the action to do and the goal that motivated to do this action.
 */
external fun lookForAnActionToDo(problem: Problem, domain: Domain): OneStepOfPlannerResult


/**
 * @brief Notify that an action has been done.
 * @param oneStepOfPlannerResult Planner result step that motivated this action.
 * @param problem Problem to be notified.
 * @param domain Domain containing all the possible actions.
 */
fun notifyActionDone(oneStepOfPlannerResult: OneStepOfPlannerResult, problem: Problem, domain: Domain) {
    problem.notifyActionDone(oneStepOfPlannerResult, domain)
}
