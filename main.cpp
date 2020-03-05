//============================================================================
// Name        : IteratedPD.cpp
// Author      : Caglar Dogan - 2018400072
// Version     : 0.9
// Copyright   : -
// Description : Iterated PD in C++
//============================================================================

#include <iostream>
#include <utility>

#include <stdlib.h>
#include <fstream>
#include <ctime>

using namespace std;

class Agent {
private:

public:
    int c; //current number of people in memory
    int col; //current number of collaborators in memory
	int memory[200];
	int id;
	int M; //memory (from 0 to 99)
	int P; //defection rate (from 1 to 100)
	int F; //Fitness

	Agent() {
		this->id = -1;
		this->M = 50;
		this->P = 50;
		this->c = 0;
		this->col = 0;
		this->F = 0;

	}
	Agent(int ID, int M, int P) {
		this->id = ID;
		this->M = M;
		this->P = P;
		this->c = 0;
		this->col = 0;
		this->F = 0;
	}

	bool choose(Agent* A) {
	    if(this->M == 0){
            return true; //if the agent can't remember anyone
	    }
		int id = A->id;
		if (this->memory[id*2] == 0) { //if they have never played before
			return true;
		}
		else {
			if (double(this->memory[id*2+1]) / this->memory[id*2] <= 0.5) { //if target is a collaborator
				return true;
			}
			else { //if target is a defector
                return false;
			}
		}
	}

};

double averageMemory(Agent* agents[100]) { //returns the average memory of the current generation
	double result = 0.0;
	for (int i = 0; i < 100; i++) {
		result += agents[i]->M;
	}
	return (result/100.0);
}

double averageDeflection(Agent* agents[100]) { //returns the average defection chance multiplier of the current generation
	double result = 0.0;
	for (int i = 0; i < 100; i++) {
		result += agents[i]->P;
	}
	return (result / 100.0);
}

