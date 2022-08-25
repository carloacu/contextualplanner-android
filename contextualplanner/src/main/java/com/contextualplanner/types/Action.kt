package com.contextualplanner.types


/**
 * @brief Axiomatic thing that the bot can do.
 * @param id Identifier of the action.
 * @param preferInContext Set of facts serialized in string that will increase the priority of this action if they are present in the world.
 * @param precondition Set of facts serialized in string that should be present in the world to be able to do this action.
 * @param effect Fact modifications declared and that will be applied to the world.
 * @param potentialEffect Fact modifications declared but that will not be applied to the world.
 * @param goalsToAdd Goals to add in the world.
 */
data class Action(
    val id: String,
    val preferInContext: String = "",
    val precondition: String = "",
    val effect: String = "",
    val potentialEffect: String = "",
    val goalsToAdd: Array<Goal> = arrayOf()
)
