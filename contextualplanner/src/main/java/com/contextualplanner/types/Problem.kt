package com.contextualplanner.types

import com.contextualplanner.types.detail.DisposableWithId
import com.contextualplanner.util.detail.ensureInitialized


/**
 * @brief Current world, goal for the world and historical of actions done.
 */
class Problem : DisposableWithId(newProblem()) {

    companion object {
        // Be sure that the C++ library is loaded.
        init {
            ensureInitialized()
        }

        /**
         * @brief Construct a problem.
         * @return Identifier of the object.
         */
        private external fun newProblem(): Int
    }

    /**
     * @brief Notify that an action has been done.
     * @param actionId Action identifier, of the finished action.
     * @param domain Domain containing all the possible actions.
     */
    external fun notifyActionDone(actionId: String, domain: Domain)

    /**
     * @brief Add a goal in front of the existing goals that have the same level of priority.
     * @param goal Goal to add.
     */
    external fun pushFrontGoal(goal: Goal)

    /**
     * @brief Add a goal on bottom of the existing goals that have the same level of priority.
     * @param goal Goal to add.
     */
    external fun pushBackGoal(goal: Goal)

    /**
     * @brief Remove some goals.
     * @param goalGroupId Group identifier of the goals to remove.
     */
    external fun removeGoals(goalGroupId: String)

    /**
     * @brief Change the priority of a goal.
     * @param goalName Goal concerned.
     * @param priority New priority to set.
     * @param pushFrontOrBottomInCaseOfConflictWithAnotherGoal Push in front or in bottom in case of conflict with another goal.
     */
    external fun setGoalPriority(goalName: String, priority: Int, pushFrontOrBottomInCaseOfConflictWithAnotherGoal: Boolean)

    /**
     * @brief Get the goals to satisfy.
     * @return Goals to satisfy.
     */
    external fun getGoals(): Array<Goal>?

    /**
     * @brief Add a fact.
     * @param fact Fact to add.
     */
    external fun addFact(fact: String)

    /**
     * @brief Check if the world has a fact.
     * @param fact Fact to check.
     */
    external fun hasFact(fact: String): Boolean

    /**
     * @brief Remove a fact.
     * @param fact Fact to remove.
     */
    external fun removeFact(fact: String)

    /**
     * @brief Modify some facts in the world.
     * @param facts Facts to modify.
     */
    external fun modifyFacts(facts: String)

    /**
     * @brief Add some goals.
     * @param goals Goals to add.
     */
    external fun addGoals(goals: Array<Goal>)

    /**
     * @brief Remove the first goals that are already satisfied.
     */
    external fun removeFirstGoalsThatAreAlreadySatisfied()

    /**
     * @brief Add variables to value.
     * @param variableName Name of the variable to add.
     * @param variableValue Value of the variable to add.
     */
    external fun addVariableToValue(variableName: String, variableValue: String)

    /**
     * @brief What should be done to release this object.<br/>
     * /!\ You should never call this function but dispose() instead to release this object.
     */
    external override fun disposeImplementation()
}
