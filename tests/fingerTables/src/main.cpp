#include "fingerTable.hpp"
#include<cstdio>
#include<cstdint>

int main(int argc, char **argv)
{
//Make chord ID just at the edge of overflow
uint64_t chordID = 10;
chordID--;

uint64_t predecessorID = 0;
predecessorID = predecessorID - 10;

uint64_t inputChordID = 14;
inputChordID = inputChordID - 5;

printf("Chord ID: %ld\nAssigned predecessor ID: %ld\nPotential ID: %ld\n", chordID, predecessorID, inputChordID);

//Should be new ID if the distance 
if(((chordID - predecessorID) > (chordID - inputChordID) || predecessorID == chordID) && (chordID != inputChordID))
{
printf("Potential should be the new ID\n");
}
else
{
printf("Potential should not be the new ID\n");
}


return 0;
}
