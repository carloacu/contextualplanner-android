package com.contextualplanner

data class Goal(
    val priority: Int,
    val name: String,
    val stackable: Boolean = true,
    val maxTimeToKeepInactive: Int = -1,
    val groupId: String = ""
)
