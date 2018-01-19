# castle tools - sublime project creator
# AUTHOR: DWR

# WIKI: https://wiki.bose.com/display/BC/Sublime+Text+3+-+Castle+Tools+Project+Generator

import json
import os
import subprocess
import sys
import argparse

parser = argparse.ArgumentParser(description='Create sublime text project')
parser.add_argument('--update', dest="updateFile", help='Update a project file after changing components')
parser.add_argument('--output', dest="outputName", help='Output file name')

parser.add_argument('--select', action="store_true", default=False, help='Show Y/n for each component')

# THIS DEFAULTS TO 1.3 FOR DP1 FOR NOW
parser.add_argument('--hsp', dest="hspVersion", help='Set RIVIERA_HSP_VERSION ')

# NOTE YOU NEED WINDOWS SSH PUBKEY IN THE REMOTE ~/.ssh/authorized_keys to use this
parser.add_argument('--scratch', dest="scratchRemote", help='Remote scratch mount dir')
parser.add_argument('--user', dest="remoteUser", help='Username to log into remote machine')
parser.add_argument('--host', dest="remoteHost", help='Hostname of remote machine to make on')


args = parser.parse_args()

def run_command(command):
    """given shell command, returns communication tuple of stdout and stderr"""
    return subprocess.Popen(command,
                            shell=True,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            stdin=subprocess.PIPE).communicate()

########################################################################
# DEFAULT PARAMS
hsp = "1.3"
remote = False
remotePath = ""
remoteUser = ""
remoteHost = ""
remoteBuild = False



########################################################################
# CHECK IF UPDATING
updating = False
updateJson = {}
if( args.updateFile ):
    updating = True
    updateJson = json.load( open( os.path.join( os.getcwd(), args.updateFile ) ) )

    # LOAD IN SAVED PARAMS
    hsp = updateJson["castle_sublime_params"]["hsp"]
    remote = updateJson["castle_sublime_params"]["remote"]
    remotePath = updateJson["castle_sublime_params"]["remote_path"]
    remoteUser = updateJson["castle_sublime_params"]["remote_user"]
    remoteHost = updateJson["castle_sublime_params"]["remote_host"]
    remoteBuild = updateJson["castle_sublime_params"]["remote_build"]

def IsComponentInProjext( name ):
    #print(updateJson["folders"])
    for comp in updateJson["folders"]:
        if( "name" in comp and comp["name"] == name ):
            return True;
    return False

########################################################################
# PARSE REMOTE BUILD
if( args.remoteUser and args.remoteHost):
    remoteBuild = True
    remoteUser = args.remoteUser
    remoteHost = args.remoteHost

########################################################################
# PARSE OUTPUT FILE NAME
fileName = os.path.basename( os.getcwd() )
if( args.outputName ):
    fileName = args.outputName

outputFileName = os.path.join( os.getcwd(), fileName + ".sublime-project" )
print( "Creating project : " + outputFileName )


########################################################################
# PARSE HSP VERSION
if( args.hspVersion ):
    hsp = args.hspVersion


########################################################################
# PARSE REMOTE SCRATCH PATH
if( args.scratchRemote ):
    remote = True
    remoteIsWin = True # For now we assume remote is windows machine with samba mounted drive
    remotePath = args.scratchRemote
    print( "Remote scratch path set to : " + remotePath)
    if( not args.remoteUser or not args.remoteHost):
        print("!!!!Warning set scratch to remote but set no remote build user/host!!!")

########################################################################
# ENSURE COMPONENTS ARE INSTALLED
run_command( "env RIVIERA_HSP_VERSION="+hsp+" make check_tools")

########################################################################
# INIT BUILD LIST
buildMeta = [ [ "Make " + fileName , os.getcwd() ] ]

########################################################################
# LOAD COMPONENTS.JSON
filePath = os.path.abspath(os.path.join(os.path.dirname( __file__ ), '..', 'components.json'))
data = json.load( open(filePath) )

########################################################################
# INIT FOLDERS LIST
baseJson = '{ "folders" : [] }'
baseJsonObj = json.loads( baseJson )

outputFolders = []

