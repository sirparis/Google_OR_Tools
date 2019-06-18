#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <set>
#include <vector>
#include <list>
#include <cmath>
using namespace std;

int stu_no; // αριθμός σπουδαστών
int crs_no; // αριθμός μαθημάτων
int per_no; // αριθμός περιόδων εξέτασης (δίνεται στην εκφώνηση)
int* crs_per_stu_n; // πίνακας με αριθμό μαθημάτων ανά σπουδαστή
int** crs_per_stu; // 2Δ πίνακας με τους αριθμούς των μαθημάτων ανά σπουδαστή
int* stu_per_crs_n; // πίνακας με τον αριθμό των σπουδαστών ανά μάθημα
int** stu_per_crs; // 2Δ πίνακας με τους αριθμούς των σπουδαστών ανά μάθημα
int** conflict_matrix; // 2Δ πίνακας συγκρούσεων
int* sol; // πίνακας με περίοδο στην οποία τοποθετείται η κάθε εξέταση μαθήματος
int v;


void print_conflict_density() {
	int c = 0;
	for (int i = 1; i < crs_no + 1; i++)
		for (int j = 1; j < crs_no + 1; j++)
			if (conflict_matrix[i][j] == 1)
				c++;
	double conflictDensity = (double) c / (crs_no * crs_no);
	cout << "Conflict density is " << conflictDensity << endl;
}

void read_problem_data(string problem) {
	string stu_fn = "";
	string grafos = "";
	if (problem == "TINY") {
		stu_fn = "tiny.stu";
		stu_no = 10;
		v=crs_no = 5;
		per_no = 5;
		//		graph=read_data("TINY_GRAPH.txt",v);
	} else if (problem == "CAR91-II") {
		stu_fn = "car-s-91-II.stu";
		stu_no = 16925;
		crs_no = 682;
		per_no = 35;
	} else if (problem == "CAR91") {
		stu_fn = "car-s-91.stu";
		stu_no = 16925;
		v=crs_no = 682;
		per_no = 35;
		grafos= "CAR91_GRAPH.txt";
	} else if (problem == "KFU93") {
		stu_fn = "kfu-s-93.stu";
		stu_no = 5349;
		v=crs_no = 461;
		per_no = 20;
		grafos= "KFU93_GRAPH.txt";
	} else if (problem == "LSE91") {
		stu_fn = "lse-f-91.stu";
		stu_no = 2726;
		v=crs_no = 381;
		per_no = 18;
		grafos= "LSE91_GRAPH.txt";
	} else if (problem == "TRE92") {
		stu_fn = "tre-s-92.stu";
		stu_no = 4360;
		v=crs_no = 261;
		grafos= "tre92_GRAPH.txt";
		per_no = 23;
	} else if (problem == "UTE92") {
		stu_fn = "ute-s-92.stu";
		stu_no = 2750;
		v=crs_no = 184;
		per_no = 10;
		grafos= "Ute92_GRAPH.txt";
	} else {
		cerr << "Not implemented";
		exit(-1);
	}
	crs_per_stu_n = new int[stu_no + 1](); // δημιουργία πίνακα και αρχικοποίηση με 0
	crs_per_stu = new int*[stu_no + 1]();
	stu_per_crs_n = new int[crs_no + 1]();
	stu_per_crs = new int*[crs_no + 1]();
	conflict_matrix = new int*[crs_no + 1]();
	for (int i = 0; i < crs_no + 1; i++) {
		conflict_matrix[i] = new int[crs_no + 1]();
	}
	fstream filestr;
	string buffer;
	filestr.open(stu_fn.c_str());
	int stu_index = 1;
	if (filestr.is_open()) {
		while (getline(filestr, buffer)) {
			string buffer2;
			stringstream ss;
			ss.str(buffer); // αρχικοποίηση stringstream
			int crs = 0; // αριθμός μαθημάτων στα οποία θα εξεταστεί ο σπουδαστής
			while (ss >> buffer2) {
				int crs_index = atoi(buffer2.c_str());
				crs++;
				stu_per_crs_n[crs_index]++;
			}
			crs_per_stu_n[stu_index] = crs;
			crs_per_stu[stu_index] = new int[crs]();
			ss.clear();
			ss.str(buffer);
			int j = 0;
			while (ss >> buffer2) {
				int crs_index = atoi(buffer2.c_str());
				crs_per_stu[stu_index][j] = crs_index;
				j++;
			}
			stu_index++;
		}
		filestr.close();
		// δημιουργία πίνακα crs_per_stu
		for (int i = 1; i < crs_no + 1; i++) {
			int students = stu_per_crs_n[i];
			stu_per_crs[i] = new int[students]();
		}
		int temp[crs_no + 1];
		for (int i = 0; i <= crs_no; i++)
			temp[i] = 0;
		for (int i = 1; i < stu_no + 1; i++) {
			for (int j = 0; j < crs_per_stu_n[i]; j++) {
				int k = crs_per_stu[i][j];
				stu_per_crs[k][temp[k]] = i;
				temp[k]++;
			}
		}
		// δημιουργία πίνακα conflict_matrix
		for (int exam1 = 1; exam1 < crs_no + 1; exam1++) {
			for (int exam2 = exam1 + 1; exam2 < crs_no + 1; exam2++) {
				bool f = true;
				for (int i = 0; i < stu_per_crs_n[exam1] && f; i++)
					for (int j = 0; j < stu_per_crs_n[exam2] && f; j++)
						if (stu_per_crs[exam1][i] == stu_per_crs[exam2][j]) {
							conflict_matrix[exam1][exam2] = 1;
							conflict_matrix[exam2][exam1] = 1;
							f = false;
						}
			}
		}
	} else {
		cout << "Error opening file: " << stu_fn << endl;
		exit(-1);
	}
}

