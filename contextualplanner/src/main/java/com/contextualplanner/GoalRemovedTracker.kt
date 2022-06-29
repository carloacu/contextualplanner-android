package com.contextualplanner


class GoalRemovedTracker(problem: Problem) : DisposableWithId(newGoalRemovedTracker(problem)) {

    companion object {
        init {
            ensureInitialized()
        }
        private external fun newGoalRemovedTracker(problem: Problem): Int
    }

    external fun flushGoalRemoved(): Array<String>
    external override fun disposeImplementation()
}