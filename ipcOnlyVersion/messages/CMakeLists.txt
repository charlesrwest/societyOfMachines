
file(GLOB ProtoFiles *.proto)

PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})
add_library(messages STATIC ${ProtoSources} ${ProtoHeaders})
target_link_libraries(messages ${PROTOBUF_LIBRARY})

