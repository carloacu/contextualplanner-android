# Contextual Planner Android


## Description

This is Kotlin library to do plannification adapted for social context.<br/>
It is wrapper of a C++ implementation available here: https://github.com/carloacu/contextualplanner

To read the description of this library, please go here: https://github.com/carloacu/contextualplanner


## To use the library in your project

### Step 1. Add the maven repository to your build file
Add it in your root build.gradle at the end of repositories:
```Kotlin
allprojects {
    repositories {
        ...
        maven { url 'https://raw.github.com/carloacu/contextualplanner-android-releases/master' }
    }
}
```

### Step 2. Add the dependency
```Kotlin
dependencies {
    implementation 'com.github.carloacu:contextualplanner-android:1.1.15'
}
```


## Code documentation

[Here](contextualplanner/src/main/java/com/contextualplanner/ContextualPlanner.kt) are the documented headers of the main function.

### Types

Here are the types providec by this library:

* [Action](contextualplanner/src/main/java/com/contextualplanner/types/Action.kt): Axiomatic thing that the bot can do.
* [Domain](contextualplanner/src/main/java/com/contextualplanner/types/Domain.kt): Set of all the actions that the bot can do.
* [Goal](contextualplanner/src/main/java/com/contextualplanner/types/Goal.kt): A characteristic that the world should have. It is the motivation of the bot for doing actions to respect this characteristic of the world.
* [Problem](contextualplanner/src/main/java/com/contextualplanner/types/Problem.kt): Current world, goal for the world and historical of actions done.



## Examples of usage

Here is an example with only one action to do:

```kotlin
        // Fact
        val sayHiActionId = "say_hi"

        // Action identifier
        val userIsGreetedFact = "user_is_greeted"

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
```


Here is an example with two actions to do and with the usage of preconditions:


```kotlin
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
```
