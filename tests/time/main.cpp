#include<cstdio>
#include<chrono>

int main(int argc, char** argv)
{
//Get a timepoint associated with the current time using the steady clock
std::chrono::time_point<std::chrono::steady_clock> currentTime;
currentTime = std::chrono::steady_clock::now();

//Create timepoint that is offset by 10000 milliseconds
std::chrono::time_point<std::chrono::steady_clock> offsetTime; 
offsetTime = currentTime + std::chrono::milliseconds(10000);

//Busy loop until the timepoint is reached
while(std::chrono::steady_clock::now() < offsetTime)
{
printf("Milliseconds remaining: %ld\n", std::chrono::duration_cast<std::chrono::milliseconds>(offsetTime - std::chrono::steady_clock::now()).count());
}


return 0;
}