########################################################################
# ADD ROOT FOLDER
if( remote ):
    basePath = remotePath + os.getcwd().replace("scratch/","").replace("/", "\\")
    compDict = { "path" : basePath, "name":  os.path.basename( os.getcwd() )}
    outputFolders.append( compDict )
else:
    compDict = { "path" : os.getcwd(), "name": os.path.basename( os.getcwd() )}
    outputFolders.append( compDict )

########################################################################
# LOOP THROUGH COMPONENTS AND GET LOCATIONS
for comp in data:
    findCmd = "env RIVIERA_HSP_VERSION=" +hsp+ " components get " + comp['name'] + " installed_location"
    cmdResp = run_command( findCmd )
    # print(cmdResp)
    compLocation = cmdResp[0].replace("\n", "")

    # hack for setloc
    if( "/builds" in compLocation ):
        # Bump up to root *.git dir
        compLocation = compLocation[ 0 : compLocation.index("/builds" )]
        # Add to list of things build when running make
        buildMeta.append( [ "Make " + comp['name'] , compLocation ] )

    if( remote ):
        compLocation = remotePath + compLocation.replace("scratch/","").replace("/", "\\")
    # print( compLocation )
    compDict = { "path" : compLocation, "name" : comp['name'] }

    if( args.select ):
        maybe = raw_input(comp['name'] + " [Y/n] ? ")
        if( maybe == "Y" ):
            outputFolders.append( compDict )
    else:
        # Only update if the thing is in out list
        if( updating == False or IsComponentInProjext( comp["name"] ) ) :
            outputFolders.append( compDict )

########################################################################
# SUPER HACKY PER COMPONENT BUILD STUFF :)
myBuilds = []
buildMeta.reverse()

########################################################################
# CREATE COMMAND TO MAKE ALL SETLOC COMPONENTS
buildAllString = ""
if( remoteBuild ):
    buildAllString = "ssh " + remoteUser + "@" + remoteHost + " \""

for meta in buildMeta:
    # Example command I got to work
    # ssh dirk@drbox "cd /scratch/Professor ; source ~/.profile; env RIVIERA_HSP_VERSION=1.3 make"

    makeCommand = ""
    if( remoteBuild ):
        makeCommand = "ssh " + remoteUser + "@" + remoteHost + " \""

        makeCommand += "echo !!!! MAKING "+meta[1]+" !!!; cd " + meta[1] + " ; source ~/.profile; env RIVIERA_HSP_VERSION=" +hsp+ " make ;"
        buildAllString += "echo !!!! MAKING "+meta[1]+" !!!; cd " + meta[1] + " ; source ~/.profile; env RIVIERA_HSP_VERSION=" +hsp+ " make ;"

        makeCommand += "\""
    else:
        makeCommand = "echo !!!! MAKING "+meta[1]+" !!!; cd " + meta[1] + " ; env RIVIERA_HSP_VERSION=" +hsp+ " make ; cd " + os.getcwd() + "; "
        buildAllString += "echo !!!! MAKING "+meta[1]+" !!!; cd " + meta[1] + " ; env RIVIERA_HSP_VERSION=" +hsp+ " make ; cd " + os.getcwd() + "; "


    # Maybe do something about sdk in name here
    myBuilds.append( { "name" : meta[0], "shell_cmd" : makeCommand })

if( remoteBuild ):
    buildAllString += " \""

myBuilds.append( { "name" : "Make All", "shell_cmd" : buildAllString })


########################################################################
# FINAL DICTIONARY CREATION
baseJsonObj["folders"] = outputFolders
baseJsonObj["build_systems"] = myBuilds

########################################################################
# SAVE PARAMS SCRIPT WAS RAN WITH
myParams = {}
myParams["version"]       = "potato"
#myParams["creation_hash"] = "TODO"
myParams["hsp"]           = hsp
myParams["remote"]        = remote
myParams["remote_path"]   = remotePath
myParams["remote_user"]   = remoteUser
myParams["remote_host"]   = remoteHost
myParams["remote_build"]  = remoteBuild

baseJsonObj["castle_sublime_params"] = myParams

########################################################################
# SAVE IT OFF
outputFile = open( outputFileName, "w")
outputFile.truncate()
outputFile.write( json.dumps(baseJsonObj, indent=4, sort_keys=True) )

########################################################################
# ENJOY
