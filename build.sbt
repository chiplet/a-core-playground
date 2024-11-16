import scala.sys.process._
// OBS: sbt._ has also process. Importing scala.sys.process
// and explicitly using it ensures the correct operation

ThisBuild / scalaVersion     := "2.13.8"
ThisBuild / version          := scala.sys.process.Process("git rev-parse --short HEAD").!!.mkString.replaceAll("\\s", "")+"-SNAPSHOT"
ThisBuild / organization     := "com.vhirvone"

// Suppresses eviction errors for new sbt versions
ThisBuild / evictionErrorLevel := Level.Info

resolvers += "A-Core Gitlab" at "https://gitlab.com/api/v4/groups/13348068/-/packages/maven"

val chiselVersion = "3.5.1"

// Sets amba and a_core_common version for the whole project.
// including sub-projects like ACoreBase and ProgMem
val ambaVersion = settingKey[String]("The version of amba used for building.")
val accVersion  = settingKey[String]("The version of a_core_common used for building")
ambaVersion := "0.7+"
accVersion  := "0.4+"

lazy val acoreplayground = (project in file("."))
  .settings(
    name := "acoreplayground",
    libraryDependencies ++= Seq(
      "edu.berkeley.cs" %% "chisel3" % chiselVersion,
      "edu.berkeley.cs" %% "chiseltest" % "0.5.1",
      "edu.berkeley.cs" %% "chisel-iotesters" % "2.5.0",
      "Chisel-blocks"   %% "amba" % ambaVersion.value
    ),
    scalacOptions ++= Seq(
      "-language:reflectiveCalls",
      "-deprecation",
      "-feature",
      "-Xcheckinit",
      "-P:chiselplugin:genBundleElements",
    ),
    addCompilerPlugin("edu.berkeley.cs" % "chisel3-plugin" % chiselVersion cross CrossVersion.full),
  )
