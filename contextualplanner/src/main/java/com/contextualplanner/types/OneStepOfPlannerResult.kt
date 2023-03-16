package com.contextualplanner.types


/**
 * @brief Class gathering the result of one step of the planner.
 * @param actionId Action identifier.
 * @param fromGoal Goal with his priority that motivated the action.
 */
data class OneStepOfPlannerResult(val actionId: String, val fromGoal: GoalWithPriority)
