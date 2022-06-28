package com.contextualplanner

data class Goal(
    val priority: Int,
    val name: String,
    val stackable: Boolean = true,
    val condition: String? = null,
    val maxTimeToKeepInactive: Int = -1,
    val groupId: String = ""
)
