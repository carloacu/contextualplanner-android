package com.contextualplanner



class Problem : DisposableWithId(newProblem()) {

    companion object {
        init {
            ensureInitialized()
        }
        private external fun newProblem(): Int
    }

    external override fun disposeImplementation()

    external fun pushFrontGoal(goal: Goal)

    external fun pushBackGoal(goal: Goal)

    external fun addFact(fact: String)

    external fun removeFact(fact: String)

    external fun modifyFacts(facts: String)

    external fun addGoals(goals: Array<Goal>)

    external fun removeFirstGoalsThatAreAlreadySatisfied()

    external fun addVariableToValue(variableName: String, variableValue: String)

    external fun printGoals(goalNameMaxSize: Int): String

    external fun printFacts(): String

    external fun getGoalsStr(): Array<String>
}




