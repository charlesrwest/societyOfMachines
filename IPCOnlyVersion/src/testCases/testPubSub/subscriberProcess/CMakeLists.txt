cmake_minimum_required (VERSION 2.8.3)

FILE(GLOB SOURCEFILES *.cpp *.c)

message( ${SOURCEFILES} ${ProtoSources} )


#Add the compilation target
ADD_EXECUTABLE(testPubSubSubscriber ${SOURCEFILES})

#link libraries to executable
target_link_libraries(testPubSubSubscriber societyOfMachines.a ${PROTOBUF_LIBRARY} sqlite3 zmq messages.a )
