#!/usr/bin/env groovy
@Library(['JenkinsSocketAPI','JenkinsSharedLibrary']) _
import socket
import NodeVerification
/*
  Description: Jenkinsfile is a container for the pipeline script.
               Runs tests against a given branch in CastleTestUtils repostiory
*/

def buildJob(jobName)
{
    build job: jobName, parameters: [string(name: 'TESTRAIL_RUNID', value: "${TESTRAIL_RUNID}"),string(name: 'SW_FREQUENCY', value: "${SW_FREQUENCY}"), string(name: 'SW_BRANCH', value: "${SW_BRANCH}"),string(name: 'SW_VERSION', value: "${SW_VERSION}"), string(name: 'EC_URL', value: "${EC_URL}"),booleanParam(name: 'SW_UPDATE', value: Boolean.valueOf("${SW_UPDATE}")),[$class: 'GitParameterValue', name: 'Branch', value: "${Branch}"]]
}

/*def Eddie_Product  = ["Bluetooth/Bluetooth_Sink_Target_SOS",
					"EddieProduct/Eddie_chimes_test"]*/
def Eddie_Product  = 			["EddieProduct/Eddie-Bootup-Sequence-Timing",
                      			//"EddieProduct/Eddie-DemoController",
					//"EddieProduct/Eddie-Factory-Default",
					//"EddieProduct/Eddie-Key-Functionality",
					//"EddieProduct/Eddie-LightBar",
					"EddieProduct/Eddie-LPM",
					"EddieProduct/Eddie-ProductController",
					"EddieProduct/Eddie-Preset",
					//"EddieProduct/Eddie-LowPowerStandby",
					//"EddieProduct/Eddie-NoCrashResumingLowPowerStandby",
					"EddieProduct/Eddie-Network-Disabled",
					//"EddieProduct/Eddie-AUX",
					"EddieProduct/Eddie-Manufacturing",
					"EddieProduct/Eddie-Language",
					"EddieProduct/Eddie-Clock",
					//"Bluetooth/Bluetooth_Sink_Target_SOS",
					//"EddieProduct/Eddie_chimes_test"]

failureList = [] 
buildCount=0

def notifyBuild(String buildStatus = 'STARTED')
{
  // build status of null means successful
  buildStatus =  buildStatus ?: 'SUCCESS'
  def pipelineName = 'Eddie-Pipeline'
  def colorName = 'RED'
  def name = 'aman_osman'
  def colorCode = '#FF0000'
  for (item in failureList)
   { 
        def subject = "${buildStatus}: EDDIE PRODUCT PIPELINE UNSTABLE: Job '$item '"
        def summary = "${subject} ${item}: http://eco2jenkins.bose.com:8080/job/Pipelines/job/$pipelineName/lastFailedBuild/console"
        slackSend (color: colorCode, message: summary)
   }

  // Override default values based on build status
  if (buildStatus == 'STARTED')
    {
        color = 'YELLOW'
        colorCode = '#FFFF00'
    }
  else if(buildStatus == 'SUCCESS')
    {
        def subject = "${buildStatus}: EDDIE PRODUCT PIPELINE PASSED"
        def summary = "${subject}: http://eco2jenkins.bose.com:8080/job/Pipelines/job/$pipelineName/lastSuccessfulBuild/console"
		color = 'GREEN'
        colorCode = '#00FF00'
		slackSend (color: colorCode, message: summary)
    }
  else
    {
        color = 'RED'
        colorCode = '#FF0000'
    }
  //slackSend (color: colorCode, message: summary)
}

def executeBuild(item){
    try
    {
        echo 'call for Node Verification'
        echo "buildJob"
        buildCount = buildCount + 1
        def objNode = new CheckNode()
        println item
        if(objNode.CheckNodeVerification(item)){
            echo 'verify node and build Job call'
            buildJob(item)
            echo 'build Job executed'
            currentBuild.result = 'SUCCESS'
            echo 'Current Build set to SUCCESS'
        }
        else{
            echo "${item}: Add to Failure List"
            failureList.add(item)
            echo "${item}: Node offline"
            println failureList
            currentBuild.result = 'FAILURE'
            echo "${item}: Current Build set to FAILURE"
        }
    }
    catch (Exception ex)
    {
        echo "Caught: ${ex}"
        failureList.add(item)
        echo item
        echo 'Exception'
        println failureList
        currentBuild.result = 'FAILED'
    }
}

timeout(time: 8, unit: 'HOURS')
{
  node('eco2-pipeline-container')
    {
      echo "EC_URL used: ${env.EC_URL}"
      echo "Branch Used used: ${env.SW_BRANCH}"
      echo "freq Used used: ${env.SW_FREQUENCY}"
      echo "VERSION used: ${env.VERSION}"
      echo "Git Branch used: ${env.Branch}"
     try
       {
               

					stage("Eddie_Product")
                    {
                        for (item in Eddie_Product)
                         {
                           try
                           {
                              executeBuild(item)
							  
                           }
                           catch (Exception ex)
                           {
                              echo "Caught: ${ex}"
                           }
                           continue
                         }
                    }
                
        }

       catch (e)
       {
           currentBuild.result = "FAILED"
           throw e
       }

       finally
       {
            /* Comment out this part - To Stop sending Notifications Always
            * This will send notifications for Start Failure
            * Success or failure, always send notifications */
			notifyBuild(currentBuild.result)
            currentBuild.description = "${failureList}"
			println failureList
       }

    }
	// To Push Data of Failed Job and Total Job on Graphite
    println failureList.size()
    println buildCount
    graphite_table_name = 'Eddie-Product-Pipeline'
    if ("${SW_BRANCH}" != 'SOS'){
        graphite_table_name += "-${SW_BRANCH}"
    }
    def GraphiteSocket = new GraphiteSocket(failureList.size(),buildCount,graphite_table_name,Eddie_Product)  
}
