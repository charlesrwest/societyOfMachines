FILE(GLOB SOURCEFILES *.cpp *.c ../../libraryCode/*.cpp ../../libraryCode/*.c)

message( ${SOURCEFILES} ${ProtoSources} )


#Add the compilation target
ADD_EXECUTABLE(testDatabase ${SOURCEFILES})

#link libraries to executable
target_link_libraries(testDatabase ${PROTOBUF_LIBRARY} sqlite3 zmq messages.a)
