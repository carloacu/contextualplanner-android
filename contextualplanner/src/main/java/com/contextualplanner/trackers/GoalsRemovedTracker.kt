package com.contextualplanner.trackers

import com.contextualplanner.DisposableWithId
import com.contextualplanner.Problem
import com.contextualplanner.ensureInitialized


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