FILE(GLOB SOURCEFILES *.cpp *.c)

message( ${SOURCEFILES} ${ProtoSources} )


#Add the compilation target
#ADD_EXECUTABLE(localURIDatabase ${SOURCEFILES})

#link libraries to executable
#target_link_libraries(localURIDatabase ${PROTOBUF_LIBRARY} sqlite3 zmq messages.a)

#Tell cmake were to find the sub-projects
add_subdirectory(./testCode/testDatabase)
add_subdirectory(./testCode/testPubSubURISearching)
add_subdirectory(./testCode/testPubSub)
add_subdirectory(./testCode/testRequestReply)
