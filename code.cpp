#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <iomanip>
using namespace std;

const int maxn = 11000;
const int maxd = 30;
const long double eps = 1e-6;

int n,d,k;

long double point[maxn][maxd];//point[i][j] is the jth coordinate of the ith point

//Reading the input points. Each point comes in a separate line and the coordinates are separated by comma.
void readInput(){
	ifstream fin("input.csv");
	for (int i=0 ; i<n ; ++i)
		for (int j=0 ; j<d ; ++j){
			fin >> point[i][j];
			char c;
			if (j<d-1)
				fin >> c;
		}
}

//Computing the distance between the ith and jth points
long double compute_dist(int i , int j){
	long double ans = 0.0;
	for (int dd=0 ; dd<d ; ++dd)
		ans += (point[i][dd]-point[j][dd])*(point[i][dd]-point[j][dd]);
	return sqrt(ans);
}

long double r[maxn];//fair radius of each point
long double temp[maxn];
long double compute_r(int i){//computes the fair radius for each point
	for (int j=0 ; j<n ; ++j)
		temp[j] = compute_dist(i,j);
	sort(temp,temp+n);
	return temp[(n-1)/k];
}


bool covered[maxn];//shows if a point is covered by current centers
int center[maxn];//shows the corresponding center for each point
int compute_balls(long double alpha){//computes a set of balls with respect to the fairness parameter alpha such that each point x, has a center within distance alpha*r[x]
	memset(covered,0,sizeof(covered));
	int center_number=0;
	while (true){
		int new_c =-1;
		long double dist = 1e100;
		for (int i=0 ; i<n ; ++i)
			if (!covered[i] && r[i]<dist){
				dist = r[i];
				new_c = i;
			}
		if (new_c ==-1)
			break;
		center_number++;
		for (int i=0 ; i<n ; ++i)
			if (!covered[i] && compute_dist(new_c,i)<=alpha*r[i]+eps){
				covered[i] = true;
 				center[i] = new_c;
			}
	}
	return center_number;
}

//this function performs a binary search to find the right value of alpha for which the number of balls becomes exactly k
//this function is used in Jung et al 's algorithm for finding k centers
long double find_alpha(){
	if (compute_balls(2.0)==k)
		return 2.0;
	long double b = 1.0, e=2.0;
	while (b<e-eps){
		long double med = (b+e)/2.0;
		int num = compute_balls(med);
		if (num < k)
			e = med;
		else
			b = med;
	}
	if (compute_balls(b)!=k){
		cerr << "------------error: not k balls in Jung's algorithm " << endl;
		b = e;
	}
	return b;
}

//this function computes the k-median cost using the array center.
long double compute_k_median_cost(){
	long double ans = 0.0;
	for (int i=0 ; i<n ; ++i){
		long double d = compute_dist(i,center[i]);
		ans += d;
	}
	return ans;
}


vector <int> balls; //keeps the critical balls used by our algorithm
vector <int> centers; // the set of centers found by our algorithm
vector <int> cnt; //keeps track of the number of centers in each ball
int whichBall[maxn];//keeps for each point in which critical ball they exist or -1 otherwise
bool iscenter[maxn];//keeps for each point if it is in the current set of centers

void add_center(){//this function is used in the initialization step and adds one more center that is furthest away from current centers
	long double dist = -1.0;
	int candidate = -1;
	for (int i=0 ; i<n ; ++i)
		if (center[i]!=i && compute_dist(i,center[i])>dist){
			dist = compute_dist(i,center[i]);
			candidate = i;
		}
	centers.push_back(candidate);
	iscenter[candidate] = true;
	if (whichBall[candidate]!=-1)
		cnt[whichBall[candidate]]++;
	for (int i=0 ; i<n ; ++i)
		if (compute_dist(i,candidate)<compute_dist(i,center[i]))
			center[i] = candidate;
}


int output_crit_balls;//for outputting the number of critical balls

