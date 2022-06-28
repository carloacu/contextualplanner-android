package com.contextualplanner

import org.junit.Assert.*
import org.junit.Test
import java.lang.Thread.sleep


class PlannerTest {

    private val greetActionId = "greet-action"
    private val checkInActionId = "checkIn-action"
    private val informedAboutTheCompanyActionId = "informedAboutTheCompany-action"

    private val greetedFact = "greeted"
    private val checkedInFact = "checkedIn"
    private val informedAboutTheCompanyFact = "informedAboutTheCompany"

    @Test
    fun directLinkFromGoalToEffect() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(10, checkedInFact)))
        val actionAndGoal = lookForAnActionToDo(problem, domain)
        assertEquals(checkInActionId, actionAndGoal.actionId)
        assertEquals(checkedInFact, actionAndGoal.goal.name)
        assertEquals(10, actionAndGoal.goal.priority)
    }


    @Test
    fun preconditionResolution() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", greetedFact, checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(10, checkedInFact)))
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        notifyActionDone(greetActionId, problem, domain)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain).actionId)
    }

    @Test
    fun goalPriority() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        actions.add(Action(informedAboutTheCompanyActionId, "", "", informedAboutTheCompanyFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(9, checkedInFact)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        problem.addGoals(arrayOf(Goal(9, informedAboutTheCompanyFact)))

        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
    }


    @Test
    fun goalStackable() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(9, checkedInFact, stackable = true)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        notifyActionDone(greetActionId, problem, domain)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain).actionId)
    }


    @Test
    fun goalNotStackable() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(9, checkedInFact, stackable = false)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        notifyActionDone(greetActionId, problem, domain)
        assertEquals("", lookForAnActionToDo(problem, domain).actionId)
    }


    @Test
    fun goalInactiveLessThanTheMaxTimeToKeepInactive() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(9, checkedInFact, true, maxTimeToKeepInactive = 10)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        notifyActionDone(greetActionId, problem, domain)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain).actionId)
    }


    @Test
    fun goalInactiveMoreThanTheMaxTimeToKeepInactive() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(9, checkedInFact, true, maxTimeToKeepInactive = 1)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        notifyActionDone(greetActionId, problem, domain)
        sleep(2000)
        assertEquals("", lookForAnActionToDo(problem, domain).actionId)
    }


    @Test
    fun getGoals() {
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(9, checkedInFact, true, maxTimeToKeepInactive = 1)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        val goals = problem.getGoals()
        assertTrue(goals != null)
        assertEquals(2, goals!!.size)
        val firstGoal = goals[0]
        assertEquals(greetedFact, firstGoal.name)
        assertEquals(10, firstGoal.priority)
        assertEquals(true, firstGoal.stackable)
        assertEquals(-1, firstGoal.maxTimeToKeepInactive)

        val secondGoal = goals[1]
        assertEquals(checkedInFact, secondGoal.name)
        assertEquals(9, secondGoal.priority)
        assertEquals(true, secondGoal.stackable)
        assertEquals(1, secondGoal.maxTimeToKeepInactive)
    }

    @Test
    fun setGoalPriority() {
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(9, checkedInFact)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        problem.setGoalPriority(greetedFact, 8, true)

        val goals = problem.getGoals()
        assertTrue(goals != null)
        assertEquals(2, goals!!.size)
        val firstGoal = goals[0]
        assertEquals(checkedInFact, firstGoal.name)
        assertEquals(9, firstGoal.priority)

        val secondGoal = goals[1]
        assertEquals(greetedFact, secondGoal.name)
        assertEquals(8, secondGoal.priority)
    }


    @Test
    fun goalCondition() {
        val problem = Problem()
        val goalStr = "imply($greetedFact, $checkedInFact)"
        problem.addGoals(arrayOf(Goal(9, goalStr)))

        val goals = problem.getGoals()
        assertTrue(goals != null)
        assertEquals(1, goals!!.size)
        val firstGoal = goals[0]
        assertEquals(goalStr, firstGoal.name)
        val condition = firstGoal.condition ?: ""
        assertEquals(greetedFact, firstGoal.condition)
        assertFalse(problem.hasFact(condition))
        problem.addFact(condition)
        assertTrue(problem.hasFact(condition))
        problem.removeFact(condition)
        assertFalse(problem.hasFact(condition))
    }
}