package com.contextualplanner



class Problem : DisposableWithId(newProblem()) {

    companion object {
        init {
            ensureInitialized()
        }
        private external fun newProblem(): Int
    }

    external override fun disposeImplementation()

    external fun pushFrontGoal(goal: String, goalGroupId: String)

    external fun pushBackGoal(goal: String)

    external fun addFact(fact: String)

    external fun removeFact(fact: String)

    external fun modifyFacts(facts: String)

    external fun addGoals(goals: Array<String>)

    external fun removeFirstGoalsThatAreAlreadySatisfied()

    external fun addVariableToValue(variableName: String, variableValue: String)

    external fun printGoals(): String

    external fun printFacts(): String
}




