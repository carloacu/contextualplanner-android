package com.contextualplanner.util

import com.contextualplanner.util.detail.ensureInitialized


private external fun getPunctualFactPrefixCpp(): String
private external fun setPunctualFactPrefixCpp(punctualPrefix: String)

/**
 * Prefix that a fact should have to be considered as a punctual fact.<br/>
 * A punctual fact is a fact that is never stored in the world state.
 */
fun getPunctualFactPrefix(): String {
    ensureInitialized()
    return getPunctualFactPrefixCpp()
}


/**
 * Set the prefix that a fact should have to be considered as a punctual fact.<br/>
 * A punctual fact is a fact that is never stored in the world state.
 */
fun setPunctualFactPrefix(punctualPrefix: String) {
    ensureInitialized()
    return setPunctualFactPrefixCpp(punctualPrefix)
}