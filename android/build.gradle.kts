import com.vanniktech.maven.publish.AndroidSingleVariantLibrary
import org.gradle.internal.extensions.core.extra
import org.jetbrains.kotlin.gradle.dsl.JvmTarget
import org.jetbrains.kotlin.gradle.tasks.KotlinCompile

plugins {
	alias(libs.plugins.android.library)
	alias(libs.plugins.vanniktech.publish)
}

android {
	namespace = "io.openmobilemaps.gps"

	compileSdk = libs.versions.android.compileSdk.get().toInt()

	defaultConfig {
		minSdk = libs.versions.android.minSdk.get().toInt()

		testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
		consumerProguardFiles("consumer-rules.pro")
		externalNativeBuild {
			cmake {
				arguments += listOf("-DANDROID_STL=c++_shared", "-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON")
				cppFlags += "-frtti -fexceptions -O2"
			}
		}
	}

	externalNativeBuild {
		cmake {
			path = file("CMakeLists.txt")
		}
	}

	sourceSets {
		named("main") {
			kotlin.directories += "../bridging/android/java"
		}
	}

	compileOptions {
		sourceCompatibility = JavaVersion.VERSION_21
		targetCompatibility = JavaVersion.VERSION_21
	}

	buildFeatures {
		prefab = true
		prefabPublishing = true
		buildConfig = true
	}

	prefab {
		create("layergps") {
			headers = ".cpp_includes"
		}
	}

	packagingOptions {
		jniLibs {
			excludes += setOf(
				"**/libmapscore.so",
				"**/libc++_shared.so"
			)
		}
	}

	buildTypes {
		getByName("debug") {
			externalNativeBuild {
				cmake {
					cppFlags += "-DDEBUG"
				}
			}
		}
		getByName("release") {
			externalNativeBuild {
				cmake {
					cppFlags += "-DNDEBUG"
				}
			}
			ndk {
				debugSymbolLevel = "SYMBOL_TABLE"
			}
		}
	}

	// Latest LTS version (as of April 2026)
	ndkVersion = "27.3.13750724"
}

tasks.withType<KotlinCompile>().configureEach {
	compilerOptions.jvmTarget.set(JvmTarget.JVM_21)
}

val copyHeaders by tasks.registering {
	val headersDir = ".cpp_includes"
	project.delete(headersDir)
	project.mkdir(headersDir)
	val collection = project.files(
		"src/main/cpp",
		"../bridging/android/jni",
		"../shared/public",
		"../shared/src"
	)
	project.copy {
		duplicatesStrategy = DuplicatesStrategy.FAIL
		from(collection.asFileTree.files)
		include("**/*.h")
		into(project.file(headersDir))
	}
}

tasks.named("preBuild") {
	dependsOn(copyHeaders)
}

afterEvaluate {
	listOf("Debug", "Release").forEach { variant ->
		tasks.matching { it.name.startsWith("configureCMake${variant}[") }.configureEach {
			dependsOn(":mapscore:prefab${variant}Package")
		}
	}
}

publishing {
	repositories {
		maven {
			val ubiqueMavenUrl: String by extra
			val ubiqueMavenUser: String by extra
			val ubiqueMavenPass: String by extra

			name = "UbiqueMaven"
			url = uri(ubiqueMavenUrl)
			credentials {
				username = ubiqueMavenUser
				password = ubiqueMavenPass
			}
			authentication {
				create<BasicAuthentication>("basic")
				create<DigestAuthentication>("digest")
			}
		}
	}
}

mavenPublishing {
	configure(AndroidSingleVariantLibrary(variant = "release"))

	publishToMavenCentral(true)
	signAllPublications()
}

dependencies {
	implementation(libs.androidx.core)
	implementation(libs.play.services.location)

	api(projects.mapscore)
	implementation(libs.ubique.djinni)

	testImplementation(libs.junit)
	androidTestImplementation(libs.androidx.junit)
	androidTestImplementation(libs.androidx.espresso.core)
}

tasks.named("clean") {
	doLast {
		project.delete("${projectDir}/build")
		project.delete("${projectDir}/.cxx")
	}
}
