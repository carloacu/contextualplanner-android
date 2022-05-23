package com.contextualplanner

data class Action(
    val id: String,
    val preferInContext: String,
    val precondition: String,
    val effect: String,
    val potentialEffect: String,
    val goalsToAdd: Array<Goal>
)
