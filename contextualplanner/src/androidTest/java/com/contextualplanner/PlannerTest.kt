package com.contextualplanner

import org.junit.Test
import org.junit.Assert.assertEquals


class PlannerTest {

    private val greetActionId = "greet"
    private val checkInActionId = "checkIn"

    @Test
    fun directLinkFromGoalTOEffect() {
        val actions = mutableListOf<Action>()
        actions.add(Action(greetActionId, "", "", "greeted", "", arrayOf()))
        actions.add(Action(checkInActionId, "", "", "checkedIn", "", arrayOf()))

        val domain = Domain(actions.toTypedArray())

        val problem = Problem()
        problem.addGoals(arrayOf("greeted"))

        assertEquals(greetActionId, lookForAnActionToDo(problem, domain))
    }

}