void simulateGeneration(Agent* agents[100], int S, int P, int R, int T ,int numberOfRounds){
    srand(time(NULL));
    for(int i=0; i<numberOfRounds; i++){
        int id1=rand() % 100;
        int id2=id1;
        while(id1==id2){
            id2=rand() % 100;
        }
        if(agents[id1]->choose(agents[id2]) && agents[id2]->choose(agents[id1])){ //if both agents agree to play
            int roll1 = rand() % 100+1;
            int roll2 = rand() % 100+1;

            if(agents[id1]->memory[id2*2] == 0 && agents[id1]->M != 0){
                agents[id1]->c++;
                agents[id1]->col++; //assume to be collaborator
            }

            if(agents[id2]->memory[id1*2] == 0 && agents[id2]->M != 0){
                agents[id2]->c++;
                agents[id2]->col++; //assume to be collaborator
            }

            if(roll1 <= agents[id1]->P && roll2 <= agents[id2]->P){ //if they both defect
                agents[id1]->F += P;
                agents[id2]->F += P;

                agents[id1]->memory[id2*2]++;
                agents[id1]->memory[id2*2+1]++;
                if((double(agents[id1]->memory[id2*2+1])/ agents[id1]->memory[id2*2]> 0.5) && agents[id1]->M != 0){
                    agents[id1]->col--; //mark as defector
                }

                agents[id2]->memory[id1*2]++;
                agents[id2]->memory[id1*2+1]++;
                if((double(agents[id2]->memory[id1*2+1])/ agents[id2]->memory[id1*2]> 0.5) && agents[id2]->M != 0){
                    agents[id2]->col--; //mark as defector
                }
            }
            else if(roll1 <= agents[id1]->P && roll2 > agents[id2]->P){ //if agent 1 defects
                agents[id1]->F += T;
                agents[id2]->F += S;

                agents[id1]->memory[id2*2]++;

                agents[id2]->memory[id1*2]++;
                agents[id2]->memory[id1*2+1]++;
                if((double(agents[id2]->memory[id1*2+1])/ agents[id2]->memory[id1*2]> 0.5)  && agents[id2]->M != 0){
                    agents[id2]->col--; //mark as defector
                }

            }
            else if(roll1 > agents[id1]->P && roll2 <= agents[id2]->P){ //if agent 2 defects
                agents[id1]->F += S;
                agents[id2]->F += T;

                agents[id1]->memory[id2*2]++;
                agents[id1]->memory[id2*2+1]++;
                if((double(agents[id1]->memory[id2*2+1])/ agents[id1]->memory[id2*2]> 0.5) && agents[id1]->M != 0){
                    agents[id1]->col--; //mark as defector
                }

                agents[id2]->memory[id1*2]++;

            }
            else {  //if they both COOPERATE
                agents[id1]->F += R;
                agents[id2]->F += R;

                agents[id1]->memory[id2*2]++;

                agents[id2]->memory[id1*2]++;

            }

            //deletion process of memory starts here

            if(agents[id1]->c > agents[id1]->M){ //if memory deletion is necessary TODO: FIX
                if(agents[id1]->col != 0){ //if there are collaborators in memory
                    int id=rand() % agents[id1]->col;
                    int idd = -1; //id of collaborator to be deleted
                    for(int j=0; j<=id; j++){
                        idd++;
                        if(idd >= 100) {
                                cout << "Error!1 ::" << idd << " ::"<< id <<endl;
                            }
                        bool b = false;
                        if(agents[id1]->memory[idd*2] == 0){
                            b = true;
                        }
                        else if (double(agents[id1]->memory[idd*2+1])/agents[id1]->memory[idd*2] > 0.5){
                            b = true;
                        }

                        while(b){ //continue until you find a collaborator
                            idd++;
                            if(idd >= 100) {
                                cout << "Error!1 ::" <<  idd << " ::"<< id <<endl;
                            }
                            if(agents[id1]->memory[idd*2] == 0){
                                b = true;
                            }
                            else if (double(agents[id1]->memory[idd*2+1])/agents[id1]->memory[idd*2] > 0.5){
                                b = true;
                            }
                            else {
                                b = false;
                            }
                        }
                    }
                    agents[id1]->memory[idd*2] = 0;
                    agents[id1]->memory[idd*2 + 1] = 0;
                    agents[id1]->col--;
                }
                else{ //if there are no collaborators in memory
                    int id=rand() % agents[id1]->c;
                    int idd = -1;
                    for(int j=0; j<=id; j++){
                        idd++;
                        while(agents[id1]->memory[idd*2] == 0){
                            idd++;
                        }
                    }
                    if(idd >= 100) {
                                cout << "Error!1 ::" <<  idd << " ::"<< id <<endl;
                            }
                    agents[id1]->memory[idd*2] = 0;
                    agents[id1]->memory[idd*2 + 1] = 0;
                }
                agents[id1]->c--;
            }

            if(agents[id2]->c > agents[id2]->M){ //if memory deletion is necessary TODO: FIX
                if(agents[id2]->col != 0){ //if there are collaborators in memory
                    int id=rand() % agents[id2]->col;
                    int idd = -1; //id of collaborator to be deleted
                    for(int j=0; j<=id; j++){
                        idd++;
                        if(idd >= 100) {
                                cout << "Error!2 ::" <<  idd << " ::"<< id <<endl;
                        }
                        bool b = false;
                        if(agents[id2]->memory[idd*2] == 0){
                            b = true;
                        }
                        else if (double(agents[id2]->memory[idd*2+1])/agents[id2]->memory[idd*2] > 0.5){
                            b = true;
                        }

                        while(b){ //continue until you find a collaborator
                            idd++;
                            if(idd >= 100) {
                                cout << "Error!2 ::" << idd << " ::"<< id <<endl;
                            }
                            if(agents[id2]->memory[idd*2] == 0){
                                b = true;
                            }
                            else if (double(agents[id2]->memory[idd*2+1])/agents[id2]->memory[idd*2] > 0.5){
                                b = true;
                            }
                            else {
                                b = false;
                            }
                        }
                    }
                    agents[id2]->memory[idd*2] = 0;
                    agents[id2]->memory[idd*2 + 1] = 0;
                    agents[id2]->col--;
                }
                else{  //if there are no collaborators in memory
                    int id=rand() % agents[id2]->c;
                    int idd = -1;
                    for(int j=0; j<=id; j++){
                        idd++;
                        while(agents[id2]->memory[idd*2] == 0){
                            idd++;
                        }
                    }
                    if(idd >= 100) {
                        cout << "Error!2 ::" <<  idd << "::"<< id <<endl;
                    }
                    agents[id2]->memory[idd*2] = 0;
                    agents[id2]->memory[idd*2 + 1] = 0;
                }

                agents[id2]->c--;
            }

        }
    }
}

