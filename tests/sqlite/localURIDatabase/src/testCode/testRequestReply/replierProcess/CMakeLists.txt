FILE(GLOB SOURCEFILES *.cpp *.c ../../../*.cpp ../../../*.c)

message( ${SOURCEFILES} ${ProtoSources} )


#Add the compilation target
ADD_EXECUTABLE(testRequestReplierReplier ${SOURCEFILES})

#link libraries to executable
target_link_libraries(testRequestReplierReplier ${PROTOBUF_LIBRARY} sqlite3 zmq messages.a)
