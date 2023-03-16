package com.contextualplanner.types

 /**
  * @brief A goal with an associated priority to add the goal a the good position in the stack.
  * @param priority Priority of this goal.
  * @param goal The goal.
  */
data class GoalWithPriority(
    val priority: Int,
    val goal: Goal
)
