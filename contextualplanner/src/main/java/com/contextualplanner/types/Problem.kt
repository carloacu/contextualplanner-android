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
     * @param oneStepOfPlannerResult Planner result step that motivated this action.
     * @param domain Domain containing all the possible actions.
     */
    external fun notifyActionDone(oneStepOfPlannerResult: OneStepOfPlannerResult, domain: Domain)

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
     * @brief Get the goals with their priorities to satisfy.
     * @return Goals with their priorities to satisfy.
     */
    external fun getGoals(): Array<GoalWithPriority>?

    /**
     * @brief Get the not satisfied goals.<br/>
     * A goal is not satisfied if the condition is true (if it exist) and if the value of the goal is not true.
     * @return Goals with their priorities to satisfy that are not already satisfied.
     */
    external fun getNotSatisfiedGoals(): Array<GoalWithPriority>?

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
     * @param goalWithPriority Goals with priorities to add.
     */
    external fun addGoals(goalWithPriority: Array<GoalWithPriority>)

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
     * @brief Are the facts contained in the problem.
     * @param[in] pSetOfFacts Facts to check.
     * @return True if all the fact are contained in the problem, false otherwise.
     */
    external fun areFactsTrue(setOfFacts: String): Boolean

    /**
     * @brief Add a set of inferences.
     * @param setOfInferences Set of inferences to add.
     */
    external fun addSetOfInferences(setOfInferences: SetOfInferences)

    /**
     * @brief Remove a set of inferences.
     * @param setOfInferences Set of inferences to remove.
     */
    external fun removeSetOfInferences(setOfInferences: SetOfInferences)

    /**
     * @brief Flush the cache of punctual facts.
     * @return Punctual facts raised since the last flush.
     */
    external fun flushPunctualFacts(): Array<String>

    /**
     * @brief Flush the cache of facts added.
     * @return Facts added since the last flush.<br/>
     * If a fact was added then removed it will not be present here.
     */
    external fun flushFactsAdded(): Array<String>

    /**
     * @brief What should be done to release this object.<br/>
     * /!\ You should never call this function but dispose() instead to release this object.
     */
    external override fun disposeImplementation()
}
