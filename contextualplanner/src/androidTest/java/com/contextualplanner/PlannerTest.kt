package com.contextualplanner

import com.contextualplanner.types.Action
import com.contextualplanner.types.Domain
import com.contextualplanner.types.Goal
import com.contextualplanner.types.Problem
import org.junit.Assert.*
import org.junit.Test
import java.lang.Thread.sleep
import com.contextualplanner.util.trackers.*

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
        assertEquals(checkedInFact, actionAndGoal.goal.fact)
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
        problem.notifyActionDone(greetActionId, domain)
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
        problem.addGoals(arrayOf(Goal(9, checkedInFact, maxTimeToKeepInactive = -1))) // maxTimeToKeepInactive = -1 means stackable
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        problem.notifyActionDone(greetActionId, domain)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain).actionId)
    }

    @Test
    fun removeAGoal() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        val greetGoalGroupId = "greetGoalGroupId"
        problem.addGoals(arrayOf(Goal(9, checkedInFact)))
        problem.addGoals(arrayOf(Goal(10, greetedFact, groupId = greetGoalGroupId)))
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        problem.removeGoals(greetGoalGroupId)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain).actionId)
    }


    @Test
    fun goalNotStackable() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(9, checkedInFact, maxTimeToKeepInactive = 0))) // maxTimeToKeepInactive = 0 means not stackable
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        problem.notifyActionDone(greetActionId, domain)
        assertEquals("", lookForAnActionToDo(problem, domain).actionId)
    }


    @Test
    fun goalInactiveLessThanTheMaxTimeToKeepInactive() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(9, checkedInFact, maxTimeToKeepInactive = 10)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        problem.notifyActionDone(greetActionId, domain)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain).actionId)
    }


    @Test
    fun goalInactiveMoreThanTheMaxTimeToKeepInactive() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(9, checkedInFact, maxTimeToKeepInactive = 1)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        problem.notifyActionDone(greetActionId, domain)
        sleep(2000)
        assertEquals("", lookForAnActionToDo(problem, domain).actionId)
    }


    @Test
    fun getGoals() {
        val problem = Problem()
        problem.addGoals(arrayOf(Goal(9, checkedInFact, maxTimeToKeepInactive = 1)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        val goals = problem.getGoals()
        assertTrue(goals != null)
        assertEquals(2, goals!!.size)
        val firstGoal = goals[0]
        assertEquals(greetedFact, firstGoal.fact)
        assertEquals(10, firstGoal.priority)
        assertEquals(-1, firstGoal.maxTimeToKeepInactive)

        val secondGoal = goals[1]
        assertEquals(checkedInFact, secondGoal.fact)
        assertEquals(9, secondGoal.priority)
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
        assertEquals(checkedInFact, firstGoal.fact)
        assertEquals(9, firstGoal.priority)

        val secondGoal = goals[1]
        assertEquals(greetedFact, secondGoal.fact)
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
        assertEquals(goalStr, firstGoal.fact)
        val condition = firstGoal.condition ?: ""
        assertEquals(greetedFact, firstGoal.condition)
        assertFalse(problem.hasFact(condition))

        val actions = mutableListOf<Action>()
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        assertEquals("", lookForAnActionToDo(problem, domain).actionId)
        assertEquals(0, problem.getGoals()!!.size) // goal remove because it is not persistent

        problem.addFact(condition)
        assertTrue(problem.hasFact(condition))
        problem.removeFact(condition)
        assertFalse(problem.hasFact(condition))
    }


    @Test
    fun persistImplication() {
        val problem = Problem()
        val goalStr = "persist(imply($greetedFact, $checkedInFact))"
        problem.addGoals(arrayOf(Goal(9, goalStr)))

        val actions = mutableListOf<Action>()
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        assertEquals("", lookForAnActionToDo(problem, domain).actionId)
        assertEquals(1, problem.getGoals()!!.size)
        problem.addFact(greetedFact)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain).actionId)
        problem.addFact(checkedInFact)
        assertEquals("", lookForAnActionToDo(problem, domain).actionId)
        assertEquals(1, problem.getGoals()!!.size)
    }

    @Test
    fun goalsRemovedTracker() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        val goalsRemovedTracker = GoalsRemovedTracker(problem)
        val goalsRemovedTracker2 = GoalsRemovedTracker(problem)
        assertEquals(0, goalsRemovedTracker.flushGoalsRemoved().size)
        problem.addGoals(arrayOf(Goal(9, checkedInFact)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))
        assertEquals(0, goalsRemovedTracker.flushGoalsRemoved().size)
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        problem.notifyActionDone(greetActionId, domain)
        assertEquals(0, goalsRemovedTracker.flushGoalsRemoved().size)
        assertEquals(0, goalsRemovedTracker2.flushGoalsRemoved().size)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain).actionId)
        problem.notifyActionDone(checkInActionId, domain)

        var goalsRemoved = goalsRemovedTracker.flushGoalsRemoved()
        assertEquals(1, goalsRemoved.size)
        assertEquals(greetedFact, goalsRemoved[0])

        assertEquals(0, goalsRemovedTracker.flushGoalsRemoved().size)
        assertEquals("", lookForAnActionToDo(problem, domain).actionId)

        goalsRemoved = goalsRemovedTracker.flushGoalsRemoved()
        assertEquals(1, goalsRemoved.size)
        assertEquals(checkedInFact, goalsRemoved[0])

        goalsRemoved = goalsRemovedTracker2.flushGoalsRemoved()
        assertEquals(2, goalsRemoved.size)
        assertEquals(checkedInFact, goalsRemoved[0])
        assertEquals(greetedFact, goalsRemoved[1])

        goalsRemovedTracker2.dispose()
        goalsRemovedTracker.dispose()
    }


    @Test
    fun factsAddedTracker() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", checkedInFact, "", arrayOf()))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        val factsAddedTracker = FactsAddedTracker(problem)
        val factsAddedTracker2 = FactsAddedTracker(problem)
        problem.addFact(informedAboutTheCompanyFact)
        val factsAddedTracker3 = FactsAddedTracker(problem)
        problem.addGoals(arrayOf(Goal(9, checkedInFact)))
        problem.addGoals(arrayOf(Goal(10, greetedFact)))

        var factsAdded = factsAddedTracker.flushFactsAdded()
        assertEquals(1, factsAdded.size)
        assertEquals(informedAboutTheCompanyFact, factsAdded[0])

        assertEquals(greetActionId, lookForAnActionToDo(problem, domain).actionId)
        problem.notifyActionDone(greetActionId, domain)

        factsAdded = factsAddedTracker.flushFactsAdded()
        assertEquals(1, factsAdded.size)
        assertEquals(greetedFact, factsAdded[0])

        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain).actionId)
        problem.notifyActionDone(checkInActionId, domain)

        factsAdded = factsAddedTracker.flushFactsAdded()
        assertEquals(1, factsAdded.size)
        assertEquals(checkedInFact, factsAdded[0])

        assertEquals(0, factsAddedTracker.flushFactsAdded().size)
        assertEquals("", lookForAnActionToDo(problem, domain).actionId)

        assertEquals(0, factsAddedTracker.flushFactsAdded().size)

        factsAdded = factsAddedTracker2.flushFactsAdded()
        assertEquals(3, factsAdded.size)
        assertEquals(checkedInFact, factsAdded[0])
        assertEquals(greetedFact, factsAdded[1])
        assertEquals(informedAboutTheCompanyFact, factsAdded[2])

        problem.removeFact(checkedInFact)

        factsAdded = factsAddedTracker3.flushFactsAdded()
        assertEquals(1, factsAdded.size)
        assertEquals(greetedFact, factsAdded[0])

        factsAddedTracker3.dispose()
        factsAddedTracker2.dispose()
        factsAddedTracker.dispose()
    }


    @Test
    fun persistGoalReprint() {
        val problem = Problem()
        val goalStr = "persist(imply($greetedFact, $checkedInFact))"
        problem.addGoals(arrayOf(Goal(9, goalStr)))
        val goals = problem.getGoals()
        assertTrue(goals != null)
        assertEquals(1, goals!!.size)
        assertEquals(goalStr, goals[0].fact)
    }


}