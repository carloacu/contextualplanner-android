package com.contextualplanner.types

/**
 * @brief A characteristic that the world should have. It is the motivation of the bot for doing actions to respect this characteristic of the world.
 * @param fact Serialized string corresponding to the fact contained in this goal.
 * @param condition Condition associated, it is a fact serialized in string that should be present in the world to enable this goal.
 * @param maxTimeToKeepInactive The maximum time that we allow for this goal to be inactive.<b/>
 * A negative value means that the time is infinite.
 * @param groupId Group identifier of this goal. It can be empty if the goal does not belong to a group.
 */
data class Goal(
    val fact: String,
    val condition: String? = null,
    val maxTimeToKeepInactive: Int = -1,
    val groupId: String = ""
)
