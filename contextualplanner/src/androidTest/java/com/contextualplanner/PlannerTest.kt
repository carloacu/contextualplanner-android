package com.contextualplanner

import com.contextualplanner.types.*
import org.junit.Assert.*
import org.junit.Test
import java.lang.Thread.sleep
import com.contextualplanner.util.*
import com.contextualplanner.util.trackers.*

class PlannerTest {

    private val greetActionId = "greet-action"
    private val checkInActionId = "checkIn-action"
    private val informedAboutTheCompanyActionId = "informedAboutTheCompany-action"
    private val actionId1 = "action-1"
    private val actionId2 = "action-2"

    private val greetedFact = "greeted"
    private val checkedInFact = "checkedIn"
    private val informedAboutTheCompanyFact = "informedAboutTheCompany"
    private val factA = "factA"
    private val factB = "factB"

    @Test
    fun directLinkFromGoalToEffect() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, ""))
        actions.add(Action(checkInActionId, "", "", checkedInFact, ""))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(checkedInFact))))
        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(checkInActionId, oneStepOfPlannerResult!!.actionId)
        assertEquals(checkedInFact, oneStepOfPlannerResult.fromGoal.goal.fact)
        assertEquals(10, oneStepOfPlannerResult.fromGoal.priority)
    }


    @Test
    fun preconditionResolution() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, ""))
        actions.add(Action(checkInActionId, "", greetedFact, checkedInFact, ""))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(checkedInFact))))
        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(greetActionId, oneStepOfPlannerResult!!.actionId)
        problem.notifyActionDone(oneStepOfPlannerResult, domain)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain)?.actionId)
    }

    @Test
    fun goalPriority() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, ""))
        actions.add(Action(checkInActionId, "", "", checkedInFact, ""))
        actions.add(Action(informedAboutTheCompanyActionId, "", "", informedAboutTheCompanyFact, ""))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(checkedInFact))))
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(greetedFact))))
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(informedAboutTheCompanyFact))))

        assertEquals(greetActionId, lookForAnActionToDo(problem, domain)?.actionId)
    }

    @Test
    fun goalStackable() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, ""))
        actions.add(Action(checkInActionId, "", "", checkedInFact, ""))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(checkedInFact, maxTimeToKeepInactive = -1)))) // maxTimeToKeepInactive = -1 means stackable
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(greetedFact))))
        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(greetActionId, oneStepOfPlannerResult!!.actionId)
        problem.notifyActionDone(oneStepOfPlannerResult, domain)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain)?.actionId)
    }

    @Test
    fun goalStackableWithCondition() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, ""))
        actions.add(Action(checkInActionId, "", "", checkedInFact, ""))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(checkedInFact, maxTimeToKeepInactive = -1)))) // maxTimeToKeepInactive = -1 means stackable
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal("persist(imply($checkedInFact, $greetedFact))"))))
        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(checkInActionId, oneStepOfPlannerResult!!.actionId)
        problem.notifyActionDone(oneStepOfPlannerResult, domain)
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain)?.actionId)
    }

    @Test
    fun removeAGoal() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, ""))
        actions.add(Action(checkInActionId, "", "", checkedInFact, ""))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        val greetGoalGroupId = "greetGoalGroupId"
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(checkedInFact))))
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(greetedFact, groupId = greetGoalGroupId))))
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain)?.actionId)
        assertEquals(greetActionId, lookForAnActionToDo(problem, domain)?.actionId)
        problem.removeGoals(greetGoalGroupId)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain)?.actionId)
    }


    @Test
    fun goalNotStackable() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, ""))
        actions.add(Action(checkInActionId, "", "", checkedInFact, ""))
        actions.add(Action(informedAboutTheCompanyActionId, "", "", informedAboutTheCompanyFact, ""))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(greetedFact))))
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(informedAboutTheCompanyFact))))
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(checkedInFact, maxTimeToKeepInactive = 0)))) // maxTimeToKeepInactive = 0 means not stackable
        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(greetActionId, oneStepOfPlannerResult!!.actionId)
        problem.notifyActionDone(oneStepOfPlannerResult, domain)
        val oneStepOfPlannerResult2 = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult2)
        assertEquals(informedAboutTheCompanyActionId, oneStepOfPlannerResult2!!.actionId)
        problem.notifyActionDone(oneStepOfPlannerResult2, domain)
        assertNull(lookForAnActionToDo(problem, domain))
    }


    @Test
    fun goalInactiveLessThanTheMaxTimeToKeepInactive() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, ""))
        actions.add(Action(checkInActionId, "", "", checkedInFact, ""))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(checkedInFact, maxTimeToKeepInactive = 10))))
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(greetedFact))))
        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(greetActionId, oneStepOfPlannerResult!!.actionId)
        problem.notifyActionDone(oneStepOfPlannerResult, domain)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain)?.actionId)
    }


    @Test
    fun goalInactiveMoreThanTheMaxTimeToKeepInactive() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, ""))
        actions.add(Action(checkInActionId, "", "", checkedInFact, ""))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(checkedInFact, maxTimeToKeepInactive = 1))))
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(greetedFact))))
        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(greetActionId, oneStepOfPlannerResult!!.actionId)
        sleep(2000)
        problem.notifyActionDone(oneStepOfPlannerResult, domain)
        assertNull(lookForAnActionToDo(problem, domain))
    }


    @Test
    fun getGoals() {
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(checkedInFact, maxTimeToKeepInactive = 1))))
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(greetedFact))))
        val goals = problem.getGoals()
        assertTrue(goals != null)
        assertEquals(2, goals!!.size)
        val firstGoal = goals[0]
        assertEquals(greetedFact, firstGoal.goal.fact)
        assertEquals(10, firstGoal.priority)
        assertEquals(-1, firstGoal.goal.maxTimeToKeepInactive)

        val secondGoal = goals[1]
        assertEquals(checkedInFact, secondGoal.goal.fact)
        assertEquals(9, secondGoal.priority)
        assertEquals(1, secondGoal.goal.maxTimeToKeepInactive)
    }

    @Test
    fun setGoalPriority() {
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(checkedInFact))))
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(greetedFact))))
        problem.setGoalPriority(greetedFact, 8, true)

        val goals = problem.getGoals()
        assertTrue(goals != null)
        assertEquals(2, goals!!.size)
        val firstGoal = goals[0]
        assertEquals(checkedInFact, firstGoal.goal.fact)
        assertEquals(9, firstGoal.priority)

        val secondGoal = goals[1]
        assertEquals(greetedFact, secondGoal.goal.fact)
        assertEquals(8, secondGoal.priority)
    }


    @Test
    fun goalCondition() {
        val problem = Problem()
        val goalStr = "imply($greetedFact, $checkedInFact)"
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(goalStr))))

        val goals = problem.getGoals()
        assertTrue(goals != null)
        assertEquals(1, goals!!.size)
        val firstGoal = goals[0]
        assertEquals(goalStr, firstGoal.goal.fact)
        val condition = firstGoal.goal.condition ?: ""
        assertEquals(greetedFact, firstGoal.goal.condition)
        assertFalse(problem.hasFact(condition))

        val actions = mutableListOf<Action>()
        actions.add(Action(checkInActionId, "", "", checkedInFact, ""))
        val domain = Domain(actions.toTypedArray())
        assertNull(lookForAnActionToDo(problem, domain))
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
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(goalStr))))

        val actions = mutableListOf<Action>()
        actions.add(Action(checkInActionId, "", "", checkedInFact, ""))
        val domain = Domain(actions.toTypedArray())
        assertNull(lookForAnActionToDo(problem, domain))
        assertEquals(1, problem.getGoals()!!.size)
        problem.addFact(greetedFact)
        assertEquals(checkInActionId, lookForAnActionToDo(problem, domain)?.actionId)
        problem.addFact(checkedInFact)
        assertNull(lookForAnActionToDo(problem, domain))
        assertEquals(1, problem.getGoals()!!.size)
    }

    @Test
    fun goalsRemovedTracker() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, ""))
        actions.add(Action(checkInActionId, "", "", checkedInFact, ""))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        val goalsRemovedTracker = GoalsRemovedTracker(problem)
        val goalsRemovedTracker2 = GoalsRemovedTracker(problem)
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(checkedInFact))))
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(greetedFact))))
        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(greetActionId, oneStepOfPlannerResult!!.actionId)
        assertEquals(0, goalsRemovedTracker.flushGoalsRemoved().size)
        assertEquals(0, goalsRemovedTracker2.flushGoalsRemoved().size)
        problem.notifyActionDone(oneStepOfPlannerResult, domain)
        var goalsRemoved = goalsRemovedTracker.flushGoalsRemoved()
        assertEquals(1, goalsRemoved.size)
        assertEquals(greetedFact, goalsRemoved[0])

        val oneStepOfPlannerResult2 = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult2)
        assertEquals(checkInActionId, oneStepOfPlannerResult2!!.actionId)
        assertEquals(0, goalsRemovedTracker.flushGoalsRemoved().size)
        problem.notifyActionDone(oneStepOfPlannerResult2, domain)

        goalsRemoved = goalsRemovedTracker.flushGoalsRemoved()
        assertEquals(1, goalsRemoved.size)
        assertEquals(checkedInFact, goalsRemoved[0])

        goalsRemoved = goalsRemovedTracker2.flushGoalsRemoved()
        assertEquals(2, goalsRemoved.size)
        assertEquals(checkedInFact, goalsRemoved[0])
        assertEquals(greetedFact, goalsRemoved[1])

        assertNull(lookForAnActionToDo(problem, domain))

        goalsRemovedTracker2.dispose()
        goalsRemovedTracker.dispose()
    }


    @Test
    fun factsAddedCache() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", greetedFact, ""))
        val punctualFact1 = getPunctualFactPrefix() + "fact1"
        actions.add(Action(checkInActionId, "", "", "$checkedInFact & $punctualFact1", ""))
        val domain = Domain(actions.toTypedArray())
        val problem = Problem()
        problem.addFact(informedAboutTheCompanyFact)
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(checkedInFact))))
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(greetedFact))))

        var punctualFacts = problem.flushPunctualFacts()
        var factsAdded = problem.flushFactsAdded()
        assertEquals(1, factsAdded.size)
        assertEquals(informedAboutTheCompanyFact, factsAdded[0])

        assertEquals(0, punctualFacts.size)

        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(greetActionId, oneStepOfPlannerResult!!.actionId)
        problem.notifyActionDone(oneStepOfPlannerResult, domain)

        factsAdded = problem.flushFactsAdded()
        assertEquals(1, factsAdded.size)
        assertEquals(greetedFact, factsAdded[0])

        val oneStepOfPlannerResult2 = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult2)
        assertEquals(checkInActionId, oneStepOfPlannerResult2!!.actionId)
        problem.notifyActionDone(oneStepOfPlannerResult2, domain)

        punctualFacts = problem.flushPunctualFacts()
        assertEquals(1, punctualFacts.size)
        assertEquals(punctualFact1, punctualFacts[0])

        factsAdded = problem.flushFactsAdded()
        assertEquals(1, factsAdded.size)
        assertEquals(checkedInFact, factsAdded[0])

        assertEquals(0, problem.flushPunctualFacts().size)
        assertEquals(0, problem.flushFactsAdded().size)
        assertNull(lookForAnActionToDo(problem, domain))

        assertEquals(0, problem.flushPunctualFacts().size)
        assertEquals(0, problem.flushFactsAdded().size)
    }


    @Test
    fun persistGoalReprint() {
        val problem = Problem()
        val goalStr = "persist(imply($greetedFact, $checkedInFact))"
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal(goalStr))))
        val goals = problem.getGoals()
        assertTrue(goals != null)
        assertEquals(1, goals!!.size)
        assertEquals(goalStr, goals[0].goal.fact)
    }


    @Test
    fun checkInference() {
        val punctualFactToto = getPunctualFactPrefix() + "toto"
        val problem = Problem()
        val setOfInferences = SetOfInferences()
        problem.addSetOfInferences(setOfInferences)
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal( "persist($factA)"))))

        val actions = mutableListOf<Action>()
        actions.add(Action(actionId1, "", factB, punctualFactToto, ""))
        problem.addFact(factB)

        val domain = Domain(actions.toTypedArray())
        assertNull(lookForAnActionToDo(problem, domain))
        val inference1Id = "inference1"
        setOfInferences.addInference(Inference(inference1Id, punctualFactToto, factA))
        assertEquals(actionId1, lookForAnActionToDo(problem, domain)?.actionId)
        setOfInferences.removeInference(inference1Id)
        assertNull(lookForAnActionToDo(problem, domain))
    }


    @Test
    fun changePunctualPrefixName() {
        val newPunctualPrefix = "~NewPunctualPrefixName~"
        setPunctualFactPrefix(newPunctualPrefix)
        assertEquals(newPunctualPrefix, getPunctualFactPrefix())
        val problem = Problem()
        val punctualFactToto = newPunctualPrefix + "toto"
        problem.addFact(punctualFactToto)
        assertFalse(problem.hasFact(punctualFactToto))
        val notPunctualFactToto = "totoFact"
        problem.addFact(notPunctualFactToto)
        assertTrue(problem.hasFact(notPunctualFactToto))
    }

    @Test
    fun checkOneStepTowards() {
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(9, Goal("oneStepTowards($factA)"))))
        problem.addGoals(arrayOf(GoalWithPriority(8, Goal(factB))))

        val actions = mutableListOf<Action>()
        actions.add(Action(actionId1, potentialEffect = factA))
        actions.add(Action(actionId2, effect = factB))

        val domain = Domain(actions.toTypedArray())
        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(actionId1, oneStepOfPlannerResult!!.actionId)
        problem.notifyActionDone(oneStepOfPlannerResult, domain)

        assertEquals(actionId2, lookForAnActionToDo(problem, domain)?.actionId)
    }
}