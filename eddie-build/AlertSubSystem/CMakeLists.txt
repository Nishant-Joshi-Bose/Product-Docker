INCLUDE( exe.cmake )

# this is a test
#cmake_minimum_required(VERSION 3.5)
#
#project(Alerts)
#  
#add_compile_options("-std=gnu++11" "-g" "-Wformat=0" )
#
# the base directory the checked out element came from
# for example if you did "svn checkout Voice_Experimentation"  from the directory dog,
# your VEXP would be ~/dog/Voice_Experimentation
# We use this area as the starting place
#set (VEXP "../../../../Voice_Experimentation")
#
#list(APPEND IncDirs  "${CMAKE_SOURCE_DIR}")
#list(APPEND IncDirs  "../")
#list(APPEND IncDirs  "include")
#list(APPEND IncDirs  "${VEXP}/Core/OsAbstraction")
#list(APPEND IncDirs  "${VEXP}/Core/Network")
#list(APPEND IncDirs  "${VEXP}/Core/Utilities")
#list(APPEND IncDirs  "${VEXP}/Core/Utilities/StringUtils")
#list(APPEND IncDirs  "${VEXP}/Libs/protobuf/prebuilt/include")
#list(APPEND IncDirs  "${VEXP}/Core/InfrastructureLite")
#list(APPEND IncDirs  "${VEXP}/App/SystemController")
#list(APPEND IncDirs  "${VEXP}/Core/Infrastructure/Threads")
#list(APPEND IncDirs  "${VEXP}/Core/Voice/avs-cpp-sdk/AuthDelegate/include")
#list(APPEND IncDirs  "${VEXP}/Core/Voice/avs-cpp-sdk/ACL/include")
#list(APPEND IncDirs  "${VEXP}/Core/Voice/avs-cpp-sdk/Integration/include/")
#
#include_directories( ${IncDirs} )
#
# this is where I built my AVS libs
#set (AVS_LBASE "~/mylibs/avslib")
#
#
#message ("AVS_LBASE is " ${AVS_LBASE} )
#
# where to get the avs directories
#link_directories("${AVS_LBASE}/AuthDelegate/src/" "${AVS_LBASE}/Integration/src/"  "${AVS_LBASE}/ACL/src/"  )
#
# create a a list of the source files to included
#list(APPEND mySource main.cpp )
#list(APPEND mySource Alerts.cpp )
#list(APPEND mySource AlertsScheduler.cpp )
#list(APPEND mySource AlertsSystemClient.cpp )
#list(APPEND mySource AlertsManager.cpp )
#list(APPEND mySource AlertsMessage.cpp )
#list(APPEND mySource AlsaAlertOutput.cpp )
#list(APPEND mySource playAudio.cpp )
#list(APPEND mySource mongoose.c )
#list(APPEND mySource AlertsClient.cpp )
#list(APPEND mySource ../../InfrastructureLite/BreakThread.cpp )
#list(APPEND mySource ../../InfrastructureLite/APTimer.cpp )
#list(APPEND mySource ../../InfrastructureLite/APTimerFactory.cpp )
#list(APPEND mySource ../../InfrastructureLite/APTask.cpp )
#list(APPEND mySource ../../InfrastructureLite/APSimpleTask.cpp )
#list(APPEND mySource ../../InfrastructureLite/APTaskFactory.cpp )
#list(APPEND mySource ../../InfrastructureLite/CallbackConnection.cpp )
#list(APPEND mySource ../../InfrastructureLite/APEventGroup.cpp )
#list(APPEND mySource ../../InfrastructureLite/NotifyTargetTaskIF.cpp )
#list(APPEND mySource ../../OsAbstraction/Timer.cpp )
#list(APPEND mySource ../../OsAbstraction/ThreadMutex.cpp )
#list(APPEND mySource ../../OsAbstraction/TimeKeeper.cpp )
#list(APPEND mySource DPrint.cpp )
#list(APPEND mySource ../../OsAbstraction/Panics.cpp )
#list(APPEND mySource ../../OsAbstraction/Task.cpp )
#list(APPEND mySource ../../OsAbstraction/BSemaphore.cpp )
#list(APPEND mySource ../../OsAbstraction/Assertions.cpp )
#list(APPEND mySource ../../OsAbstraction/EventFd.cpp )
#list(APPEND mySource ../../OsAbstraction/EventGroup.cpp )
#list(APPEND mySource ../../OsAbstraction/FunctionInfo.cpp )
#list(APPEND mySource ../../OsAbstraction/Demangle.cpp )
#list(APPEND mySource ../../Network/Handle.cpp )
#list(APPEND mySource ../../Network/HandleSet.cpp )
#list(APPEND mySource ../../OsAbstraction/FunctionInfo.cpp )
#list(APPEND mySource ../../Utilities/BTime.cpp )
#
#add_executable(AlertsSubsystem ${mySource} )
#
#target_link_libraries(AlertsSubsystem pthread rt uuid asound)
#
#
#
#
#
