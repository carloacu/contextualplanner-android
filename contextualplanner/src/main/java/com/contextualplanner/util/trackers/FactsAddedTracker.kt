package com.contextualplanner.util.trackers

import com.contextualplanner.types.detail.DisposableWithId
import com.contextualplanner.types.Problem
import com.contextualplanner.util.detail.ensureInitialized


class FactsAddedTracker(problem: Problem) : DisposableWithId(newFactsAddedTracker(problem)) {

    companion object {
        init {
            ensureInitialized()
        }
        private external fun newFactsAddedTracker(problem: Problem): Int
    }

    external fun flushFactsAdded(): Array<String>
    external override fun disposeImplementation()
}