#include<cstdio>
#include <functional>
#include <unistd.h>

class testClass
{
public:

testClass(int inputTestClassVariable)
{
printf("Test class constructing\n");
testClassVariable = inputTestClassVariable;
}

~testClass()
{
printf("Test class destructing\n");
}

int testClassVariable;
};


class lambdaScopeGuard
{
public:
lambdaScopeGuard(std::function<void()> inputFunction)
{
functionToCall = inputFunction;
}

~lambdaScopeGuard() noexcept
{
functionToCall();
}

private:
lambdaScopeGuard()
{
}

std::function<void()> functionToCall;
};

int main(int argc, char **argv)
{

/*
int i=0;

auto func = [&]() {printf("Hello World %d\n", i);};

func();

i++;

func();
*/
try
{
printf("Entered scope\n");
printf("Allocating class on heap\n");
testClass *myTestClass = new testClass(5);
lambdaScopeGuard myLambdaScopeGuard([&](){printf("Scope guard cleaning up object\n"); delete myTestClass;});

//Exit by exception
throw 20;
}
catch(int &inputException)
{
}
printf("Scope has been exited\n");

sleep(10);

return 0;
}


