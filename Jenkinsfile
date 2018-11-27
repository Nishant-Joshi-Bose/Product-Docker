#!/usr/bin/env groovy
@Library(['JenkinsSharedLibrary']) _
import JenkinsJobCleanup
/*
  Description: Jenkinsfile is a container for the pipeline script.
               Runs tests against a given branch in CastleProducts repo
*/

/*
To abort in progress / in queue jobs started on PR.
*/
def JobCleanup = new JenkinsJobCleanup()
JobCleanup.CheckPendingJob('Continous_Build_Testing/CastleProducts', env.BRANCH_NAME)

def buildJob(jobName)
{
   build job: jobName , parameters: [[$class: 'GitParameterValue', name: 'Branch', value: "${env.CHANGE_BRANCH}"],string(name: 'PullReqId', value: "${env.CHANGE_ID}"), string(name: 'PR_NAME', value: "${env.BRANCH_NAME}")]
}

def buildQCJob(jobName, SDKValue)
{
   build job: jobName, parameters: [string(name: 'SDK', value: SDKValue), string(name: 'Branch', value: "${env.CHANGE_BRANCH}"), string(name: 'PullReqId', value: "${env.CHANGE_ID}")]
}

/*
Below are the jobs list to run against each PR.
*/

def EddiePRTest   = ["Continuous_Build_Testing/CastleProduct_Continuous_Build_Testing_Master_PR"]


failureList = []

def executeBuild(item, SDKValue = null){
    try
    {
        if(SDKValue != null){
            buildQCJob(item, SDKValue)
        }
        else{
            buildJob(item)
        }
        echo 'Current Build set to SUCCESS'
    }
    catch (Exception ex)
    {
        echo "Caught: ${ex}"
        failureList.add(item)
        echo 'Current Build set to FAILURE'
    }
}

timeout(time: 4, unit: 'HOURS')
{
  node('eco2-pipeline-container')
    {
      try
       {
          stage("EDDIE_PR")
                 {
                    for (item in EddiePRTest)
                    {
                        try
                        {
                            executeBuild(item)
                        }
                        catch (Exception ex)
                        {
                            echo "Caught: ${ex}"
                        }
                    }
                 }
        }
    }
     catch (e)
       {
           currentBuild.result = 'FAILURE'
           throw e
       }
    finally
      {
          currentBuild.description = "${failureList}"
          if(failureList){
              currentBuild.result = 'FAILURE'
          }
          else{
              currentBuild.result = 'SUCCESS'
          }
      }
    }
}
