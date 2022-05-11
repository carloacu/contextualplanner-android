package com.contextualplanner


class Domain(
    actions: Array<Action>
) : DisposableWithId(newDomain(actions)) {

    companion object {
        init {
            ensureInitialized()
        }

        private external fun newDomain(actions: Array<Action>): Int
    }

    external fun addAction(action: Action)

    external fun removeAction(actionId: String)

    external fun printActions(): String

    external override fun disposeImplementation()
}





