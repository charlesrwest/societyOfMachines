FILE(GLOB SOURCEFILES *.cpp *.c ../../../*.cpp ../../../*.c)

message( ${SOURCEFILES} ${ProtoSources} )


#Add the compilation target
ADD_EXECUTABLE(testPubSubPublisher ${SOURCEFILES})

#link libraries to executable
target_link_libraries(testPubSubPublisher ${PROTOBUF_LIBRARY} sqlite3 zmq messages.a)
