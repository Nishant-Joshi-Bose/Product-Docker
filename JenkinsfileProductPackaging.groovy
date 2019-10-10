#!/usr/bin/env groovy

@Library(['JenkinsConanLibrary@conan']) _

import JenkinsConan
def conan = new JenkinsConan(this)

//PARAMS
String compile_label = "COMPILE_NODE_BIG"
String[] alwaysBuild = ['professor/master']
String tagPrefix = 'professor/'
String build_timeout = '120'
String product_type = 'professor,ginger-cheevers' // must be all lowercase

conanBuildProductPipeline( conan, compile_label, alwaysBuild, tagPrefix, build_timeout, product_type )