void createNewGeneration(Agent* (&agents)[100]){ //Using wheel selection for reproducers, creates 100 new agents and sets up the next generation
    srand(time(NULL));
    int mini = agents[0]->F; //starting normalization
    int sum = 0;
    for(int i=0; i<100; i++){
        sum += agents[i]->F;
        if(agents[i]->F<mini){
            mini = agents[i]->F;
        }
    }
    sum = sum - (100*mini);
    for(int i=0; i<100; i++){
        agents[i]->F = agents[i]->F - mini;
    }
    //creating new agents
    Agent* newAgents[100];
    for(int i=0; i<100; i++){
        int counted = 0;
        int r = rand() % sum;
        for(int j = 0; j<100; j++){
            counted += agents[j]->F;
            if(counted > r){
                int mutationRoll = rand() % 40; //1 in 20 chance total for mutation
                if(mutationRoll <= 1){
                    if(mutationRoll == 0){
                        newAgents[i] = new Agent(i, rand() % 100, agents[j]->P); // memory can be 99 at most
                    }
                    else {
                        newAgents[i] = new Agent(i, agents[j]->M, rand() % 101);
                    }
                }
                else {
                    newAgents[i] = new Agent(i, agents[j]->M, agents[j]->P);
                }
                break;
            }
        }
    }
    //setting up the new agents as the current population and deleting last generation
    for(int i=0; i<100; i++){
        delete agents[i];
        agents[i] = newAgents[i];
    }
}


int main()
{
    srand(time(NULL));
	const int NUMBER_OF_GEN = 500; //Number of generations
	cout << "Initializing first generation:" << endl;
	Agent *agents[100];
	for (int i = 0; i < 100; i++) { //initialization of the first generation (homogeneous distribution)
		agents[i] = new Agent(i, rand() % 100, rand() % 101); // memory can be 99 at most
	}
	cout << "Done!" << endl;

	ofstream outfile;
	outfile.open("outputfile.txt");

	cout << "Simulating Evolution..." << endl;
	outfile << "Generation,Average Memory,Average Deflection Rate" << endl;
	outfile << "0," << averageMemory(agents)/100.0 << "," << averageDeflection(agents)/100.0 << endl;
	for (int i = 1; i <= NUMBER_OF_GEN; i++) {
		cout << "Simulating Generation " << i << "... " << endl;
		simulateGeneration(agents, 0, 1, 3, 5, 148500); //148500 rounds used in original paper, each pairing occurs approximately 30 times
		cout << "Preparing New Generation...  ";
		createNewGeneration(agents);
        cout << "Saving Results... " << endl;
		outfile << i << "," << (averageMemory(agents)/99.0) << "," << (averageDeflection(agents)/100.0) << endl;
	}
	cout << "Simulation Complete." << endl;

    outfile.close();

	for(int i = 0; i < 100; i++){ //cleans up memory
        delete agents[i];
	}

	return 0;
}
