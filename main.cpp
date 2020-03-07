//============================================================================
// Name        : IteratedPD.cpp
// Author      : Caglar Dogan - 2018400072
// Version     : 1.1
// Copyright   : -
// Description : Iterated PD in C++
//============================================================================

//TODO: Optimize simulation

#include <bits/stdc++.h>

#include <iostream>
#include <utility>

#include <stdlib.h>
#include <fstream>
#include <ctime>

using namespace std;

class Agent {
private:

public:
    unsigned short int c; //current number of people in memory
	long memory[200];
	unsigned short int id;
	unsigned short int M; //memory (from 0 to 99)
	unsigned short int P; //defection rate (from 0 to 100)
	long F; //Fitness

	unordered_set  <unsigned short int> collaborators;
	unordered_set  <unsigned short int> defectors;

	Agent() {
		this->id = 100;
		this->M = 50;
		this->P = 50;
		this->c = 0;
		this->F = 0;

	}
	Agent(unsigned short int ID, unsigned short int M, unsigned short int P) {
		this->id = ID;
		this->M = M;
		this->P = P;
		this->c = 0;
		this->F = 0;
	}

	bool choose(Agent* A) {
	    if(this->M == 0){
            return true; //if the agent can't remember anyone
	    }
		unsigned short int id = A->id;
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
        unsigned short int id1=rand() % 100;
        unsigned short int id2=id1;
        while(id1==id2){
            id2=rand() % 100;
        }
        if(agents[id1]->choose(agents[id2]) && agents[id2]->choose(agents[id1])){ //if both agents agree to play

            unsigned short int roll1 = rand() % 100+1;
            unsigned short int roll2 = rand() % 100+1;


            if(agents[id1]->memory[id2*2] == 0 && agents[id1]->M != 0){
                //agents[id1]->collaborators.insert(id2); //assumes to be collaborator at first : line (1) - does not work properly
                agents[id1]->c++;
            }

            if(agents[id2]->memory[id1*2] == 0 && agents[id2]->M != 0){
                agents[id2]->c++;
                //agents[id2]->collaborators.insert(id1); //assumes to be collaborator at first : line (2) - does not work properly
            }

            agents[id1]->memory[id2*2]++;
            agents[id2]->memory[id1*2]++;

            if(roll1 <= agents[id1]->P && roll2 <= agents[id2]->P){ //if they both defect
                agents[id1]->F += P;
                agents[id2]->F += P;

                if(agents[id1]->M != 0){ //wouldn't have needed this if line(1) worked
                    agents[id1]->collaborators.insert(id2);
                }
                if(agents[id2]->M != 0){ //wouldn't have needed this if line(1) worked
                    agents[id2]->collaborators.insert(id1);
                }

                agents[id1]->memory[id2*2+1]++;
                if(((double(agents[id1]->memory[id2*2+1])/ agents[id1]->memory[id2*2])> 0.5)  && agents[id1]->M != 0){
                    agents[id1]->collaborators.erase(agents[id1]->collaborators.find(id2));
                    agents[id1]->defectors.insert(id2);
                }

                agents[id2]->memory[id1*2+1]++;
                if(((double(agents[id2]->memory[id1*2+1])/ agents[id2]->memory[id1*2])> 0.5)  && agents[id2]->M != 0){
                    agents[id2]->collaborators.erase(agents[id2]->collaborators.find(id1));
                    agents[id2]->defectors.insert(id1);
                }
            }
            else if(roll1 <= agents[id1]->P && roll2 > agents[id2]->P){ //if agent 1 defects
                agents[id1]->F += T;
                agents[id2]->F += S;

                if(agents[id1]->M != 0){ //wouldn't have needed this if line(1) worked
                    agents[id1]->collaborators.insert(id2);
                }
                if(agents[id2]->M != 0){ //wouldn't have needed this if line(2) worked
                    agents[id2]->collaborators.insert(id1);
                }

                agents[id2]->memory[id1*2+1]++;
                if(((double(agents[id2]->memory[id1*2+1])/ agents[id2]->memory[id1*2])> 0.5)  && agents[id2]->M != 0){
                    agents[id2]->collaborators.insert(id1);
                    agents[id2]->collaborators.erase(agents[id2]->collaborators.find(id1));
                    agents[id2]->defectors.insert(id1);
                }
            }
            else if(roll1 > agents[id1]->P && roll2 <= agents[id2]->P){ //if agent 2 defects
                agents[id1]->F += S;
                agents[id2]->F += T;

                if(agents[id1]->M != 0){ //wouldn't have needed this if line(1) worked
                    agents[id1]->collaborators.insert(id2);
                }
                if(agents[id2]->M != 0){ //wouldn't have needed this if line(2) worked
                    agents[id2]->collaborators.insert(id1);
                }

                agents[id1]->memory[id2*2+1]++;
                if(((double(agents[id1]->memory[id2*2+1])/ agents[id1]->memory[id2*2])> 0.5) && agents[id1]->M != 0){
                    agents[id1]->collaborators.erase(agents[id1]->collaborators.find(id2));
                    agents[id1]->defectors.insert(id2);
                }
            }
            else {  //if they both COOPERATE

                if(agents[id1]->M != 0){ //wouldn't have needed this if line(1) worked
                    agents[id1]->collaborators.insert(id2);
                }
                if(agents[id2]->M != 0){ //wouldn't have needed this if line(2) worked
                    agents[id2]->collaborators.insert(id1);
                }

                agents[id1]->F += R;
                agents[id2]->F += R;
            }

            //deletion process of memory starts here

            if(agents[id1]->c > agents[id1]->M && agents[id1]->M !=0){ //if memory deletion is necessary
                unsigned short int col = agents[id1]->collaborators.size();
                if(col != 0){
                    unsigned short int index = rand() % col;
                    unordered_set<unsigned short int>::iterator it = agents[id1]->collaborators.begin();
                    advance(it,index);
                    agents[id1]->memory[(*it)*2] = 0;
                    agents[id1]->memory[(*it)*2+1] = 0;
                    agents[id1]->collaborators.erase(it);
                }
                else{
                    unsigned short int index = rand() % agents[id1]->defectors.size();
                    unordered_set<unsigned short int>::iterator it = agents[id1]->defectors.begin();
                    advance(it,index);
                    agents[id1]->memory[(*it)*2] = 0;
                    agents[id1]->memory[(*it)*2+1] = 0;
                    agents[id1]->defectors.erase(it);
                }
                agents[id1]->c--;
            }

            if(agents[id2]->c > agents[id2]->M  && agents[id2]->M !=0){ //if memory deletion is necessary
                unsigned short int col = agents[id2]->collaborators.size();
                if(col != 0){
                    unsigned short int index = rand() % col;
                    unordered_set<unsigned short int>::iterator it = agents[id2]->collaborators.begin();
                    advance(it,index);
                    agents[id2]->memory[(*it)*2] = 0;
                    agents[id2]->memory[(*it)*2+1] = 0;
                    agents[id2]->collaborators.erase(it);
                }
                else{
                    unsigned short int index = rand() % agents[id2]->defectors.size();
                    unordered_set<unsigned short int>::iterator it = agents[id2]->defectors.begin();
                    advance(it,index);
                    agents[id2]->memory[(*it)*2] = 0;
                    agents[id2]->memory[(*it)*2+1] = 0;
                    agents[id2]->defectors.erase(it);
                }
                agents[id2]->c--;
            }

        }
    }
}

void createNewGeneration(Agent* (&agents)[100]){ //Using wheel selection for reproducers, creates 100 new agents and sets up the next generation
    srand(time(NULL));
    long long mini = agents[0]->F; //starting normalization
    long long sum = 0;
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
        long long counted = 0;
        long long r = rand() % sum;
        for(int j = 0; j<100; j++){
            counted += agents[j]->F;
            if(counted > r){
                unsigned short int mutationRoll = rand() % 40; //1 in 20 chance total for mutation
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
	for (unsigned short int i = 0; i < 100; i++) { //initialization of the first generation (homogeneous distribution)
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
