package com.contextualplanner

data class Goal(
    val priority: Int,
    val name: String,
    val condition: String? = null,
    val maxTimeToKeepInactive: Int = -1,
    val groupId: String = ""
)
