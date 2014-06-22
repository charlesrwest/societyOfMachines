FILE(GLOB SOURCEFILES *.cpp *.c)

message( SOURCEFILES )

message(ProtoSources)

#Add the compilation target
ADD_EXECUTABLE(addPerson ${SOURCEFILES})

#link libraries to executable
target_link_libraries(addPerson ${PROTOBUF_LIBRARY} messages.a)
