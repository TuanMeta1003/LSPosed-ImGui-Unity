plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

android {
    namespace = "com.zrt.xposed"
    compileSdk = 34
    ndkVersion = "28.2.13676358"

    defaultConfig {
        applicationId = "com.zrt.xposed"
        minSdk = 29
        targetSdk = 35
        versionCode = 11
        versionName = "2.3.4"
		
		externalNativeBuild {
            cmake {
			    cppFlags += "-Wno-macro-redefined"
                cppFlags += "-DVERSION_NAME=\\\"${versionName}\\\""
            }
        }
    }

    buildTypes {
        debug {
            ndk {
                abiFilters += listOf("arm64-v8a")
            }
        }
        release {
            ndk {
                abiFilters += listOf("arm64-v8a", "armeabi-v7a", "x86", "x86_64")
            }
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/jni/CMakeLists.txt")
        }
    }
	
	android.applicationVariants.all {
        outputs.all {
            val appName = "LSPosed"
            val version = versionName
            val buildType = buildType.name
            val outputImpl = this as com.android.build.gradle.internal.api.BaseVariantOutputImpl
            outputImpl.outputFileName = "${appName}-v${version}-${buildType}.apk"
        }
    }
}

dependencies {
    // Note the libxposed-service should not be compileOnly
    compileOnly(fileTree(mapOf("dir" to "libs", "include" to listOf("*.aar", "*.jar"))))
    compileOnly("androidx.annotation:annotation:1.7.1") // required only for Java Module
}