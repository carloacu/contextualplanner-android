package com.contextualplanner.types

import com.contextualplanner.types.detail.DisposableWithId
import com.contextualplanner.util.detail.ensureInitialized


/**
 * @brief Set of all the actions that the bot can do.
 * @param actions Initial set of all the actions.
 */
class Domain(
    actions: Array<Action>
) : DisposableWithId(newDomain(actions)) {

    companion object {
        // Be sure that the C++ library is loaded.
        init {
            ensureInitialized()
        }

        /**
         * @brief Construct a domain.
         * @param[in] actions Initial set of all the actions.
         * @return Identifier of the object.
         */
        private external fun newDomain(actions: Array<Action>): Int
    }

    /**
     * @brief Add an action.
     * @param action Action to add.
     */
    external fun addAction(action: Action)

    /**
     * @brief Remove an action.
     * @param actionId Identifier of the action to remove.
     */
    external fun removeAction(actionId: String)

    /**
     * @brief What should be done to release this object.<br/>
     * /!\ You should never call this function but dispose() instead to release this object.
     */
    external override fun disposeImplementation()
}





