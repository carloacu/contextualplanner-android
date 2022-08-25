package com.contextualplanner

import com.contextualplanner.types.*


/**
 * @brief Ask the planner to get the next action to do.
 * @param[in, out] pProblem Problem of the planner.
 * @param[in] pDomain Domain of the planner
 * @return A class containing the action to do and the goal that motivated to do this action.
 */
external fun lookForAnActionToDo(problem: Problem, domain: Domain): ActionAndGoal


/**
 * @brief Notify that an action has been done.
 * @param action Action finished.
 * @param problem Problem to be notified.
 * @param domain Domain containing all the possible actions.
 */
fun notifyActionDone(action: Action, problem: Problem, domain: Domain) {
    problem.notifyActionDone(action.id, domain)
}


/**
 * @brief Notify that an action has been done.
 * @param actionId Action identifier, of the finished action.
 * @param problem Problem to be notified.
 * @param domain Domain containing all the possible actions.
 */
fun notifyActionDone(actionId: String, problem: Problem, domain: Domain) {
    problem.notifyActionDone(actionId, domain)
}