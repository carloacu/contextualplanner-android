package com.contextualplanner

import java.lang.RuntimeException

enum class BehaviorSequentialSuccession {
    ON_ERROR,
    ON_SUCCESS,
    ALWAYS
}

data class NextBehavior(
    val succession: BehaviorSequentialSuccession,
    val taskTree: TaskTree
)

data class TaskSignature(
    var taskName: String = "",
    var parameters: List<String> = listOf()
) {
    fun toStr(): String {
        var res = taskName
        if (parameters.isNotEmpty()) {
            res += "("
            var firstIteration = true
            parameters.forEach {
                if (firstIteration)
                    firstIteration = false
                else
                    res += ", "
                res += it
            }
            res += ")"
        }
        return res
    }

}

class TaskTree(
    var parent: TaskTree? = null,
    var taskSignature: TaskSignature? = null
) {
    companion object {
        init {
            ensureInitialized()
        }
    }

    val behaviorsToRunInParallel = mutableListOf<TaskTree>()
    val behaviorsToRunSequentially = mutableListOf<NextBehavior>()

    fun isEmpty(): Boolean {
        return taskSignature == null && behaviorsToRunInParallel.isEmpty() && behaviorsToRunSequentially.isEmpty()
    }

    fun getTaskSignatures(): Set<TaskSignature> {
        val res = mutableSetOf<TaskSignature>()
        taskSignature?.let { res.add(it) }
        behaviorsToRunInParallel.forEach {
            res.addAll(it.getTaskSignatures())
        }
        behaviorsToRunSequentially.forEach {
            res.addAll(it.taskTree.getTaskSignatures())
        }
        return res
    }
}

private val endOfCommandName = " ()".toCharArray()

fun strToTaskTree(input: String): TaskTree {

    var currentSuccession: BehaviorSequentialSuccession? = null
    val res = TaskTree()
    var currentBehavior = res
    var i = 0
    while (i < input.length) {
        val c = input[i]
        if (c == ' ') {
            ++i
            continue
        }
        if (c == '(') {
            val subBehavior = TaskTree(currentBehavior)
            if (currentSuccession != null) {
                currentBehavior.behaviorsToRunSequentially.add(
                    NextBehavior(
                        currentSuccession,
                        subBehavior
                    )
                )
            } else {
                currentBehavior.behaviorsToRunInParallel.add(subBehavior)
            }
            currentBehavior = subBehavior
            currentSuccession = null
            ++i
            continue
        }
        if (c == ')') {
            currentBehavior = currentBehavior.parent
                ?: throw RuntimeException("Unexpected end of parenthesis")
            currentSuccession = null
            ++i
            continue
        }
        if (c == '>') {
            currentSuccession = BehaviorSequentialSuccession.ON_SUCCESS
            ++i
            if (input[i] == 'e') {
                currentSuccession = BehaviorSequentialSuccession.ON_ERROR
                ++i
            } else if (input[i] == '!') {
                currentSuccession = BehaviorSequentialSuccession.ALWAYS
                ++i
            }
            continue
        }
        if (c == '|') {
            currentSuccession = null
            ++i
            continue
        }

        val addCommand = { taskSignature: TaskSignature ->
            if (currentBehavior.isEmpty()) {
                currentBehavior.taskSignature = taskSignature
            } else {
                val subBehavior = TaskTree(currentBehavior, taskSignature)
                if (currentSuccession != null) {
                    currentBehavior.behaviorsToRunSequentially.add(
                        NextBehavior(
                            currentSuccession!!,
                            subBehavior
                        )
                    )
                } else {
                    currentBehavior.behaviorsToRunInParallel.add(subBehavior)
                }
            }
        }

        val endOfCommandName = input.indexOfAny(endOfCommandName, i)
        if (endOfCommandName == -1) {
            addCommand(TaskSignature(input.substring(i, input.length)))
            return res
        }

        val commandName = input.substring(i, endOfCommandName)
        addCommand(
            if (input[endOfCommandName] == '(') {
                val beginOfParameters = endOfCommandName + 1
                val endOfParameters = input.indexOf(')', beginOfParameters)
                if (endOfParameters == -1)
                    throw RuntimeException("End of parameters not found in $input")
                val parameters = input.substring(beginOfParameters, endOfParameters).split(',').map {
                    it.trim()
                }
                i = endOfParameters + 1
                TaskSignature(commandName, parameters)
            } else {
                i = endOfCommandName
                TaskSignature(commandName)
            }
        )
        currentSuccession = null
    }
    return res
}