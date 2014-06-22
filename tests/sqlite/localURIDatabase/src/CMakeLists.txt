FILE(GLOB SOURCEFILES *.cpp *.c)

message( ${SOURCEFILES} ${ProtoSources} )

#Add the compilation target
ADD_EXECUTABLE(localURIDatabase ${SOURCEFILES})

#link libraries to executable
target_link_libraries(localURIDatabase ${PROTOBUF_LIBRARY} sqlite3 messages.a)
