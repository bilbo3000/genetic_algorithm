/*
 * This program uses genetic algorithm to 
 * search for a specific string "AIandHeuristicSE". 
 * Each chromosome has length of 16 and each gene is
 * a ASC character. Initial population is 32 and the 
 * size changes to 16 later on. 
 * 
 * Dongpu Jin
 * 9/23/2012
 */
 
 #include <iostream>  
 #include <time.h> 
 #include <string> 
 #include <stdlib.h>
 #include <math.h>
 #include <assert.h>
 #include <fstream> 
 
 using namespace std; 
 
 /*
  * Function prototypes
  */
 void init(int** initPop, int initSize);
 int** crossover(int** pop, int insize, int outsize, int choice);
 int** crossover2(int** pop, int insize, int outsize, int choice);
 int randGen(); 
 int squaredFit(int chrom[16]);
 int binaryFit(int chrom[16]);
 void mutation(int ** pop, int size, double rate); 
 template <class T> void swap(T * a, int i, int j);
 int minFit(int ** pop, int size, int choice);
 void myprint(int** pop, int size, int choice); 
 
/*
 * Utility functions. 
 */
double diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
	return diffms;
}

int isPowerOfTwo (unsigned int x)
{
	while (((x % 2) == 0) && x > 1) /* While x is even and > 1 */
		x /= 2;
	return (x == 1);
}
 
 char target[16] = {'A', 'I', 'a', 'n', 'd', 'H', 'e', 'u', 'r', 'i', 's', 't', 'i', 'c', 'S', 'E'};
 ofstream outfile("data.csv"); // the output file
 
 int main(int argc, char* argv[]){
	// initialize the random seed
	srand(time(NULL));
	
	// prompt user to select fitness function
	string input; 
	int choice; 
	cout << "Please select a fitness function: " << endl; 
	cout << "1. Sqared Distance (default)" << endl; 
	cout << "2. Binary Distance" << endl; 
	cin >> input; 
	choice = atoi(input.c_str());
	if (choice != 2){
		choice = 1; // set to default if enter invalid number
	}
	
	// prompt user for mutation rate
	double rate; 
	cout << "Please enter mutation rate (0~1): ";
	cin >> input; 
	rate = atof(input.c_str());
	while (rate > 1 || rate < 0){ // rate has to be within (0, 1) range
		cout << "Please enter mutation rate (0~1): ";
		cin >> input; 
		rate = atof(input.c_str());
	}
	
	/*
	 * Population size has to be power of 2 and no less than 8. 
	 * Initial population size cannot be less than regular 
	 * population size. 
	 */
	int initSize = 32; // initial population size
	int pop = 16; // regular population size
	assert(isPowerOfTwo(initSize) && isPowerOfTwo(pop)); // ensure power of two
	assert(initSize >= pop); // ensure init size no less than regular population
	assert(initSize >= 8 && pop >= 8); // ensure min size
	

	int** initPop; // points to initial population 
	int** currPop; // points to current population
	initPop = new int * [initSize];
	for(int i = 0; i < initSize; i++){
		initPop[i] = new int[16];
	}
	
	init(initPop, initSize); // initialize the initial population
	currPop = crossover(initPop, initSize, pop, choice); // crossover
	// currPop = crossover2(initPop, initSize, pop, choice); 
	mutation(currPop, pop, rate); // mutation
	
	int min = minFit(currPop, pop, choice);
	long cnt = 0; // count number of generations
	clock_t begin = clock();
	while( min != 0){ // haven't found solution yet
		currPop = crossover(currPop, pop, pop, choice);
		// currPop = crossover2(currPop, pop, pop, choice);
		mutation(currPop, pop, rate); 
		min = minFit(currPop, pop, choice);
		cnt++; 
	}
	clock_t end = clock();
	cout << endl; 
	cout << "Current population: " << endl; 
	myprint(currPop, pop, choice);
	if(choice == 1)
		cout << "Squared distance fitness function (#1)" << endl; 
	else
		cout << "Binary distance fitness function (#2)" << endl;
	cout << "Mutation rate: " << rate << endl; 
	cout << "Number of iteration: " << cnt << endl; 
	cout << "Total time: " << diffclock(end, begin) << "ms" << endl;
	
	outfile.close(); 
	
	return 0; 
 }
 
 /*
  * initialization initial population
  */
 void init(int** initPop, int initSize){
	for(int i = 0; i < initSize; i++){
		for(int j = 0; j < 16; j++){
			initPop[i][j] = randGen(); 
		}
	}
 }
 
 /*
  * Crossover chromosomes based on their fitness ranking.
  */
 int** crossover(int** pop, int insize, int outsize, int choice){
	int * fit = new int[insize]; // hold the fitness for each chromosome
	for(int i = 0; i < insize; i++){
		if(choice == 1)  // choice 1. squared distance fitness 
			fit[i] = squaredFit(pop[i]);
		else // choice 2. binary distance fitness 
			fit[i] = binaryFit(pop[i]); 
	}
	
	// sort chromosomes based on fitness ranking(bubble sort)
	for(int i = 0; i < insize; i++){
		for(int j = i + 1; j < insize; j++){
			if(fit[i] > fit[j]){
				swap<int>(fit, i, j);  // sort fit (int element) 
				swap<int *>(pop, i, j); // sort chromosomes(ptr)
			}
		}
	}
	
	// deep copy top ranked half chromosomes to top half and 
	// bottom half of the new population
	int ** newpop = new int*[outsize];
	for(int i = 0; i < outsize; i++){
		newpop[i] = new int[16]; 
	}
	for(int i = 0; i < outsize / 2; i++){
		for(int j = 0; j < 16; j++){
			newpop[i][j] = pop[i][j]; 
			newpop[i + outsize / 2][j] = pop[i][j]; 
		}
	}
	 
	// one point crossover the bottom half (swap entire chunk)
	for(int i = outsize / 2; i < outsize; i++){
		// crossover the even pair
		int a = rand() % 16;    // find a point
		if(a < 16){
			int * temp = new int[16 - a]; // temporary holder
			for(int m = a; m < 16; m++){  // swap entire chunk after the point
				temp[m - a] = newpop[i][m]; 
				newpop[i][m] = newpop[i + 2][m];
				newpop[i + 2][m] = temp[m - a]; 
			}
			delete [] temp; // delete temp
		}
		
		// crossover the odd pair
		a = rand() % 16;     // find a point
		if(a < 16){
			int * temp = new int[16 - a]; // temporary holder
			for(int m = a; m < 16; m++){  // swap entire chunk after the point
				temp[m - a] = newpop[i + 1][m]; 
				newpop[i + 1][m] = newpop[i + 3][m];
				newpop[i + 3][m] = temp[m - a]; 
			}
			delete [] temp; // delete temp
		}
		
		i = i + 3;
	}
	
	// delete space for old population
	for(int i = 0; i < insize; i++){
		delete [] pop[i];
	}
	delete [] pop; 
	return newpop;  
 }
 
