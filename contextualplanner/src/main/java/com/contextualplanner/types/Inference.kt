package com.contextualplanner.types


/**
 * @brief Axiomatic inference (= deduction) that the bot can do.
 * @param id Identifier of the inference.
 * @param condition Set of facts serialized in string that should be present in the world to trigger this inference.
 * @param factsToModify Fact modifications declared and that will be applied to the world.
 * @param goalsToAdd Goals to add in the world.
 */
data class Inference(
    val id: String,
    val condition: String,
    val factsToModify: String,
    val goalsToAdd: Array<Goal> = arrayOf()
)
