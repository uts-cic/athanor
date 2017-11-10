//
// A dummy athanor project 
// just to get project dependency going
// between athanor-server and athanor 
// 
//
// The pack settings is to produce the dummy
// athanor jar that is a prerequisite of the athanor-server project
// This is produced by running sbt pack 
packAutoSettings
name := "athanor"
// Match the scala version used in the athanor-server project
scalaVersion := "2.12.3"
organization := "au.edu.utscic"
 

