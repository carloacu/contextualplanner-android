package com.contextualplanner.util

import com.contextualplanner.util.detail.ensureInitialized


private external fun getPunctualFactPrefixCpp(): String

/**
 * Prefix that a fact should have to be considered as a punctual fact.<br/>
 * A puntual fact is a fact that is never stored in the world state.
 */
fun getPunctualFactPrefix(): String {
    ensureInitialized()
    return getPunctualFactPrefixCpp()
}