list<pair<int, int> >* read_data(string fn, int V) {
	list<pair<int, int> >* graph = new list<pair<int, int> > [V + 1]();
	fstream filestr;
	string buffer;
	filestr.open(fn.c_str());
	if (filestr.is_open()) {
		while (getline(filestr, buffer)) {
			string buffer2;
			stringstream ss;
			ss.str(buffer);
			vector<string> tokens;
			while (ss >> buffer2) {
				tokens.push_back(buffer2);
			}
			int vertex1 = atoi(tokens[0].c_str());
			for (unsigned int i = 1; i < tokens.size(); i++) {
				int pos = tokens[i].find(",");

				int vertex2 = atoi(tokens[i].substr(0, pos).c_str());
				int weight =
						atoi(
								tokens[i].substr(pos + 1,
										tokens[i].length() - 1).c_str());
				graph[vertex1].push_back(make_pair(vertex2, weight));
			}
		}
	} else {
		cout << "Error opening file: " << fn << endl;
		exit(-1);
	}
	return graph;
}

void print_graph(list<pair<int, int> >* g, int V) {
	int k = 1;
	for (int i = 1; i <= V; i++) {
		for (pair<int, int> p : g[i]) {
			printf("%d<--%d-->%d\t", i, p.first, p.second);
			if (k % v == 0)
				printf("\n");
			k++;
		}
	}
	printf("\n");
}

void compute_shortest_paths_to_all_vertices(list<pair<int, int> >* g, int V,
		int source, int* distance) {
	list<int> S;
	set<int> V_S;
	for (int i = 1; i <= V; i++) {
		if (i == source) {
			S.push_back(i);
			distance[i] = 0;
		} else {
			V_S.insert(i);
			distance[i] = INT_MAX;
		}
	}
	while (!V_S.empty()) {
		int min = INT_MAX;
		int pmin = -1;
		for (int v1 : S) {
			for (pair<int, int> w_v : g[v1]) {
				int weight = w_v.first;
				int v2 = w_v.second;
				bool is_in_V_S = V_S.find(v2) != V_S.end();
				if ((is_in_V_S) && (distance[v1] + weight < min)) {
					min = distance[v1] + weight;
					pmin = v2;
				}
			}
		}
		// The graph might not be connected
		if (pmin == -1)
			break;
		distance[pmin] = min;
		S.push_back(pmin);
		V_S.erase(pmin);
	}
}

void load_graph(string problem)
{
	list<pair<int, int> >* graph;
	string arxeio="";

	if (problem == "TINY")  graph = read_data("TINY_GRAPH.txt",v);
	if (problem == "CAR91") graph = read_data("CAR91_GRAPH.txt",v);
	if (problem == "KFU93") graph = read_data("KFU93_GRAPH.txt",v);
	if (problem == "LSE91") graph = read_data("LSE91_GRAPH.txt",v);
	if (problem == "TRE92") graph = read_data("tre92_GRAPH.txt",v);
	if (problem == "UTE92") graph = read_data("Ute92_GRAPH.txt",v);

	int max_min_path=0;
	for (int source_vertex = 1; source_vertex <= v; source_vertex++)
	{
		int* shortest_path_distances = new int[v + 1];
		// shortest path lengths from vertex 1 to all other vertices
		compute_shortest_paths_to_all_vertices(graph, v, source_vertex,shortest_path_distances);
		for (int destination_vertex = source_vertex + 1;
				destination_vertex <= v; destination_vertex++) {
			if (shortest_path_distances[destination_vertex] == INT_MAX)
				continue;
			if (shortest_path_distances[destination_vertex]>max_min_path) max_min_path=shortest_path_distances[destination_vertex];
			//printf("vertex:%d to vertex:%d shortest path distance=%d\n",source_vertex, destination_vertex,shortest_path_distances[destination_vertex]);
		}
		delete[] shortest_path_distances;
	}
	cout<<"H diametros tou grafimatos einai :"<<max_min_path<<endl;
}

