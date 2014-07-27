FILE(GLOB SOURCEFILES *.cpp *.c ../../../libraryCode/*.cpp ../../../libraryCode/*.c)

message( ${SOURCEFILES} ${ProtoSources} )


#Add the compilation target
ADD_EXECUTABLE(testPubSubSubscriber ${SOURCEFILES})

#link libraries to executable
target_link_libraries(testPubSubSubscriber ${PROTOBUF_LIBRARY} sqlite3 zmq messages.a)
