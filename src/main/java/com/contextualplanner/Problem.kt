package com.contextualplanner



class Problem : DisposableWithId(newProblem()) {

    companion object {
        init {
            ensureInitialized()
        }
    }

    override fun disposeImplementation(id: Int) {
        deleteProblem(id)
    }

    fun pushFrontGoal(goal: String, goalGroupId: String) {
        pushFrontGoal(id, goal, goalGroupId)
    }

    fun pushBackGoal(goal: String) {
        pushBackGoal(id, goal)
    }

    fun addFact(fact: String) {
        addFact(id, fact)
    }

    fun removeFact(fact: String) {
        removeFact(id, fact)
    }

    fun modifyFacts(facts: String) {
        modifyFacts(id, facts)
    }

    fun addGoals(goals: Array<String>) {
        addGoals(id, goals)
    }

    fun removeFirstGoalsThatAreAlreadySatisfied() {
        removeFirstGoalsThatAreAlreadySatisfied(id)
    }

    fun addVariableToValue(variableName: String, variableValue: String) {
        addVariableToValue(id, variableName, variableValue)
    }

    fun printGoals(): String {
        return printGoals(id)
    }

    fun printFacts(): String {
        return printFacts(id)
    }
}



private external fun newProblem(): Int

private external fun deleteProblem(id: Int)

private external fun pushFrontGoal(id: Int, goal: String, goalGroupId: String)
private external fun pushBackGoal(id: Int, goal: String)
private external fun addFact(id: Int, fact: String)
private external fun removeFact(id: Int, fact: String)
private external fun modifyFacts(id: Int, facts: String)
private external fun addGoals(id: Int, goals: Array<String>)
private external fun removeFirstGoalsThatAreAlreadySatisfied(id: Int)
private external fun addVariableToValue(id: Int, variableName: String, variableValue: String)
private external fun printGoals(id: Int): String
private external fun printFacts(id: Int): String