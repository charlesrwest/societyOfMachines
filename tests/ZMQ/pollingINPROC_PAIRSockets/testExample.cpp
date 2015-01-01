#include "zmq.hpp"


int main (int argc, char *argv [])
{
zmq::context_t context;

zmq::socket_t socket1(context, ZMQ_PAIR);
zmq::socket_t socket2(context, ZMQ_PAIR);

socket1.bind("inproc://d.inproc");
socket2.connect("inproc://d.inproc");

char arr[1];
arr[0] = 1;

socket1.send(arr, 1, 0);

zmq::pollitem_t item;
item.socket = (void *) socket2;
item.events = ZMQ_POLLIN;

int result = zmq::poll(&item, 1, -1);

return 0;
}
