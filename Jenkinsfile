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
    build job: jobName, parameters: [string(name: 'SW_FREQUENCY', value: "${SW_FREQUENCY}"), string(name: 'SW_BRANCH', value: "${SW_BRANCH}"),string(name: 'EC_URL', value: "${EC_URL}"),[$class: 'GitParameterValue', name: 'Branch', value: "${Branch}"]]
}

def CAPS_Component = ["CAPS-Component/playbackRequest",
                      "CAPS-Component/volumeAPIs"]

def Castle_SW_Update = ["CastleSoftwareUpdate/Riviera_BonjourUpdate",
			"CastleSoftwareUpdate/Riviera_BonjourUpdate_Corrupt_IPK_Scenarios",
                        "CastleSoftwareUpdate/Riviera_OTA_Corrupt_IPK_Fail_Install_Scenarios",
                        "CastleSoftwareUpdate/Riviera_OTA_Corrupt_IPK_Scenarios",
                        "CastleSoftwareUpdate/Riviera_OTA_Corrupt_Packages_Scenarios",
                        "CastleSoftwareUpdate/Riviera_OTA_Corrupt_Signature_IPKs",
                        "CastleSoftwareUpdate/Riviera_OTA_Corrupt_Signature_Package_Files",
                        "CastleSoftwareUpdate/Riviera_OTA_Corrupt_Signature_Sequence_File",
		       	"CastleSoftwareUpdate/Riviera_OTA_Missing_Signature_IPKs",
		       	"CastleSoftwareUpdate/Riviera_OTA_Missing_Signature_Packages",
		       	"CastleSoftwareUpdate/Riviera_OTA_Missing_Signature_Sequence_File",
		        "CastleSoftwareUpdate/Riviera_OTA_Same_IPK_Version",
		       	"CastleSoftwareUpdate/Riviera_SoftwareUpdate"]

def CastleTestUtils_Integrated = ["CastleTestUtils_Integrated/memoryConsumption"]

def CastleTestUtils_Component = ["CastleTestUtils-Component/CastleTestUtils-tests"]

def Eddie_Product  = ["EddieProduct/Eddie-Bootup-Sequence-Timing",
                      "EddieProduct/Eddie-DemoController",
                      "EddieProduct/Eddie-Preset"]

def Galapagos_Client = ["GalapagosClient/Authentication",
                        "GalapagosClient/GalapagosClient-Activation",
                        "GalapagosClient/GalapagosClient-ServiceDiscovery"]

def STS_Component = ["STS-Component/Deezer_Component_d",
                     "STS-Component/Amazon_Component_d",
		     "STS-Component/Spotify_Component_d"]

def STS_Integrated = ["STS-Integrated/Amazon_Integrated",
                      "STS-Integrated/Deezer_Integrated",
                      "STS-Integrated/Spotify_Integrated",
                      "STS-Integrated/TuneIn-Integrated"]

def NetworkServices_Component = ["NetworkServices-Component/network-wifi-profile"]

def E2E = ["E2E/BonjourUpdateTest_Master",
	   "E2E/setupAP"]


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
        def subject = "${buildStatus}: EDDIE PIPELINE UNSTABLE: Job '$item '"
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
        def subject = "${buildStatus}: EDDIE PIPELINE PASSED"
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
  node('master')
    {
      echo "EC_URL used: ${env.EC_URL}"
      echo "Branch Used used: ${env.SW_BRANCH}"
      echo "freq Used used: ${env.SW_FREQUENCY}"
      echo "VERSION used: ${env.VERSION}"
      echo "Git Branch used: ${env.Branch}"
     try
       {
          parallel(
               "stream 1 (CAPS_Component)" :{

					stage("CAPS_Component")
                    {
                        for (item in CAPS_Component)
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
                },

             "stream 2 (Castle_SW_Update)" :{
					
					stage("Castle_SW_Update")
                    {
                        for (item in Castle_SW_Update)
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
               },
			    "stream 3 (CastleTestUtils_Integrated)" :{
					
					stage("CastleTestUtils_Integrated")
                    {
                        for (item in CastleTestUtils_Integrated)
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
               }, "stream 4 (CastleTestUtils_Component)" :{
					
					stage("CastleTestUtils_Component")
                    {
                        for (item in CastleTestUtils_Component)
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
               }, "stream 5 (Eddie_Product)" :{
					
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
               }, "stream 6 (Galapagos_Client)" :{
					
					stage("Galapagos_Client")
                    {
                        for (item in Galapagos_Client)
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
               }, "stream 7 (STS_Component)" :{
					
					stage("STS_Component")
                    {
                        for (item in STS_Component)
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
               }, "stream 8 (STS_Integrated)" :{
					
					stage("STS_Integrated")
                    {
                        for (item in STS_Integrated)
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
               }, "stream 9 (E2E)" :{
					
					stage("E2E")
                    {
                        for (item in E2E)
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
               })
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
    def GraphiteSocket = new GraphiteSocket(failureList.size(),buildCount,'Eddie_Nightly')  
}