void load_solution(string problem) {
	sol = new int[crs_no + 1] {};
	fstream filestr;
	string buffer;
	string sol_fn = "";

	if (problem == "TINY")  sol_fn ="tiny.sol";
	if (problem == "CAR91") sol_fn ="car-s-91.sol";
	if (problem == "KFU93") sol_fn ="kfu-s-93.sol";
	if (problem == "LSE91") sol_fn ="lse-f-91.sol";
	if (problem == "TRE92") sol_fn ="tre-s-92.sol";
	if (problem == "UTE92") sol_fn ="Ute-s-92.sol";

	filestr.open(sol_fn.c_str());
	if (filestr.is_open()) {
		while (getline(filestr, buffer)) {
			string buffer2;
			stringstream ss;
			ss.str(buffer);
			ss >> buffer2;
			int crs_index = atoi(buffer2.c_str());
			ss >> buffer2;
			int period = atoi(buffer2.c_str());
			sol[crs_index] = period;
		}
		filestr.close();
	} else {
		cerr << "file not found" << endl;
	}
}//telos load_solution








int main(int argc, char **argv) {
	int choice;
	string prob;
	do {
		cout << "1. Fortosi Problimatos" << endl;
		cout << "2. Statistika" << endl;
		cout << "3. Grafos problimatos" << endl;
		cout << "4. Fortwsi lisis - ypologismos kostous" << endl;
		cout << "5. Exit" << endl;
		cout << "Choise: ";
		cin >> choice;
		switch (choice) {
		case 1:
			cout
			<< "Give name of problem [TINY|CAR91|KFU93|LSE91|TRE92|UTE92]: ";
			cin >> prob;
			read_problem_data(prob);
			print_conflict_density();
			break;
		case 2:
		{
			float c=0;
			int max=crs_per_stu_n[1];
			for (int i=1;i<=stu_no;i++)
			{
				c=c+crs_per_stu_n[i];
				if (max<crs_per_stu_n[i]) max=crs_per_stu_n[i];
			}
			int stu_with_xless[max+1]{};
			for (int i=1;i<=stu_no;i++)
			{
				int a=crs_per_stu_n[i];
				stu_with_xless[a]++ ;
			}
			cout << "Mesos oros mathimatwn ana foititi:" << c/stu_no << endl;
			cout << "Megistos arithmos dilwthentvn mathimatwn apo foititi:" << max << endl;
			for (int i=1;i<=max;i++)
				cout << stu_with_xless[i] <<" Mathites me " << i << " Mathimata " << endl;

			float d=0;
			int maxf=stu_per_crs_n[1];
			for (int i=1;i<=crs_no;i++)
			{
				d=d+stu_per_crs_n[i];
				if (maxf<stu_per_crs_n[i]) maxf=stu_per_crs_n[i];
			}
			cout << "Mesos oros foititwn ana mathima:" << d/crs_no << endl;
			cout << "Megistos arithmos foititwn ana mathima:" << maxf << endl;
			int* less_stu;
			less_stu=new int[maxf+1]();
			for (int i=1;i<=crs_no;i++)
			{
				int a=stu_per_crs_n[i];
				less_stu[a]++;
			}
			for (int j=1;j<=maxf;j++)
				if (less_stu[j]!=0)	cout << less_stu[j] << " Mathimata me " << j << " Mathites " << endl;
		}

		break;
		case 3:
			int* akmes;
			akmes=new int [crs_no+1];
			{
				for (int i=1;i<=v;i++){
					int cnt=0;
					for (int j=1;j<=v;j++)
						if (conflict_matrix[i][j]==1) cnt++;
					akmes[i]=cnt;
				}
				int akmes_min=akmes[1];
				int akmes_max=akmes[1];
				float akmes_mo=0;
				for (int k=1;k<=crs_no;k++)
				{
					if (akmes[k]>akmes_max) akmes_max=akmes[k];
					if (akmes[k]<akmes_min) akmes_min=akmes[k];
					akmes_mo=akmes_mo+akmes[k];
					cout<<"H eksetasi "<< k << " sugkrouetai me "<< akmes[k]<< " eksetaseis ." << endl;
				}
				cout<<" " <<endl;
				cout<<"Oi ligoteres sugkrouseis anamesa stis eksetaseis einai : "<<akmes_min<<endl;
				cout<<"Oi perissoteres sugkrouseis anamesa stis eksetaseis einai : "<<akmes_max<<endl;
				cout<<"O m.o twn sugkrousewn einai :  "<< akmes_mo/crs_no<<endl;


				load_graph(prob);
			}


			break;
		case 4:
		{
			int kostos = 0;
			int diafora;
			load_solution(prob);
			for (int x = 1; x < stu_no; x++)
				for (int y = 0; y < crs_per_stu_n[x]; y++)
					for (int z = y + 1; z < crs_per_stu_n[x]; z++)
					{
						diafora = sol[crs_per_stu[x][y]]-sol[crs_per_stu[x][z]];
						if (abs(diafora) == 1) kostos = kostos + 16;
						else if (abs(diafora) == 2) kostos = kostos + 8;
						else if (abs(diafora) == 3) kostos = kostos + 4;
						else if (abs(diafora) == 4) kostos = kostos + 2;
						else if (abs(diafora) == 5) kostos = kostos + 1;
					}

			cout << "To kostos lusis einai : " << kostos << endl;
		}
			break;
		case 5:
			break;
		default:
			cout << "Lathos epilogi. Parakalw ksanadokimaste !" << endl;
		}
		} while (choice != 5);
	}
