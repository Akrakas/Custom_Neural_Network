#ifndef FUNCTION_H_INCLUDED
#define FUNCTION_H_INCLUDED

#include "def.h"

typedef enum TYPE_EMOTION {ANGER, DISGUST, FEAR, JOY, SADNESS, SURPRISE} EMOTION;

void pick_pattern (int* count_seq, int* count_cat, int number_of_categories, int number_of_sequences);

/*----------------------------RANDOM VALUE-----------------------------------*/
double Random_Value(double alti);
double Random_Gauss(double mean, double stddev);
double Rdm_Inter(double a,double b);
int dirExists(string path);

class Tree_node_class
{
    public :
        Tree_node_class();
        ~Tree_node_class();

		vector<Tree_node_class*> Upper_nodes;
		vector<Tree_node_class*> Lower_nodes;
		int Layer_Cell_Number;
		int layer_index;
		vector<Tree_node_class*> Callback_recurent;
		Tree_node_class* recurent_layer;
		int database_input_reference;

		void Clear_tree();
};

class test_results_class
{
	public :
		bool succes;
		double erreur[NB_OF_REINJECTIONS+1];
		int categorisation;
};

class test_results_class_V2
{
	public :
		double** database_success;
		double** database_error;
		double database_success_total;
		double database_error_total;
		int number_of_categories;
		int number_of_intensites;

		test_results_class_V2(int _number_of_categories, int _number_of_intensites);
		~test_results_class_V2();
};

class jobs_sharing_class
{
	public :
		mutex mtx;
		vector<string> job_info_database;
		vector<int> job_carac;
		vector<int> available;
		vector<double> progress;
		int* status;
		int updatedelay;
		int precision;
		int TimeStamps_index;
		vector< chrono::time_point<chrono::system_clock> > TimeStamps;
		chrono::time_point<chrono::system_clock> StartingTime;
		vector<double> ProgressStamps;
		int available_threads;

		jobs_sharing_class();
		~jobs_sharing_class();
		void create_jobs(string database_path, int carac, int _available_threads);
		int get_job();
		bool is_finished();
		void show_progress();
};

#endif // FUNCTION_H_INCLUDED
