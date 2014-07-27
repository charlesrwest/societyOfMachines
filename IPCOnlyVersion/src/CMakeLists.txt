FILE(GLOB SOURCEFILES ./libraryCode/*.cpp ./libraryCode/*.c)

message( ${SOURCEFILES} ${ProtoSources} )


#Add the compilation target
#ADD_EXECUTABLE(localURIDatabase ${SOURCEFILES})

#link libraries to executable
#target_link_libraries(localURIDatabase ${PROTOBUF_LIBRARY} sqlite3 zmq messages.a)

#Tell cmake were to find the sub-projects
#add_subdirectory(./libraryCode)
add_subdirectory(./testCases/testDatabase)
add_subdirectory(./testCases/testPubSubURISearching)
add_subdirectory(./testCases/testPubSub)
add_subdirectory(./testCases/testRequestReply)
