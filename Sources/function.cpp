#include "function.h"

void pick_pattern (int* count_seq, int* count_cat, int number_of_categories, int number_of_sequences)
{
	vector<int> patterns;
	for (int f=0; f<number_of_categories*number_of_sequences; f++) patterns.push_back(f);
	random_shuffle(patterns.begin(), patterns.end());
	for (int f=0; f<number_of_categories*number_of_sequences; f++) 
	{
		count_seq[f] = patterns.at(f)%number_of_sequences; //nombre entre 0 et number_of_sequences
		count_cat[f] = (int)(patterns.at(f)/(double)number_of_sequences); //nombre entre 0 et number_of_categories
	}
}


//----------------------------RANDOM VALUE-----------------------------------
double Random_Value(double alti) //retourne une valeur entre -alti et alti
{
    double result;
	result = (2*((rand() % 3272) / 3273.0)-1)*alti;
	return(result);
	//return ((2*((rand() % 3272) / 3272.0)-1)*alti);
}

double Random_Gauss(double mean, double stddev) //retourne une valeur aleatoire d'une gaussienne
{
	std::default_random_engine generator(rand());
	std::normal_distribution<double> distribution(mean,stddev);
	return distribution(generator);
}

double Rdm_Inter(double a,double b) //retourne une valeur aleatoire entre a et b
{
    long double tmp_rl;
	tmp_rl = a + ((rand() % 32727) / 32737.0) * (b - a);
	return(tmp_rl);
	//return (a + ((rand() % 3272) / 3272.0) * (b - a));
}

int dirExists(string path)
{
	struct stat info;
	if(stat( path.c_str(), &info ) != 0) return 0;
	else if(info.st_mode & S_IFDIR) return 1;
	else return 0;
}

Tree_node_class::Tree_node_class()
{
	recurent_layer = NULL;
	Layer_Cell_Number = -1;
	layer_index = -1;
	database_input_reference = -1;
}

Tree_node_class::~Tree_node_class()
{
}

void Tree_node_class::Clear_tree()
{
	for(int i=0 ; i<Upper_nodes.size() ; i++)
	{
		Upper_nodes.at(i)->Clear_tree();
	}
	if(Upper_nodes.size() == 0) return;
	for(int i=0 ; i<Upper_nodes.size() ; i++)
	{
		delete Upper_nodes.at(i);
	}
}

jobs_sharing_class::jobs_sharing_class()
{
	updatedelay = 2000000;
	precision = 3;
	StartingTime = chrono::system_clock::now();
	status = NULL;
}

jobs_sharing_class::~jobs_sharing_class()
{
	if(status != NULL) {free(status); status = NULL;}
}
void jobs_sharing_class::create_jobs(string database_path, int carac, int _available_threads)
{
	
	available_threads = _available_threads;
	if(status == NULL) status = (int*)calloc(available_threads, sizeof(int));
	int previous_number_of_jobs = job_carac.size();
	for (int f=previous_number_of_jobs; f<previous_number_of_jobs+NB_OF_RUN; f++) 
	{
		job_info_database.push_back(database_path);
		job_carac.push_back(carac);
		available.push_back(f);
		progress.push_back(0);
	}
}

int jobs_sharing_class::get_job()
{
	if(available.size() < 1) return -1;
	
	mtx.lock(); //Verouille l'acces aux autres threads
	int job = available.back();
	available.pop_back();
	mtx.unlock(); //Deverouille l'acces aux autres threads
	return job;
}

bool jobs_sharing_class::is_finished()
{
	for(int i=0 ; i<available_threads ; i++)
	{
		if(status[i] != -1) return false; //Si un des threads n'a pas fini la fonction retourne false
	}
	return true; //Si tout est fini la fonction retourne true
}

void jobs_sharing_class::show_progress()
{
	if(ProgressStamps.size() >= precision) ProgressStamps.erase(ProgressStamps.begin());
	if(TimeStamps.size() >= precision) TimeStamps.erase(TimeStamps.begin());
	TimeStamps.push_back(chrono::system_clock::now());

	double total_progress = 0.0;
	for(int i=0 ; i<progress.size() ; i++)
	{
		total_progress += progress[i];
	}
	total_progress /= progress.size();

	cout << "PROGRESS = " << total_progress*100 << "%  ";
	ProgressStamps.push_back(total_progress);

	double progres_moyen = 0.0;
	int temps_moyen = 0.0;
	for(int i=1 ; i<ProgressStamps.size() ; i++) progres_moyen += (ProgressStamps.at(i) - ProgressStamps.at(i-1))/ProgressStamps.size();
	for(int i=1 ; i<TimeStamps.size() ; i++) temps_moyen += chrono::duration_cast<chrono::milliseconds>(TimeStamps.at(i) - TimeStamps.at(i-1)).count() /TimeStamps.size();
	int lasting_time = (1-total_progress) * ((temps_moyen/(double)1000)/progres_moyen); 
	int lasting_time_hour = (int)lasting_time/3600.0;
	int lasting_time_min = (int)(lasting_time-(lasting_time_hour*3600.0))/60.0;
	int lasting_time_sec = (int)(lasting_time-(lasting_time_hour*3600.0)-(lasting_time_min*60));
	
	int predicted_time = lasting_time + chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - StartingTime).count();
	int predicted_time_hour = (int)predicted_time/3600.0;
	int predicted_time_min = (int)(predicted_time-(predicted_time_hour*3600.0))/60.0;
	int predicted_time_sec = (int)(predicted_time-(predicted_time_hour*3600.0)-(predicted_time_min*60));

	cout << "LEFT = ";
	cout << lasting_time_hour << ":" << lasting_time_min << ":" << lasting_time_sec << "  ";
	cout << "EST. = ";
	cout << predicted_time_hour << ":" << predicted_time_min << ":" << predicted_time_sec;
	cout << "     \n" << flush;
}

test_results_class_V2::test_results_class_V2(int _number_of_categories, int _number_of_intensites)
{
	number_of_categories = _number_of_categories;
	number_of_intensites = _number_of_intensites;
	database_success = (double**)calloc(number_of_categories, sizeof(double*));
	database_error = (double**)calloc(number_of_categories, sizeof(double*));
	for(int i = 0 ; i<number_of_categories ; i++)
	{
		database_success[i] = (double*)calloc(number_of_intensites, sizeof(double));
		database_error[i] = (double*)calloc(number_of_intensites, sizeof(double));
	}
	database_success_total = 0.0;
	database_error_total = 0.0;
}

test_results_class_V2::~test_results_class_V2()
{
	for(int i = 0 ; i<number_of_categories ; i++)
	{
		free(database_success[i]);
		free(database_error[i]);
	}
	free(database_success);
	free(database_error);
}


