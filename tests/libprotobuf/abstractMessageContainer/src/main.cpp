#include<cstdio>

#include "../messages/localURI.pb.h"
#include "../messages/localURIQuery.pb.h"
#include "../messages/localURIRemovalRequest.pb.h"
#include "../messages/localURIReplyHeader.pb.h"
#include "../messages/messageContainer.pb.h"


int main(int argc, char **argv)
{


messageContainer myMessageContainer;

if(!myMessageContainer.HasExtension(localURIReply::aLocalURIReply))
{
printf("Message container does not currently have a local URI reply header\n");
}

//This is one way
localURIReply myLocalURIReplyHeader;
myLocalURIReplyHeader.set_value(REQUEST_SUCCEEDED);

(*myMessageContainer.MutableExtension(localURIReply::aLocalURIReply)) = myLocalURIReplyHeader;

//This is another
//myMessageContainer.MutableExtension(localURIReply::aLocalURIReply)->set_value(REQUEST_SUCCEEDED); 




if(myMessageContainer.HasExtension(localURIReply::aLocalURIReply))
{
printf("The message container now has a localURIReplyHeader\n");

if(myMessageContainer.GetExtension(localURIReply::aLocalURIReply).value() != REQUEST_FAILED)
{
printf("It says a request succeeded\n");
}
else
{
printf("It says a request failed\n");
}

}



return 0;
}
