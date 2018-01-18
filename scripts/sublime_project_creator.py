# castle tools - sublime project creator

# Super hacky script that creates a sublime project that will load all components into the folder view
# File will be in root repo directory named <FOLDER>.sublime-project
# This allows for easy autofill and quicker API checking

# TODO:
#   - Utilize file_exclude_patterns and folder_exclude_patterns for folders
#   - Ignore folders for all but apq?

# How to use:
# Run from location of your components.json

# To create a project for sublime on windows it takes location of your scratch mount as arg:
#     dirk@drbox:/scratch/Professor$ python scripts/sublime_project_creator.py --scratch Y:

# To create project for sublime on linux just run from *.git base:
#     dirk@drbox:/scratch/Professor$ python scripts/sublime_project_creator.py

# If you do setloc for a repo just rerun and re-open project

import json
import os
import subprocess
import sys
import argparse

parser = argparse.ArgumentParser(description='Create sublime text project')
parser.add_argument('--scratch', dest="scratchRemote", help='Remote scratch mount dir')
parser.add_argument('--output', dest="outputName", help='Output file name')
parser.add_argument('--select', action="store_true", default=False, help='Show Y/n for each component')

# THIS DEFAULTS TO 1.3 FOR DP1 FOR NOW
parser.add_argument('--hsp', dest="hspVersion", help='Set RIVIERA_HSP_VERSION ')

# NOTE YOU NEED WINDOWS SSH PUBKEY IN THE REMOTE ~/.ssh/authorized_keys to use this
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

# PARSE OUTPUT FILE NAME
fileName = os.path.basename( os.getcwd() )
if( args.outputName ):
    fileName = args.outputName


# PARSE HSP VERSION
hsp = "1.3" # hard coded
if( args.hspVersion ):
    hsp = args.hspVersion

run_command( "env RIVIERA_HSP_VERSION="+hsp+" make check_tools")

buildMeta = [ [ "Make " + fileName , os.getcwd() ] ]

# PARSE REMOTE SCRATCH PATH
remote = False
remotePath = ""
if( args.scratchRemote ):
    remote = True
    remoteIsWin = True # For now we assume remote is windows machine with samba mounted drive
    remotePath = args.scratchRemote
    print( "Remote scratch path set to : " + remotePath)
    if( not args.remoteUser or not args.remoteHost):
        print("!!!!Warning set scratch to remote but set no remote build user/host!!!")


outputFileName = os.path.join( os.getcwd(), fileName + ".sublime-project" )
print( "Creating project : " + outputFileName )

filePath = os.path.abspath(os.path.join(os.path.dirname( __file__ ), '..', 'components.json'))
data = json.load( open(filePath) )

baseJson = '{ "folders" : [] }'
baseJsonObj = json.loads( baseJson )

outputFolders = []


if( remote ):
    basePath = remotePath + os.getcwd().replace("scratch/","").replace("/", "\\")
    compDict = { "path" : basePath }
    outputFolders.append( compDict )
else:
    compDict = { "path" : os.getcwd()}
    outputFolders.append( compDict )

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
    compDict = { "path" : compLocation }

    if( args.select ):
        maybe = raw_input(comp['name'] + " [Y/n] ? ")
        if( maybe == "Y" ):
            outputFolders.append( compDict )
    else:
        # Skip HSP dirs
        if( comp["name"] != "Riviera-Toolchain" and comp["name"] != "Riviera-HSP-Images" ):
            outputFolders.append( compDict )

# SUPER HACKY PER COMPONENT BUILD STUFF :)
myBuilds = []

buildMeta.reverse()

# CREATE COMMAND TO MAKE ALL SETLOC COMPONENTS
buildAllString = ""
if( args.remoteUser and args.remoteHost ):
    buildAllString = "ssh " + args.remoteUser + "@" + args.remoteHost + " \""

for meta in buildMeta:
    # Example command I got to work
    # ssh dirk@drbox "cd /scratch/Professor ; source ~/.profile; env RIVIERA_HSP_VERSION=1.3 make"

    makeCommand = ""
    if( args.remoteUser and args.remoteHost ):
        makeCommand = "ssh " + args.remoteUser + "@" + args.remoteHost + " \""

        makeCommand += "echo !!!! MAKING "+meta[1]+" !!!; cd " + meta[1] + " ; source ~/.profile; env RIVIERA_HSP_VERSION=" +hsp+ " make ;"
        buildAllString += "echo !!!! MAKING "+meta[1]+" !!!; cd " + meta[1] + " ; source ~/.profile; env RIVIERA_HSP_VERSION=" +hsp+ " make ;"

        makeCommand += "\""
    else:
        makeCommand = "echo !!!! MAKING "+meta[1]+" !!!; cd " + meta[1] + " ; env RIVIERA_HSP_VERSION=" +hsp+ " make ; cd " + os.getcwd() + "; "
        buildAllString += "echo !!!! MAKING "+meta[1]+" !!!; cd " + meta[1] + " ; env RIVIERA_HSP_VERSION=" +hsp+ " make ; cd " + os.getcwd() + "; "


    # Maybe do something about sdk in name here
    myBuilds.append( { "name" : meta[0], "shell_cmd" : makeCommand })

if( args.remoteUser and args.remoteHost ):
    buildAllString += " \""

myBuilds.append( { "name" : "Make All", "shell_cmd" : buildAllString })


baseJsonObj["folders"] = outputFolders
baseJsonObj["build_systems"] = myBuilds


outputFile = open( outputFileName, "w")
outputFile.truncate()
outputFile.write( json.dumps(baseJsonObj, indent=4, sort_keys=True) )
