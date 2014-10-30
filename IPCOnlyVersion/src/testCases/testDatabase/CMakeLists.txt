cmake_minimum_required (VERSION 2.8.3)

FILE(GLOB SOURCEFILES *.cpp *.c)


#message( ${SOURCEFILES} ${ProtoSources} )


#Add the compilation target
ADD_EXECUTABLE(testDatabase ${SOURCEFILES})

#link libraries to executable
target_link_libraries(testDatabase societyOfMachines.a sqlite3 ${PROTOBUF_LIBRARY} zmq messages.a)