void Local_Search_initialization(long double alpha){//initialization for the local search algorithm
	memset(iscenter,0,sizeof(iscenter));
	memset(whichBall,-1,sizeof(whichBall));

	int cnum = compute_balls(3*alpha);//computes critical balls
	output_crit_balls = cnum;
	for (int i=0 ; i<n ; ++i)//setting centers to balls and other initializations
		if (center[i]==i){
			balls.push_back(i);
			centers.push_back(i);
			cnt.push_back(1);
			iscenter[i] = true;
		}
	for (int i=0 ; i<n ; ++i)//setting value for whichBall
		for (int j=0 ; j<balls.size() ; ++j)
			if (compute_dist(i,balls[j])<alpha*r[balls[j]])
				whichBall[i]=j;
	for (int i=0 ; i<n ; ++i)
		for (int j=0 ; j< balls.size() ; ++j)
			if (compute_dist(i,balls[j]) < compute_dist(i,center[i]))
				center[i] = balls[j];
	for (int i=cnum ; i<k ; ++i)
		add_center();
}

long double compute_cost(){//computes k-median cost using the vectors centers
	long double ans = 0.0;
	for (int i=0 ; i<n ; ++i){
		long double dis = 1e100;
		for (int j=0 ; j<centers.size() ; ++j)
			if (compute_dist(i,centers[j])<dis)
				dis = compute_dist(i,centers[j]);
		ans += dis;
	}
	return ans;
}

long double evaluate_fairness(){//computes how fair a set of centers are
	long double maxF = 0.0;
	for (int i=0 ; i<n ; ++i){
		long double dis = 1e100;
		for (int j=0 ; j<centers.size() ; ++j)
			if (dis > compute_dist(i,centers[j])){
				dis = compute_dist(i,centers[j]);
			}
		if (dis/r[i] > maxF)
			maxF = dis/r[i];
	}
	return maxF;
}

long double output_init_fair, output_init_cost; //keeps the cost and fairness of the solution right after the initialization step

long double Local_Search(long double alpha){//local search algorithm
	Local_Search_initialization(alpha);
	output_init_cost = compute_cost();
	output_init_fair = evaluate_fairness();
	bool improved = true;
	while (improved){
		improved = false;
		for (int i=0 ; i<centers.size() ; ++i)
			for (int j=0 ; j<n ; ++j)
				if (!iscenter[j]){
					int b = whichBall[centers[i]];
					if (b==-1 || cnt[b]>1 || whichBall[j]==b){//checks for feasibility
						long double prev_cost = compute_cost();
						int prev_c = centers[i];
						centers[i] = j;
						if (compute_cost()<prev_cost*0.99){//checks if the cost is improved
							iscenter[j] = true;
							iscenter[prev_c] = false;
							if (whichBall[prev_c]!=-1)
								cnt[whichBall[prev_c]]--;
							if (whichBall[j]!=-1)
								cnt[whichBall[j]]++;
							improved = true;
						}
						else
							centers[i] = prev_c;
					}
				}
	}
	return compute_cost();
}


//long double output_jung_fair, output_jung_cost; //fairness and cost of Jung et al.'s algorithm
long double output_ls_fair, output_ls_cost;//fairness and cost of local search
void generate_output(){
	ofstream fout("output.csv",ofstream::app);
	fout << n << ",";
	fout << d << ",";
	fout << k << ",";
	
	fout << output_crit_balls << ",";
	
	//fout << output_jung_fair << ",";
	//fout << output_jung_cost << ",";
	fout << output_init_fair << ",";
	fout << output_init_cost << ",";
	fout << output_ls_fair << ",";
	fout << output_ls_cost << endl;
	fout.close();
	return;
}

int main(){
	cout << "input n  d  k" << endl;
	cin >> n >> d >> k;
	readInput();//read input
	for (int i=0 ; i<n ; ++i)//compute fair radiuses
		r[i] = compute_r(i);
	
	long double alpha = find_alpha();//find optimal alpha used by Jung et al.'s algorithm
	//output_jung_fair = alpha;
	compute_balls(alpha);//compute critical balls
	//output_jung_cost = compute_k_median_cost();//compute cost of 
	
	output_ls_cost = Local_Search(alpha);
	output_ls_fair = evaluate_fairness();
	
	generate_output();
	return 0;
}
