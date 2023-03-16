package com.contextualplanner

import com.contextualplanner.types.*
import org.junit.Assert.*
import org.junit.Test

class DocExamplesTest {

    @Test
    fun planningDummyExampleTest() {

        // Fact
        val userIsGreetedFact = "user_is_greeted"

        // Action identifier
        val sayHiActionId = "say_hi"

        // Initialize the domain with an action
        val actions = mutableListOf<Action>()
        actions.add(Action(sayHiActionId, effect = userIsGreetedFact))
        val domain = Domain(actions.toTypedArray())

        // Initialize the problem with the goal to satisfy
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(userIsGreetedFact))))

        // Look for an action to do
        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(sayHiActionId, oneStepOfPlannerResult!!.actionId)
        notifyActionDone(oneStepOfPlannerResult, problem, domain)

        // Look for the next action to do
        assertNull(lookForAnActionToDo(problem, domain))
    }


    @Test
    fun planningExampleWithAPreconditionSolvedTest() {

        // Facts
        val userIsGreetedFact = "user_is_greeted"
        val proposedOurHelpToUser = "proposed_our_help_to_user"

        // Action identifiers
        val sayHiActionId = "say_hi"
        val askHowICanHelpActionId = "proposed_our_help_to_user"

        // Initialize the domain with an action
        val actions = mutableListOf<Action>()
        actions.add(Action(sayHiActionId, effect = userIsGreetedFact))
        actions.add(Action(askHowICanHelpActionId, precondition = userIsGreetedFact, effect = proposedOurHelpToUser))
        val domain = Domain(actions.toTypedArray())

        // Initialize the problem with the goal to satisfy
        val problem = Problem()
        problem.addGoals(arrayOf(GoalWithPriority(10, Goal(proposedOurHelpToUser))))

        // Look for an action to do
        val oneStepOfPlannerResult = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult)
        assertEquals(sayHiActionId, oneStepOfPlannerResult!!.actionId)
        notifyActionDone(oneStepOfPlannerResult, problem, domain)

        // Look for the next action to do
        val oneStepOfPlannerResult2 = lookForAnActionToDo(problem, domain)
        assertNotNull(oneStepOfPlannerResult2)
        assertEquals(askHowICanHelpActionId, oneStepOfPlannerResult2!!.actionId)
        notifyActionDone(oneStepOfPlannerResult2, problem, domain)

        // Look for the next action to do
        assertNull(lookForAnActionToDo(problem, domain))
    }


}