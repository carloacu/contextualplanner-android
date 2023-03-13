package com.contextualplanner.types


/**
 * @brief Class containing an action identifier and a goal.
 * @param actionId Action identifier.
 * @param goal Goal.
 */
data class OneStepOfPlannerResult(val actionId: String, val goal: Goal)
