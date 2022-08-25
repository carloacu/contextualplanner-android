package com.contextualplanner

import com.contextualplanner.types.Action
import com.contextualplanner.types.Domain
import com.contextualplanner.types.Goal
import com.contextualplanner.types.Problem
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
        problem.addGoals(arrayOf(Goal(10, userIsGreetedFact)))

        // Look for an action to do
        val actionAndGoal = lookForAnActionToDo(problem, domain)
        assertEquals(sayHiActionId, actionAndGoal.actionId)
        notifyActionDone(actionAndGoal.actionId, problem, domain)

        // Look for the next action to do
        val actionAndGoal2 = lookForAnActionToDo(problem, domain)
        assertEquals("", actionAndGoal2.actionId)
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
        problem.addGoals(arrayOf(Goal(10, proposedOurHelpToUser)))

        // Look for an action to do
        val actionAndGoal = lookForAnActionToDo(problem, domain)
        assertEquals(sayHiActionId, actionAndGoal.actionId)
        notifyActionDone(actionAndGoal.actionId, problem, domain)

        // Look for the next action to do
        val actionAndGoal2 = lookForAnActionToDo(problem, domain)
        assertEquals(askHowICanHelpActionId, actionAndGoal2.actionId)
        notifyActionDone(actionAndGoal2.actionId, problem, domain)

        // Look for the next action to do
        val actionAndGoal3 = lookForAnActionToDo(problem, domain)
        assertEquals("", actionAndGoal3.actionId)
    }


}