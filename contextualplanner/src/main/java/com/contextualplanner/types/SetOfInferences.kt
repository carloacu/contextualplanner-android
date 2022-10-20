package com.contextualplanner.types

import com.contextualplanner.types.detail.DisposableWithId
import com.contextualplanner.util.detail.ensureInitialized


/// Container of a set of inferences.
class SetOfInferences : DisposableWithId(newSetOfInferences()) {

    companion object {
        // Be sure that the C++ library is loaded.
        init {
            ensureInitialized()
        }

        /**
         * @brief Construct the set of inferences.
         * @return Identifier of the object.
         */
        private external fun newSetOfInferences(): Int
    }

    /**
     * @brief Add an inference.
     * @param inference Inference to add.
     */
    external fun addInference(inference: Inference)

    /**
     * @brief Remove an inference.
     * @param inferenceId Identifier of the inference to remove.
     */
    external fun removeInference(inferenceId: String)

    /**
     * @brief What should be done to release this object.<br/>
     * /!\ You should never call this function but dispose() instead to release this object.
     */
    external override fun disposeImplementation()
}
