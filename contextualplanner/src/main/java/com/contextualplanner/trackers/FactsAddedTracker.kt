package com.contextualplanner.trackers

import com.contextualplanner.DisposableWithId
import com.contextualplanner.Problem
import com.contextualplanner.ensureInitialized


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