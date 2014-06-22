FILE(GLOB SOURCEFILES *.cpp *.c)

message( SOURCEFILES )

message(ProtoSources)

#Add the compilation target
ADD_EXECUTABLE(listPeople ${SOURCEFILES})

#link libraries to executable
target_link_libraries(listPeople ${PROTOBUF_LIBRARY} messages.a)
