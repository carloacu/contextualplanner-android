Contextualplanner Android
=========================

Android wrapper of contextualplanner<br/>
Url of contextualplanner: https://github.com/carloacu/contextualplanner


### Step 1. Add the maven repository to your build file
Add it in your root build.gradle at the end of repositories:
```Kotlin
allprojects {
    repositories {
        ...
        maven { url 'https://raw.github.com/carloacu/contextualplanner-android-releases/master' }
    }
}
```

### Step 2. Add the dependency
```Kotlin
dependencies {
    implementation 'com.github.carloacu:contextualplanner-android:1.1.9'
}
```