int** crossover2(int** pop, int insize, int outsize, int choice){
	int * fit = new int[insize]; // hold the fitness for each chromosome
	for(int i = 0; i < insize; i++){
		if(choice == 1)  // choice 1. squared distance fitness 
			fit[i] = squaredFit(pop[i]);
		else // choice 2. binary distance fitness 
			fit[i] = binaryFit(pop[i]); 
	}
	
	// sort chromosomes based on fitness ranking(bubble sort)
	for(int i = 0; i < insize; i++){
		for(int j = i + 1; j < insize; j++){
			if(fit[i] > fit[j]){
				swap<int>(fit, i, j);  // sort fit (int element) 
				swap<int *>(pop, i, j); // sort chromosomes(ptr)
			}
		}
	}
	
	// deep copy top ranked chromosomes to top half and 
	// bottom half of the new population
	int ** newpop = new int*[outsize];
	for(int i = 0; i < outsize; i++){
		newpop[i] = new int[16]; 
	}
	for(int i = 0; i < outsize / 2; i++){
		for(int j = 0; j < 16; j++){
			newpop[i][j] = pop[i][j]; 
			newpop[i + outsize / 2][j] = pop[i][j]; 
		}
	}
	
	// crossover, only swap two elements
	for(int i = outsize / 2; i < outsize; i++){
		// crossover the even pair
		int a = rand() % 16;    // find a point in first parent
		int b = rand() % 16;    // find a point in second parent
		int temp = newpop[i][a];
		newpop[i][a] = newpop[i + 2][b]; 
		newpop[i + 2][b] = temp; 
		
		// crossover the odd pair
		a = rand() % 16;    // find a point in first parent
		b = rand() % 16;    // find a point in second parent
		temp = newpop[i + 1][a]; 
		newpop[i + 1][a] = newpop[i + 3][b]; 
		newpop[i + 3][b] = temp; 
		
		i = i + 3;
	}
	
	// delete space for old population
	for(int i = 0; i < insize; i++){
		delete [] pop[i];
	}
	delete [] pop; 
	return newpop;  
}
 
 /*
  * Mutate the population at the given mutation rate. 
  */
 void mutation(int ** pop, int size, double rate){
	int n = ceil(size * 16 * rate); 
	int chrom, gene, randNum; 
	for(int i = 0; i < n; i++){
		chrom = rand() % size; 
		gene = rand() % 16; 
		randNum = randGen(); 
		// mutate iff a different character
		while(randNum == pop[chrom][gene]){
			randNum = randGen(); 
		}
		pop[chrom][gene] = randNum; 
	}
 } 
 
 /*
  * swap functoin template
  */
 template <class T>
 void swap(T * a, int i, int j){
	T temp;
	temp = a[i]; 
	a[i] = a[j]; 
	a[j] = temp;
 }
 
 /*
  * Squared distance fitness funtion. It sums the 
  * square of distances for each gene in a chromosome. 
  */
 int squaredFit(int chrom[16]){
	int fit = 0; 
	for(int i = 0; i < 16; i++){
		fit = fit + pow(chrom[i] - (int)target[i], 2);
	}
	return fit; 
 }
 
 /*
  * Binary distance fitness function. It uses 0, 1 to 
  * represent distance whether the char match or not. 
  * Then it sums all the distance. 
  */
 int binaryFit(int chrom[16]){
	int fit = 0; 
	for(int i = 0; i < 16; i++){
		if(chrom[i] - (int)target[i] != 0){
			fit += 1; // increment by 1 if not match
		}
	}
	return fit; 
 }
 
 /*
  * Generate random number within valid range
  */
 int randGen(){
	bool cap = 0; // 0: lower case; 1: capital  
	int result; 
	cap = rand() % 2; 
	if(cap){ // capital 
		result = rand() % 26 + 65; 
	}else{ // lower case
		result = rand() % 26 + 97;
	}
	return result;
 }
 /*
  * find the minmum (optimal) fitness within the given population
  */ 
 int minFit(int ** pop, int size, int choice){
	int min, fit; 
	if(choice == 1) {
		min = squaredFit(pop[0]);
		outfile << min;
	}
	else {
		min = binaryFit(pop[0]);
		outfile << min; 
	}
	
	for(int i = 1; i < size; i++){
		if(choice == 1) {
			fit = squaredFit(pop[i]);
			outfile << ',' << fit; 
			if(fit < min) min = fit; // upate min
		}
		else {
			fit = binaryFit(pop[i]);
			outfile << ',' << fit; 
			if(fit < min) min = fit; // udpate min
		}
	}
	outfile << '\n' << endl; 
	
	return min; 
 }
 
 /*
  * print the given population in characters
  */
 void myprint(int** pop, int size, int choice){
	for(int i = 0; i < size; i++){
		for(int j = 0; j < 16; j++){
			cout << (char)(pop[i][j]);
		}
		if(choice == 1){ // squared distance fitness 
			if (squaredFit(pop[i]) == 0){
				cout << "*"; 
			}
		}else{ // binary distance fitness
			if (binaryFit(pop[i]) == 0){
				cout << "*"; 
			}		
		}
		cout << endl; 
	}
	cout << endl; 
 }