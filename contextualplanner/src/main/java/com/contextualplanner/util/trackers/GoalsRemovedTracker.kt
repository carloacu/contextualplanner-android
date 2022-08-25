package com.contextualplanner.util.trackers

import com.contextualplanner.types.Problem
import com.contextualplanner.types.detail.DisposableWithId
import com.contextualplanner.util.detail.ensureInitialized


class GoalsRemovedTracker(problem: Problem) : DisposableWithId(newGoalsRemovedTracker(problem)) {

    companion object {
        init {
            ensureInitialized()
        }
        private external fun newGoalsRemovedTracker(problem: Problem): Int
    }

    external fun flushGoalsRemoved(): Array<String>
    external override fun disposeImplementation()
}