#include "def.h"
#include "Cellule_class.h"
#include "NeuralNet.h"
#include "function.h"

//Class utilisee pour repartir les taches entre les threads, et pour observer l'avancement de la simulation
//Utilisee uniquement dans le main.cpp
jobs_sharing_class job_sharing;

//Chemin vers le dossier de sortie
string Results_path_main = "../Results/Nicolas/DNN_TEST/";
//Chemin vers le fichiers contenant les taches
string Jobs_path = "../Jobs/JOBS.txt";

void RunThread(int thread);
int singleFullRun(NeuralNet* Net, string* job_info_database, int jobID, int number_of_plages, ofstream* earlystop_outfile, string* Emotions);
void concatenate_files();

int main()
{
	srand(time(NULL)); 
	//Sanity check pour la création du fichier de résultat
	if(dirExists(Results_path_main) == 0)
	{
		if(mkdir(Results_path_main.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
		{
			cout << "Erreur lors de la creation du fichier \"" << Results_path_main << "\"" << endl;
			return 0;
		}
	}
	
	//Calcul du nombre de coeurs disponible sur la machine
	cout << "Nombre de threads disponible : " << thread::hardware_concurrency() << endl;
	int available_threads = thread::hardware_concurrency();
	//int available_threads = 1;
	
	//Chargement des taches
    ifstream jobs_file (Jobs_path, ios::in);
    if (jobs_file.fail()){perror(("Can't find " + Jobs_path).c_str());return 0;}
	string Database_path;
	jobs_file >> Database_path;
	int carac = 0;
	while(!jobs_file.eof())
	{
		job_sharing.create_jobs(Database_path, carac, available_threads);
		jobs_file >> Database_path;
		carac++;
	}
	jobs_file.close();
	
	//Taches chargées, lancement des threads
	thread thread_array[available_threads];
	for(int frequence = 0; frequence<available_threads ; frequence++)
	{
		thread_array[frequence] = thread(RunThread, frequence);
	}

	//Le programme principal attend pendant que les threads font le boulot
	do
	{
		//Monitoring des threads : le programme affiche la progression générale jusqu'à la fin des taches
		job_sharing.show_progress();
		usleep(job_sharing.updatedelay); //Attente de 2 secondes
	} while(job_sharing.is_finished() == false);
	
	//Toutes les threads ont fini, on les supprime
	for(int frequence = 0; frequence<available_threads ; frequence++)
	{
		thread_array[frequence].join(); //Fin des threads
	}
	
	//Ecriture d'un timestamp
	chrono::time_point<chrono::system_clock> Timestamp = chrono::system_clock::now();
	time_t ptr_tmstmp = chrono::system_clock::to_time_t(Timestamp);
	ofstream timestamp_outfile (Results_path_main + "/Timestamp.txt", ios::out);
	timestamp_outfile << "Timestamp : " << ctime(&ptr_tmstmp) << endl;
	timestamp_outfile << "Number of runs : " << NB_OF_RUN << endl;
	timestamp_outfile << "Number of epochs : " << MAX_EPOCH << endl;
	timestamp_outfile << "Number of epochs per tests : " << NB_EPOCHS_TEST << endl;
	timestamp_outfile << "Number of threads : " << available_threads << endl;
	timestamp_outfile << "Test de confusion : " << "ISCONFUSION" << endl;
	timestamp_outfile.close();
	
	//Concatenation des résultats des threads
	concatenate_files();
	cout << endl << "Fin de la generation" << endl;
    return 0;
}

void RunThread(int thread)
{
	//Chaque thread se créer son réseau de neurone, puis pioche dans la liste des taches disponibles
	
	//Creation du fichier de sortie
	string Emotions[] {"Anger", "Disgust",  "Fear",  "Joy",  "Sadness",  "Surprise"};
    ofstream earlystop_outfile (Results_path_main + "/Raw_stop_" + to_string(thread+1) + ".txt", ios::out);
	earlystop_outfile << "RUN;ACTEUR;CARAC;PLAGE;EARLY_STOP_EPOCH;EMOTION;INTENSITE;VD_PERF;VD_CONF";
	earlystop_outfile << endl;
	
	//Creation du reseau
	NeuralNet Net;

	//Tant qu'il y a du boulot, la thread continue de travailler
	int job;
	job = job_sharing.get_job();
	while(job != -1)
	{
	    ifstream meta_plage_file (job_sharing.job_info_database[job] + "/meta_plage.txt", ios::in);
		if (meta_plage_file.fail()){perror(("Can't find " + job_sharing.job_info_database[job] + "/meta_plage.txt").c_str());job_sharing.status[thread] = -1;return;}
		int number_of_plages = 0;
		meta_plage_file.ignore(256,' '); meta_plage_file >> number_of_plages;
		meta_plage_file.close();

		//Execution de la tache recuperee
		if(singleFullRun(&Net, &(job_sharing.job_info_database[job]), job, number_of_plages, &earlystop_outfile, Emotions) == -1)
		{
			job_sharing.status[thread] = -1;
			return;
		}
		job = job_sharing.get_job();
	}
	
	//Plus de taches, on arrete tout
	earlystop_outfile.close();
	job_sharing.status[thread] = -1;
}

int singleFullRun(NeuralNet* Net, string* job_info_database, int jobID, int number_of_plages, ofstream* earlystop_outfile, string* Emotions)
{
	Net->Reset(); //Reset des poids du reseau (au cas ou)
	//vector<int> Forced_choice = {rand()%NB_OF_FACES}; //Choix de la sequence de test
	vector<int> Forced_choice = {jobID%NB_OF_FACES};

	long double* early_stop_saved_net = NULL; //Reseau sauvegardé pour l'earlystop

	for(int plage_index=0 ; plage_index<number_of_plages ; plage_index++)
	{
		if(!Net->load_formatted_database(*(job_info_database)+"/plage"+ to_string(plage_index+1) +"/", false)) {return -1;} //Chargement de la base de donnee
		Net->Split_Database(Forced_choice.size(), &Forced_choice); //Separation de la base de test
		int count_seq[Net->Database_Learn.number_of_sequences*Net->Database_Learn.number_of_categories];
		int count_cat[Net->Database_Learn.number_of_sequences*Net->Database_Learn.number_of_categories];
		pick_pattern(count_seq, count_cat, Net->Database_Learn.number_of_categories, Net->Database_Learn.number_of_sequences);
		if(plage_index > 0) Net->Quick_Weight_Load(early_stop_saved_net);
		else early_stop_saved_net = (long double*)calloc(Net->Total_number_of_weight, sizeof(long double));
		int early_stop_epoch = 0;
		double early_stop_perf;
		if(ISCONFUSION) early_stop_perf = 10000000;
		else early_stop_perf = 0;
		
		//L'entrainement commence ici
		//On fait MAX_EPOCH epoch d'entrainement, avec un test tout les NB_EPOCHS_TEST epochs (
		for(int epoch=0 ; epoch<MAX_EPOCH; epoch++)
		{
			for(int cur_sequence=0 ; cur_sequence<Net->Database_Learn.number_of_sequences*Net->Database_Learn.number_of_categories ; cur_sequence++)
			{ //APPRENTISSAGE
				//On apprend toute la base d'apprentissage
				Net->Learn_Pattern(&(Net->Database_Learn.Sequences[count_cat[cur_sequence]][count_seq[cur_sequence]]));
			}
			if((epoch+1)%NB_EPOCHS_TEST == 0) 
			{ //TESTS
				//On test sur toute la base de test
				test_results_class_V2 stats_test(Net->Database_Test.number_of_categories, NB_OF_INTENSITES);
				Net->Test_From_Database(&(Net->Database_Test), &stats_test, ISCONFUSION);
				if(ISCONFUSION && early_stop_perf>stats_test.database_error_total) 
				{
					early_stop_perf = stats_test.database_error_total;
					early_stop_epoch = epoch+1;
					Net->Quick_Weight_Save(early_stop_saved_net);
				} else if(!ISCONFUSION && early_stop_perf<stats_test.database_success_total) {
					early_stop_perf = stats_test.database_success_total;
					early_stop_epoch = epoch+1;
					Net->Quick_Weight_Save(early_stop_saved_net);
				}
			}
			
			job_sharing.progress[jobID] = ((plage_index*MAX_EPOCH) + epoch)/(double)(number_of_plages*MAX_EPOCH);
		}
		
		//L'entrainement est maintenant terminé, et l'epoch la plus performante à été sauvegardée dans early_stop_saved_net
		//On mesure les résultats de early_stop_saved_net
		long double temp_net[Net->Total_number_of_weight];
		Net->Quick_Weight_Save(temp_net);
		Net->Quick_Weight_Load(early_stop_saved_net);
		test_results_class_V2 stats_test_early_stop(Net->Database_Test.number_of_categories, NB_OF_INTENSITES);
		Net->Test_From_Database(&(Net->Database_Test), &stats_test_early_stop, ISCONFUSION);
		for(int category_index = 0 ; category_index < Net->Database_Test.number_of_categories ; category_index++)
	    {
			for(int i=1 ; i<NB_OF_INTENSITES ; i++) 
			{
				*(earlystop_outfile) << jobID << ";" << Forced_choice.at(0) << ";" << job_sharing.job_carac[jobID] << ";" << plage_index << ";" << early_stop_epoch << ";" << Emotions[category_index] << ";" << i-1;
				//*(earlystop_outfile) << ";" << stats_test_early_stop.database_error_total;
				//*(earlystop_outfile) << ";" << stats_test_early_stop.database_success_total;
				*(earlystop_outfile) << ";" << stats_test_early_stop.database_error[category_index][i];
				*(earlystop_outfile) << ";" << stats_test_early_stop.database_success[category_index][i];
				*(earlystop_outfile) << endl;
			}
		}
		Net->Quick_Weight_Load(temp_net);
	}
	if(early_stop_saved_net != NULL) {free(early_stop_saved_net); early_stop_saved_net = NULL;}
	return 0;
}

////////////////////////////////////
////////////////////////////////////
////PAS COMPATIBLE AVEC WINDOWS/////
////////////////////////////////////
////////////////////////////////////
//ET TRES DANGEREUX PAR LA MEME OCCASION CAR UTILISATIONI DE LA FONCTION system()
//CONCATENE TOUT LES FICHIERS DE RESULTATS CREES PAR LES THREADS EN UN SEUL GROS FICHIER
void concatenate_files()
{
	string command = "{ head -n1 " + Results_path_main + "Raw_stop_1.txt; for f in " + Results_path_main + "Raw_stop_*.txt; do tail -n+2 \"$f\"; done; } > " + Results_path_main + "Raw_stop.txt";
	system(command.c_str());
}

	/*chrono::time_point<chrono::system_clock> Start, Stop;
	Start = chrono::system_clock::now();
	for(int i=0 ; i<1000000 ; i++)
	{	
		Net.Calc_Net_Gradient(1,1);
	}
	Stop = chrono::system_clock::now();
	cout << chrono::duration_cast<chrono::milliseconds>(Stop - Start).count() << endl;*/
