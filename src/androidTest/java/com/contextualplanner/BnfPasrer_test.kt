package com.contextualplanner


import com.contextualplanner.flattenBnfRegex
import org.junit.Test
import org.junit.Assert.assertEquals



class BnfParserTest {

    init {
        System.loadLibrary("contextualplanner-jni")
    }

    @Test
    fun noRegex() {
        val iText = "je suis content"
        val outputs = flattenBnfRegex(iText)
        assertEquals(1, outputs.size)
        assertEquals(iText, outputs[0])
    }

    @Test
    fun noRegexButBeginOfParenthesis() {
        val iText = "je suis ( content"
        val outputs = flattenBnfRegex(iText)
        assertEquals(1, outputs.size)
        assertEquals("je suis  content", outputs[0])
    }

    @Test
    fun oneParenthesisOneOption() {
        val iText = "je suis ( content )"
        val outputs = flattenBnfRegex(iText)
        assertEquals(1, outputs.size)
        assertEquals("je suis  content ", outputs[0])
    }

    @Test
    fun oneParenthesisTwoOptions() {
        val iText = "je suis ( content | heureux )"
        val outputs = flattenBnfRegex(iText)
        assertEquals(2, outputs.size)
        assertEquals("je suis  content ", outputs[0])
        assertEquals("je suis  heureux ", outputs[1])
    }

    @Test
    fun oneParenthesisTwoOptions2() {
        val iText = "Raconte (moi|nous) quelque chose de drôle"
        val outputs = flattenBnfRegex(iText)
        assertEquals(2, outputs.size)
        assertEquals("Raconte moi quelque chose de drôle", outputs[0])
        assertEquals("Raconte nous quelque chose de drôle", outputs[1])
    }

    @Test
    fun oneParenthesisTwoOptionalOptions2() {
        val iText = "Raconte [moi|nous] quelque chose de drôle"
        val outputs = flattenBnfRegex(iText)
        assertEquals(3, outputs.size)
        assertEquals("Raconte  quelque chose de drôle", outputs[0])
        assertEquals("Raconte moi quelque chose de drôle", outputs[1])
        assertEquals("Raconte nous quelque chose de drôle", outputs[2])
    }

    @Test
    fun oneOptional() {
        val iText = "Raconte une [autre] blague"
        val outputs = flattenBnfRegex(iText)
        assertEquals(2, outputs.size)
        assertEquals("Raconte une  blague", outputs[0])
        assertEquals("Raconte une autre blague", outputs[1])
    }


    @Test
    fun twoDifferentOptionals() {
        val iText = "Raconte [moi] une [autre] blague"
        val outputs = flattenBnfRegex(iText)
        assertEquals(4, outputs.size)
        assertEquals("Raconte  une  blague", outputs[0])
        assertEquals("Raconte  une autre blague", outputs[1])
        assertEquals("Raconte moi une  blague", outputs[2])
        assertEquals("Raconte moi une autre blague", outputs[3])
    }

    @Test
    fun oneNoOptionalFollowedByOneOptional() {
        val iText = "(Tu peux|Peux-tu) [me|nous] raconter une blague"
        val outputs = flattenBnfRegex(iText)
        assertEquals(6, outputs.size)
        assertEquals("Tu peux  raconter une blague", outputs[0])
        assertEquals("Tu peux me raconter une blague", outputs[1])
        assertEquals("Tu peux nous raconter une blague", outputs[2])
        assertEquals("Peux-tu  raconter une blague", outputs[3])
        assertEquals("Peux-tu me raconter une blague", outputs[4])
        assertEquals("Peux-tu nous raconter une blague", outputs[5])    }
}
