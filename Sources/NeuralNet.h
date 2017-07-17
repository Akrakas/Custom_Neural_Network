#ifndef NET_H
#define NET_H
#include "def.h"
#include "Cellule_class.h"
#include "Link_class.h"
#include "Sequence.h"
#include "function.h"
#include "Database_class.h"

class NeuralNet
{
	public :
		NeuralNet();
		void InitNet();
		~NeuralNet();

		Cellule_class** Cells;
		Tree_node_class* Root_node;
		Database_class Full_Database;
		Database_class Database_Learn;
		Database_class Database_Test;

		//Taux d'apprentissage
		long double Learning_rate;
		long double User_Learning_Rate;
		//Taux de regularisation
		long double Lambda;
		//Elan
		long double Momentum;

		
		//Variables d'optimisation : pour faire des calculs plus rapidement
		//Nombre de cellules par couche plus 1, pour compter la cellule de biais
		vector<int> Cells_Per_Layer;
		//Nombre de couche moins 1
		int Layer_Number;
		//Taux d'apprentissage negatif
		long double Learning_rate_neg;
		//Taux d'apprentissage*Lambda
		long double Learning_rate_Time_Lambda;
		//Nombre de poids dans le reseau
		int Total_number_of_weight;
		//Pour le chargement de base de données
		string Previous_Database_Path;
		int Previous_input_size;
		int Previous_output_size;
		
		//Vecteurs repertoriant toutes les couches du reseau
		vector<int> Input_layers_index;
		vector<int> No_Input_layers_index;
		vector<int> No_Input_No_Output_layers_index;
		vector<int> Output_layers_index;
		vector<int> NoRecursive_Input_layers_index;
		vector<int> Recursive_Input_layers_index;
		vector<int> Input_Reference;
		vector<Tree_node_class*> Recursive_Reference;

		void Reset();
		void Analyse_tree(Tree_node_class* Root_node);
		void Create_cells_from_tree(Tree_node_class* Root_node);
		void Link_Two_Cells(Cellule_class* First_Cell, Cellule_class* Second_Cell);
		void Full_Link_Two_Layers(Cellule_class* First_Layer, Cellule_class* Second_Layer);
		void Quick_Weight_Save(long double* Save_Buffer);
		void Quick_Weight_Load(long double* Load_Buffer);
		void Clear_Database();
		void Clear_Cells();
		bool load_formatted_database(string path, bool Net_Type);
		int Split_Database(int number_of_test_sequences, vector<int>* Override_selection);
		void Load_input_pattern(vector<double**>* pattern_vector, int pattern_index_in_sequence);
		void Propager();
		void Learn_Pattern(Sequence* sequence);
		void Calc_Net_Delta(Sequence* sequence, int pattern_index);
		void Adjust_Weight();
		long double Calc_Delta_Last_Layer(int cell_index, double sortie_desiree);
		long double Calc_Delta_Hidden_Layer(int couche, int cell_index, long double sortie);
		void Test_Pattern(Sequence* sequence, int sequence_index, int pattern_index, test_results_class* test_results, bool learnbase, bool confusion);
		void Test_From_Database(Database_class* From_Database, test_results_class_V2* test_results, bool confusion);
		void Test_Categorisation(Sequence* sequence, int pattern_index, test_results_class* test_results);
		void Reinjection(int emo_index, int pat_no, test_results_class* test_results, int reinjection_index);
};


#endif // NET_H_INCLUDED
