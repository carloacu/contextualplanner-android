package com.contextualplanner


data class Action(
    val id: String,
    val triggers: List<String>,
    val taskTrees: List<TaskTree>,
    val recommendations: List<String>,
    val preferInContext: String,
    val precondition: String,
    val effect: String,
    val potentialEffect: String,
    val goalsToAdd: Array<String>,
    val replanifyDirectlyAfter: Boolean
)


class Domain(
    private val actions: Map<String, Action>,
    val inform: List<String> = listOf()
) : DisposableWithId(newDomain(actions.values.toTypedArray())) {

    companion object {
        init {
            ensureInitialized()
        }
    }

    fun getAction(key: String): Action? {
        return actions[key]
    }

    fun forEachAction(callback: (action: Action) -> Unit) {
        actions.forEach { callback(it.value) }
    }

    fun addRuntimeTask(runtimeTaskId: String) {
        val newAction = Action(runtimeTaskId, listOf(), listOf(), listOf(),
            "", "", runtimeTaskId, "", arrayOf(), false)
        addAction(id, newAction)
    }

    fun removeRuntimeTask(runtimeTaskId: String) {
        removeAction(id, runtimeTaskId)
    }

    fun printActions(): String {
        return printActions(id)
    }

    override fun disposeImplementation(id: Int) {
        deleteDomain(id)
    }
}


private external fun newDomain(actions: Array<Action>): Int

private external fun addAction(domainId: Int, action: Action)

private external fun removeAction(domainId: Int, actionId: String)

private external fun printActions(domainId: Int): String

private external fun deleteDomain(domainId: Int)
