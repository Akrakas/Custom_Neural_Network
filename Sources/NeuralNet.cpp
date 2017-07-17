#include "NeuralNet.h"

//Les boucles de parcours du réseau sont des listes chainées fonctionnant avec des do...while, car c'est plus rapide que les boucles for


NeuralNet::NeuralNet()
{
	Cells = NULL;
	Root_node = NULL;
	Previous_input_size = 0;
	Previous_output_size = 0;
}

void NeuralNet::InitNet()
{
	Clear_Cells();
	Cells_Per_Layer.clear();

	//CHOIX DE L'ARCHITECTURE DU RESEAU (TBD : faire une fonction...)
	//MLP
	/*Tree_node_class* Output_Node = new Tree_node_class();
	Output_Node->Layer_Cell_Number = Full_Database.output_size;
	Tree_node_class* Hidden_Node_1 = new Tree_node_class();
	Hidden_Node_1->Layer_Cell_Number = 16;
	Tree_node_class* Input_Node_1 = new Tree_node_class();
	Input_Node_1->Layer_Cell_Number = Full_Database.input_size.at(0);
	Input_Node_1->database_input_reference = 0;

	Input_Node_1->Lower_nodes.push_back(Hidden_Node_1);
	Hidden_Node_1->Upper_nodes.push_back(Input_Node_1);
	Hidden_Node_1->Lower_nodes.push_back(Output_Node);
	Output_Node->Upper_nodes.push_back(Hidden_Node_1);*/

	//SRN
	/*Tree_node_class* Output_Node = new Tree_node_class();
	Output_Node->Layer_Cell_Number = Full_Database.output_size;
	Tree_node_class* Hidden_Node_1 = new Tree_node_class();
	Hidden_Node_1->Layer_Cell_Number = 16;
	Tree_node_class* Input_Node_1 = new Tree_node_class();
	Input_Node_1->Layer_Cell_Number = Full_Database.input_size.at(0);
	Input_Node_1->database_input_reference = 0;
	Tree_node_class* SRN_Node_1 = new Tree_node_class();
	SRN_Node_1->Layer_Cell_Number = 16;
	SRN_Node_1->recurent_layer = Hidden_Node_1;

	Input_Node_1->Lower_nodes.push_back(Hidden_Node_1);
	SRN_Node_1->Lower_nodes.push_back(Hidden_Node_1);
	Hidden_Node_1->Upper_nodes.push_back(Input_Node_1);
	Hidden_Node_1->Upper_nodes.push_back(SRN_Node_1);
	Hidden_Node_1->Lower_nodes.push_back(Output_Node);
	Output_Node->Upper_nodes.push_back(Hidden_Node_1);*/

	//BAR
	Tree_node_class* Output_Node = new Tree_node_class();
	Output_Node->Layer_Cell_Number = Full_Database.output_size;
	Tree_node_class* Hidden_Node_1 = new Tree_node_class();
	Hidden_Node_1->Layer_Cell_Number = 112;
	Tree_node_class* Input_Node_1 = new Tree_node_class();
	Input_Node_1->Layer_Cell_Number = Full_Database.input_size.at(0);
	Input_Node_1->database_input_reference = 0;
	Tree_node_class* Hidden_Node_2 = new Tree_node_class();
	Hidden_Node_2->Layer_Cell_Number = 16;
	Tree_node_class* Input_Node_2 = new Tree_node_class();
	Input_Node_2->Layer_Cell_Number = Full_Database.input_size.at(1);
	Input_Node_2->database_input_reference = 1;
	Tree_node_class* SRN_Node_1 = new Tree_node_class();
	SRN_Node_1->Layer_Cell_Number = 16;
	SRN_Node_1->recurent_layer = Hidden_Node_2;

	Input_Node_1->Lower_nodes.push_back(Hidden_Node_1);
	Input_Node_2->Lower_nodes.push_back(Hidden_Node_2);
	SRN_Node_1->Lower_nodes.push_back(Hidden_Node_2);
	Hidden_Node_2->Upper_nodes.push_back(Input_Node_2);
	Hidden_Node_2->Upper_nodes.push_back(SRN_Node_1);
	Hidden_Node_2->Lower_nodes.push_back(Hidden_Node_1);
	Hidden_Node_1->Upper_nodes.push_back(Input_Node_1);
	Hidden_Node_1->Upper_nodes.push_back(Hidden_Node_2);
	Hidden_Node_1->Lower_nodes.push_back(Output_Node);
	Output_Node->Upper_nodes.push_back(Hidden_Node_1);
	Root_node = Output_Node;

	Layer_Number = 0;
	Total_number_of_weight = 0;
	Analyse_tree(Root_node);

	Cells = (Cellule_class**)calloc(Layer_Number, sizeof(Cellule_class*));
	Create_cells_from_tree(Root_node);
	User_Learning_Rate = 0.01;
	Learning_rate = User_Learning_Rate;
	//Plus lambda est bas, moins la regularisation a d'effet
	//Vu que les poids sont mis a jour litteralement tout le temps, il vaut mieux que lambda soit tres tres bas
	//A changer si on utilise des mini-batchs
	Lambda = 0.0000001;
	Momentum = 0.9;
}

//Parcours l'arbre du réseau et remplis les variables comme le nombre de couches, etc.
void NeuralNet::Analyse_tree(Tree_node_class* Root_node)
{
	for(int i=0 ; i<Root_node->Upper_nodes.size() ; i++)
	{
		Analyse_tree(Root_node->Upper_nodes.at(i));
	}
	if(Root_node->Upper_nodes.size() == 0) {
		Input_layers_index.push_back(Layer_Number);
		if(Root_node->recurent_layer != NULL) 
		{
			Recursive_Input_layers_index.push_back(Layer_Number);
			Recursive_Reference.push_back(Root_node->recurent_layer);
		} else {
			NoRecursive_Input_layers_index.push_back(Layer_Number);
			Input_Reference.push_back(Root_node->database_input_reference);
		}
	}
	if(Root_node->Lower_nodes.size() == 0) Output_layers_index.push_back(Layer_Number);
	if(Root_node->Upper_nodes.size() != 0) No_Input_layers_index.push_back(Layer_Number);
	if(Root_node->Upper_nodes.size() != 0 && Root_node->Lower_nodes.size() != 0) No_Input_No_Output_layers_index.push_back(Layer_Number);
	Root_node->layer_index = Layer_Number;
	Cells_Per_Layer.push_back(Root_node->Layer_Cell_Number);
	Layer_Number++;
}

//Parcours l'arbre et créer les neurones necessaires
void NeuralNet::Create_cells_from_tree(Tree_node_class* Root_node)
{
	for(int i=0 ; i<Root_node->Upper_nodes.size() ; i++)
	{
		Create_cells_from_tree(Root_node->Upper_nodes.at(i));
	}
	//On rajoute une cellule par couche : la cellule de biais
	Cellule_class* temps_cell_array = new Cellule_class[Root_node->Layer_Cell_Number+1];
	for(int j=0 ; j<Root_node->Layer_Cell_Number+1 ; j++)
	{
		//Initialisation de variables d'optimisation
		temps_cell_array[j].index = j;
		temps_cell_array[j].number_of_connexions = 0;
		temps_cell_array[j].number_of_cells_in_layer = Root_node->Layer_Cell_Number+1;
		if(j<Root_node->Layer_Cell_Number) 
		{
			temps_cell_array[j].Next_Cell_Chain_WithBiais = &(temps_cell_array[j+1]);
			if(j<Root_node->Layer_Cell_Number-1) temps_cell_array[j].Next_Cell_Chain_NoBiais = &(temps_cell_array[j+1]);
			//Connexion a la cellule de biais
			if(Root_node->Upper_nodes.size() > 0) Link_Two_Cells(&(temps_cell_array[Root_node->Layer_Cell_Number]), &(temps_cell_array[j]));
		} else {
			//La derniere cellule est la cellule de biais
			temps_cell_array[j].Sortie = -1.0;
		}
	}
	Cells[Root_node->layer_index] = temps_cell_array;
	for(int i=0 ; i<Root_node->Upper_nodes.size() ; i++)
	{
		Full_Link_Two_Layers(Cells[Root_node->Upper_nodes.at(i)->layer_index], Cells[Root_node->layer_index]);
	}
}

//Connecte deux neurones
void NeuralNet::Link_Two_Cells(Cellule_class* First_Cell, Cellule_class* Second_Cell)
{
	Link_class* temps_new_link = new Link_class();
	temps_new_link->To_cell = Second_Cell;
	temps_new_link->From_cell = First_Cell;
	Link_class* Second_Cell_link_pre = Second_Cell->Pre_Link_Chain;
	if(Second_Cell_link_pre == NULL) Second_Cell->Pre_Link_Chain = temps_new_link;
	else {
		while(Second_Cell_link_pre->Next_Pre_Link_Chain != NULL) {
			Second_Cell_link_pre = Second_Cell_link_pre->Next_Pre_Link_Chain;
		}
		Second_Cell_link_pre->Next_Pre_Link_Chain = temps_new_link;
	}
	Link_class* First_Cell_link_post = First_Cell->Post_Link_Chain;
	if(First_Cell_link_post == NULL) First_Cell->Post_Link_Chain = temps_new_link;
	else {
		while(First_Cell_link_post->Next_Post_Link_Chain != NULL) {
			First_Cell_link_post = First_Cell_link_post->Next_Post_Link_Chain;
		}
		First_Cell_link_post->Next_Post_Link_Chain = temps_new_link;
	}
	Second_Cell->number_of_connexions++;
	Total_number_of_weight++;
}

//Connecte deux couches de neurones
void NeuralNet::Full_Link_Two_Layers(Cellule_class* First_Layer, Cellule_class* Second_Layer)
{
	Cellule_class* Temp_Second_Layer_Cell = Second_Layer;
	do
	{
		//Connexions aux cellules de la couche precedente
		Cellule_class* Temp_First_Layer_Cell = First_Layer;
		do {
			Link_Two_Cells(Temp_First_Layer_Cell, Temp_Second_Layer_Cell);
			Temp_First_Layer_Cell = Temp_First_Layer_Cell->Next_Cell_Chain_NoBiais;
		} while(Temp_First_Layer_Cell != NULL);

		Link_class* Temp_pre_link = Temp_Second_Layer_Cell->Pre_Link_Chain;
		do {
			//Initialisation des poids qui est sensee empecher le bloquage des poids
			Temp_pre_link->Weight = Random_Gauss(0.0, 1.0/sqrt(Temp_Second_Layer_Cell->number_of_connexions));
			Temp_pre_link->Weight_Variation = 0.0;
			Temp_pre_link = Temp_pre_link->Next_Pre_Link_Chain;
		} while (Temp_pre_link != NULL);
		Temp_Second_Layer_Cell = Temp_Second_Layer_Cell->Next_Cell_Chain_NoBiais;
	}while(Temp_Second_Layer_Cell != NULL);
}

//Destructeur
NeuralNet::~NeuralNet()
{
	Clear_Database();
	Clear_Cells();
}

//Nettoyage des neurones
void NeuralNet::Clear_Cells()
{
	if(Cells != NULL) {
		for(int i=0 ; i<Layer_Number ; i++)
		{
			Cellule_class* linked_chain_cell = Cells[i];
			if(i>0)
			{
				while(linked_chain_cell->Pre_Link_Chain != NULL)
				{
					Link_class* temps_cur_link = linked_chain_cell->Pre_Link_Chain;
					Link_class* temps_next_link = NULL;
					do {
						temps_next_link = temps_cur_link->Next_Pre_Link_Chain;
						delete temps_cur_link;
						temps_cur_link = temps_next_link;
					} while(temps_cur_link != NULL);
					linked_chain_cell = linked_chain_cell->Next_Cell_Chain_WithBiais;
				}
			}
		    delete[] Cells[i];
		}
		free(Cells);
		Cells = NULL;
	}
	if(Root_node != NULL) {
		Root_node->Clear_tree();
		delete Root_node;
		Root_node = NULL;
	}
}

//Nettoyage des bases de données chargées
void NeuralNet::Clear_Database()
{
	Full_Database.Clear_Database();
	Database_Learn.Clear_Database();
	Database_Test.Clear_Database();
}

//Chargement dune base de donnée (avec des workaround pour accelerer le chargement si celle deja chargée est la meme que celle mise en argument
//Voir programme Matlab pour savoir comment les fichiers sont organisés
bool NeuralNet::load_formatted_database(string path, bool Net_Type)
{
	if(path != Full_Database.Previous_Database_Path)
	{
		Full_Database.Clear_Database();
		Full_Database.Previous_Database_Path = path;
		Full_Database.input_size.clear();
		Full_Database.output_size = 0;
		Full_Database.number_of_categories = 0;
		Full_Database.number_of_patterns = 0;
		Full_Database.number_of_sequences = NB_OF_FACES;
		string filename_meta_data = path + "/META/meta_data.txt";
		ifstream meta_data_file (filename_meta_data, ios::in);
		if (meta_data_file.fail()){perror(("Can't find " + filename_meta_data).c_str());return 0;}

		meta_data_file.ignore(256,' '); meta_data_file >> Full_Database.number_of_inputs;
		Database_Learn.number_of_inputs = Full_Database.number_of_inputs;
		Database_Test.number_of_inputs = Full_Database.number_of_inputs;		
		
		Full_Database.input_size.resize(Full_Database.number_of_inputs);
		for(int i=0 ; i<Full_Database.number_of_inputs ; i++) {
			meta_data_file.ignore(256,' '); meta_data_file >> Full_Database.input_size.at(i);
			Database_Learn.input_size.push_back(Full_Database.input_size.back());
			Database_Test.input_size.push_back(Full_Database.input_size.back());
		}
		meta_data_file.ignore(256,' '); meta_data_file >> Full_Database.output_size;
		Database_Learn.output_size = Full_Database.output_size;
		Database_Test.output_size = Full_Database.output_size;
		meta_data_file.ignore(256,' '); meta_data_file >> Full_Database.number_of_categories;
		Database_Learn.number_of_categories = Full_Database.number_of_categories;
		Database_Test.number_of_categories = Full_Database.number_of_categories;
		meta_data_file.ignore(256,' '); meta_data_file >> Full_Database.number_of_patterns;
		Database_Learn.number_of_patterns = Full_Database.number_of_patterns;
		Database_Test.number_of_patterns = Full_Database.number_of_patterns;
		meta_data_file.close();

		Full_Database.Sequences = (Sequence**)calloc(Full_Database.number_of_categories, sizeof(Sequence*));

		string filename_path_in = path + "/META/path_in.txt";
		ifstream path_in_file (filename_path_in, ios::in);
		if (path_in_file.fail()){perror(("Can't find " + filename_path_in).c_str());return 0;}
		string filename_path_out = path + "/META/path_out.txt";
		ifstream path_out_file (filename_path_out, ios::in);
		if (path_out_file.fail()){perror(("Can't find " + filename_path_out).c_str());return 0;}
		for(int category_index = 0 ; category_index < Full_Database.number_of_categories ; category_index++)
		{
			//Full_Database.Sequences[category_index] = new Sequence[10];
			Full_Database.Sequences[category_index] = (Sequence*)calloc(10,sizeof(Sequence));
			for(int sequence_index = 0 ; sequence_index < 10 ; sequence_index++)
			{
				string filepath_in; 
				int category; 
				int number_of_patterns; 
				int input_index; 
				for(int i=0 ; i<Full_Database.number_of_inputs ; i++)
				{
					path_in_file >> filepath_in;
					path_in_file >> category;
					path_in_file >> number_of_patterns;
					path_in_file >> input_index;
					Full_Database.Sequences[category_index][sequence_index].Get_in_sequence_from_file(path + filepath_in, category, number_of_patterns, Full_Database.input_size.at(input_index));
				}
				
				string filepath_out; 
				path_out_file >> filepath_out;
				path_out_file >> category;
				path_out_file >> number_of_patterns;
				Full_Database.Sequences[category_index][sequence_index].Get_out_sequence_from_file(path + filepath_out, category, number_of_patterns, Full_Database.output_size);

			}
		}
		path_in_file.close();
		path_out_file.close();
		if(Previous_input_size!=Full_Database.input_size.at(0) || Previous_output_size!=Full_Database.output_size) InitNet();
		Previous_input_size = Full_Database.input_size.at(0);
		Previous_output_size = Full_Database.output_size;
	}
	return 1;
}

//Sépare la base de donnée en base d'apprentissage et de test
int NeuralNet::Split_Database(int _number_of_test_sequences, vector<int>* Override_selection)
{

	Database_Learn.Clear_Database();
	Database_Test.Clear_Database();
	Database_Test.number_of_sequences = _number_of_test_sequences;
	if (Override_selection != NULL) 
	{
		bool SANITY_CHECK = true;
		for (int i=0; i<Override_selection->size(); i++)
		{
			for (int j=i+1; j<Override_selection->size(); j++)
			{
				if(Override_selection->at(i) == Override_selection->at(j)) SANITY_CHECK = false;
			}
		}
		if(!SANITY_CHECK) {perror(("Corrupted split database parameters : using default\n"));Override_selection = NULL;}
		else {Database_Test.number_of_sequences = Override_selection->size();}
	}
	vector<int> learn_pattern_index;
	vector<int> test_patterns_index;
	Database_Learn.number_of_sequences = NB_OF_FACES - Database_Test.number_of_sequences;
	if (Override_selection == NULL) 
	{
		for (int i=0; i<NB_OF_FACES; i++) learn_pattern_index.push_back(i);
		for (int i=0; i<Database_Test.number_of_sequences; i++) 
		{
			int chosen_pattern = rand()%learn_pattern_index.size();
			test_patterns_index.push_back(learn_pattern_index.at(chosen_pattern));
			learn_pattern_index.erase(learn_pattern_index.begin()+chosen_pattern);
		}
	} else {
		for (int i=0; i<Override_selection->size(); i++) test_patterns_index.push_back(Override_selection->at(i));
		for (int i=0; i<NB_OF_FACES; i++) 
		{
			bool is_present = false;
			for (int j=0; j<Override_selection->size(); j++) if(Override_selection->at(j) == i) is_present = true;
			if(is_present == false )learn_pattern_index.push_back(i);
		}
	}
	Database_Learn.Sequences = (Sequence**)calloc(Full_Database.number_of_categories, sizeof(Sequence*));
	Database_Test.Sequences = (Sequence**)calloc(Full_Database.number_of_categories, sizeof(Sequence*));
	for(int category_index = 0 ; category_index < Full_Database.number_of_categories ; category_index++)
	{
		Database_Learn.Sequences[category_index] = (Sequence*)calloc(NB_OF_FACES-Database_Test.number_of_sequences,sizeof(Sequence));
		Database_Test.Sequences[category_index] = (Sequence*)calloc(Database_Test.number_of_sequences,sizeof(Sequence));
		
		for(int learn_index = 0 ; learn_index < learn_pattern_index.size() ; learn_index++)
		{
			Database_Learn.Sequences[category_index][learn_index] = Full_Database.Sequences[category_index][learn_pattern_index.at(learn_index)];
		}
		for(int test_index = 0 ; test_index < test_patterns_index.size() ; test_index++)
		{
			Database_Test.Sequences[category_index][test_index] = Full_Database.Sequences[category_index][test_patterns_index.at(test_index)];
		}
		
	}
}

//Initialise le réseau
void NeuralNet::Reset()
{
	
	if(Cells!=NULL) {
		for(int i=0 ; i<No_Input_layers_index.size() ; i++)
		{
			Cellule_class* Temp_link_cur_layer = Cells[No_Input_layers_index.at(i)];
			do
			{
				Link_class* temps_cur_link = Temp_link_cur_layer->Pre_Link_Chain;
				do {
					temps_cur_link->Weight = Random_Gauss(0.0, 1.0/sqrt(Temp_link_cur_layer->number_of_connexions));
					temps_cur_link->Weight_Variation = 0.0;
					temps_cur_link = temps_cur_link->Next_Pre_Link_Chain;
				} while(temps_cur_link != NULL);
				Temp_link_cur_layer = Temp_link_cur_layer->Next_Cell_Chain_NoBiais;
			}while(Temp_link_cur_layer != NULL);
		}
	}
}

//Sauvegarde rapide des poids et des biais
void NeuralNet::Quick_Weight_Save(long double* Save_Buffer)
{
	int Buffer_Index = 0;
	for(int i=0 ; i<No_Input_layers_index.size() ; i++)
	{
		Cellule_class* Temp_link_cur_layer = Cells[No_Input_layers_index.at(i)];
		do
		{
			Link_class* temps_cur_link = Temp_link_cur_layer->Pre_Link_Chain;
			do {
				Save_Buffer[Buffer_Index] = temps_cur_link->Weight;
				Buffer_Index++;
				temps_cur_link = temps_cur_link->Next_Pre_Link_Chain;
			} while(temps_cur_link != NULL);
			Temp_link_cur_layer = Temp_link_cur_layer->Next_Cell_Chain_NoBiais;
		}while(Temp_link_cur_layer != NULL);
	}
}

//Chargement rapide des poids et des biais
void NeuralNet::Quick_Weight_Load(long double* Load_Buffer)
{
	int Buffer_Index = 0;
	for(int i=0 ; i<No_Input_layers_index.size() ; i++)
	{
		Cellule_class* Temp_link_cur_layer = Cells[No_Input_layers_index.at(i)];
		do
		{
			Link_class* temps_cur_link = Temp_link_cur_layer->Pre_Link_Chain;
			do {
				temps_cur_link->Weight = Load_Buffer[Buffer_Index];
				Buffer_Index++;
				temps_cur_link = temps_cur_link->Next_Pre_Link_Chain;
			} while(temps_cur_link != NULL);
			Temp_link_cur_layer = Temp_link_cur_layer->Next_Cell_Chain_NoBiais;
		}while(Temp_link_cur_layer != NULL);
	}
}

//Chargement en entrée du réseau d'un pattern en provenance de la base de données
void NeuralNet::Load_input_pattern(vector<double**>* pattern_vector, int pattern_index_in_sequence)
{
	double* input;
	for(int i=0 ; i<NoRecursive_Input_layers_index.size() ; i++)
	{
		input = pattern_vector->at(Input_Reference.at(i))[pattern_index_in_sequence];
		Cellule_class* Temp_link_input_layer = Cells[NoRecursive_Input_layers_index.at(i)];
		do
		{
			Temp_link_input_layer->Sortie = input[Temp_link_input_layer->index];
			Temp_link_input_layer = Temp_link_input_layer->Next_Cell_Chain_NoBiais;
		}while(Temp_link_input_layer != NULL);
	}
	for(int i=0 ; i<Recursive_Input_layers_index.size() ; i++)
	{
		Cellule_class* Temp_link_input_layer = Cells[Recursive_Input_layers_index.at(i)];
		do
		{
			if(pattern_index_in_sequence == 0) Temp_link_input_layer->Sortie = 0.0;
			else Temp_link_input_layer->Sortie = Cells[Recursive_Reference.at(i)->layer_index][Temp_link_input_layer->index].Sortie;
			Temp_link_input_layer = Temp_link_input_layer->Next_Cell_Chain_NoBiais;
		}while(Temp_link_input_layer != NULL);
	}
}

//Propage les couches entrées du réseau jusqu'a la sortie
void NeuralNet::Propager()
{
	for(int i=0 ; i<No_Input_layers_index.size() ; i++)
	{
		Cellule_class* Temp_link_cur_layer = Cells[No_Input_layers_index.at(i)];
		do
		{
			long double somme_pond = 0.0;
			Link_class* temps_cur_link = Temp_link_cur_layer->Pre_Link_Chain;
			do {
				somme_pond += temps_cur_link->From_cell->Sortie*(temps_cur_link->Weight);
				temps_cur_link = temps_cur_link->Next_Pre_Link_Chain;
			} while(temps_cur_link != NULL);
			Temp_link_cur_layer->Sortie = (1.0/(1.0 + exp(-(Temp_link_cur_layer->Alpha*somme_pond))));

			Temp_link_cur_layer = Temp_link_cur_layer->Next_Cell_Chain_NoBiais;
		}while(Temp_link_cur_layer != NULL);
	}
}

//Apprentissage sur une partie de la base d'apprentissage
void NeuralNet::Learn_Pattern(Sequence* sequence)
{
	//Variables d'optimisation
	Learning_rate_neg = -Learning_rate;
	Learning_rate_Time_Lambda = Learning_rate_neg*Lambda;
	
	for(int pattern_index = 0 ; pattern_index < sequence->number_of_patterns ; pattern_index++)
	{
		Load_input_pattern(&(sequence->patterns_IN_vec), pattern_index);
		//Gradient stochastique extreme : un seul exemple par changement de poid
		Propager();
		if(pattern_index >= 1) //ELIMINATION DE CERTAINS PATTERNS (necessaire pour les tests de Roman)
		{
			//Retropropagation
			Calc_Net_Delta(sequence, pattern_index);
			//Ajustement des poids
			Adjust_Weight();
		}
	}
}

//Calcul du Delta
void NeuralNet::Calc_Net_Delta(Sequence* sequence, int pattern_index)
{
	for(int i=0 ; i<Output_layers_index.size() ; i++)
	{
		Cellule_class* Temp_link_last_layer = Cells[Output_layers_index.at(i)];
		do
		{
			//a=sortie y=sortie desiree
			
			//QUADRATIQUE = Delta = (a-y)*a*(1-a)
			//Temp_link_last_layer->Delta = (Temp_link_last_layer->Sortie - sequence->patterns_OUT[pattern_index][Temp_link_last_layer->index])*Temp_link_last_layer->Sortie*(1-Temp_link_last_layer->Sortie);
			
			//CROSS-ENTROPY = Delta = (a-y)
			Temp_link_last_layer->Delta = (Temp_link_last_layer->Sortie-sequence->patterns_OUT[pattern_index][Temp_link_last_layer->index]);
			
			Temp_link_last_layer = Temp_link_last_layer->Next_Cell_Chain_NoBiais;
		}while(Temp_link_last_layer != NULL);
	}

	long double somme_erreur = 0.0;
	for(int i=No_Input_No_Output_layers_index.size()-1 ; i>=0 ; --i)
	{
		Cellule_class* Temp_link_hid_layer = Cells[No_Input_No_Output_layers_index.at(i)];
		do
		{
			somme_erreur = 0.0;
			Link_class* temps_post_link = Temp_link_hid_layer->Post_Link_Chain;
			do
			{
				somme_erreur += (temps_post_link->Weight * temps_post_link->To_cell->Delta);
				temps_post_link = temps_post_link->Next_Post_Link_Chain;
			}while(temps_post_link != NULL);
			Temp_link_hid_layer->Delta = somme_erreur*Temp_link_hid_layer->Sortie*(1-Temp_link_hid_layer->Sortie);
			Temp_link_hid_layer = Temp_link_hid_layer->Next_Cell_Chain_NoBiais;
		}while(Temp_link_hid_layer != NULL);
	}
}

//Ajustement des poids : += -1*Learning_rate*sortie*Delta
void NeuralNet::Adjust_Weight()
{
	for(int i=0 ; i<No_Input_layers_index.size() ; i++)
	{
		Cellule_class* Temp_link_cur_layer = Cells[No_Input_layers_index.at(i)];
		do
		{
			Link_class* temps_cur_link = Temp_link_cur_layer->Pre_Link_Chain;
			do {
				//Sans regularisation
				//w += -Learning_rate*sortie*Delta + momentum*VariationPrecedente
				temps_cur_link->Weight_Variation = Learning_rate_neg*temps_cur_link->From_cell->Sortie*Temp_link_cur_layer->Delta + Momentum*temps_cur_link->Weight_Variation;
				temps_cur_link->Weight += temps_cur_link->Weight_Variation;
				temps_cur_link = temps_cur_link->Next_Pre_Link_Chain;
			} while(temps_cur_link != NULL);
			Temp_link_cur_layer = Temp_link_cur_layer->Next_Cell_Chain_NoBiais;
		}while(Temp_link_cur_layer != NULL);
	}
}

//Calcule les performances sur la base donnée en parametre
void NeuralNet::Test_From_Database(Database_class* From_Database, test_results_class_V2* test_results, bool confusion)
{
	for(int category_index = 0 ; category_index < From_Database->number_of_categories ; category_index++)
	{
		for(int sequence_index = 0 ; sequence_index < From_Database->number_of_sequences ; sequence_index++)
		{
			Sequence* temp_sequence = &(From_Database->Sequences[category_index][sequence_index]);
			for(int pattern_index = 0 ; pattern_index < temp_sequence->number_of_patterns ; pattern_index++)
			{
				Load_input_pattern(&(temp_sequence->patterns_IN_vec), pattern_index);
				Propager();
				double erreur_emotion[From_Database->number_of_categories] = {0.0};
				int index_erreur_min = 0;
				int index_max_sortie = 0;
				double max_sortie = 0;
				if(confusion) { //TEST DE CONFUSION

					for(int i=0 ; i<From_Database->number_of_categories ; i++)
					{
						Cellule_class* Temp_link_last_layer = Cells[Output_layers_index.at(0)];
						do
						{
							erreur_emotion[i] += pow(Temp_link_last_layer->Sortie - From_Database->Sequences[i][sequence_index].patterns_OUT[pattern_index][Temp_link_last_layer->index],2)/(double)Temp_link_last_layer->number_of_cells_in_layer;
							Temp_link_last_layer = Temp_link_last_layer->Next_Cell_Chain_NoBiais;
						}while(Temp_link_last_layer != NULL);
						erreur_emotion[i] = sqrt(erreur_emotion[i]);
						if(erreur_emotion[i] < erreur_emotion[index_erreur_min]) index_erreur_min=i;
					}
					test_results->database_success[category_index][pattern_index] += (index_erreur_min == temp_sequence->categorie)/(double)From_Database->number_of_sequences;
					test_results->database_success_total += (test_results->database_success[category_index][pattern_index])/(double)(temp_sequence->number_of_patterns*From_Database->number_of_categories);
				} else { //TEST DE CATEGORISATION
					Cellule_class* Temp_link_last_layer = Cells[Output_layers_index.at(0)];
					do
					{
						erreur_emotion[temp_sequence->categorie] += pow(Temp_link_last_layer->Sortie - temp_sequence->patterns_OUT[pattern_index][Temp_link_last_layer->index],2)/(double)Temp_link_last_layer->number_of_cells_in_layer;
						if(max_sortie < Temp_link_last_layer->Sortie)
						{
							max_sortie = Temp_link_last_layer->Sortie;
							index_max_sortie = Temp_link_last_layer->index;
						}
						Temp_link_last_layer = Temp_link_last_layer->Next_Cell_Chain_NoBiais;
					}while(Temp_link_last_layer != NULL);
					erreur_emotion[temp_sequence->categorie] = sqrt(erreur_emotion[temp_sequence->categorie]);
					test_results->database_success[category_index][pattern_index] = (index_max_sortie == temp_sequence->categorie)/(double)From_Database->number_of_sequences;
					test_results->database_success_total += test_results->database_success[category_index][pattern_index]/(double)(temp_sequence->number_of_patterns*From_Database->number_of_categories);
				}
				test_results->database_error[category_index][pattern_index] += (erreur_emotion[temp_sequence->categorie])/(double)From_Database->number_of_sequences;
				test_results->database_error_total += test_results->database_error[category_index][pattern_index]/(double)(temp_sequence->number_of_patterns*From_Database->number_of_categories);
			}
		}
	}

}
















/*//Calcul du delta pour les couches cachées
long double NeuralNet::Calc_Delta_Hidden_Layer(int couche, int cell_index, long double sortie)
{
	long double somme_erreur = 0.0;
	Link_class* temps_post_link = Cells[couche][cell_index].Post_Link_Chain;
	do
	{
		somme_erreur += (temps_post_link->Weight * temps_post_link->To_cell->Delta);
		temps_post_link = temps_post_link->Next_Post_Link_Chain;
	}while(temps_post_link != NULL);
	return somme_erreur*sortie*(1-sortie);
}*/



/*void NeuralNet::Test_Categorisation(Sequence* sequence, int pattern_index, test_results_class* test_results)
{
	Load_input_pattern(sequence->patterns_IN[pattern_index], pattern_index == 0);
	Propager();
	int index_max_sortie = 0;
	double max_sortie = 0;
	Cellule_class* Temp_link_last_layer = Cells[Layer_Number_moins_un];
	do
	{
		if(max_sortie < Temp_link_last_layer->Sortie)
		{
			max_sortie = Temp_link_last_layer->Sortie;
			index_max_sortie = Temp_link_last_layer->index;
		}
		Temp_link_last_layer = Temp_link_last_layer->Next_Cell_Chain_NoBiais;
	}while(Temp_link_last_layer != NULL);
	test_results->categorisation = index_max_sortie;
	//cout << "test_results->categorisation = " << test_results->categorisation << endl;
}*/




/*//Calcul du delta pour la derniere couche
long double NeuralNet::Calc_Delta_Last_Layer(int cell_index, double sortie_desiree)
{
	//a=sortie y=sortie desiree
	//QUADRATIQUE = Delta = (a-y)*a*(1-a)
	//return (Cells[Layer_Number_moins_un][cell_index].Sortie-sortie_desiree)*Cells[Layer_Number_moins_un][cell_index].Sortie*(1-Cells[Layer_Number_moins_un][cell_index].Sortie);
	//CROSS-ENTROPY = Delta = (a-y)
	return (Cells[Layer_Number_moins_un][cell_index].Sortie-sortie_desiree);
}*/

/*void NeuralNet::Test_Pattern(Sequence* sequence, int sequence_index, int pattern_index, test_results_class* test_results, bool learnbase, bool confusion)
{
	Load_input_pattern(sequence->patterns_IN[pattern_index], pattern_index == 0);
	Propager();
	if(learnbase) {
		test_results->erreur[0] = 0;
		Cellule_class* Temp_link_last_layer = Cells[Layer_Number_moins_un];
		do
		{
			test_results->erreur[0] += abs(Temp_link_last_layer->Sortie - sequence->patterns_OUT[pattern_index][Temp_link_last_layer->index]);
			Temp_link_last_layer = Temp_link_last_layer->Next_Cell_Chain_NoBiais;
		}while(Temp_link_last_layer != NULL);
		test_results->erreur[0] /= Cells_Per_Layer.at(Layer_Number_moins_un);
	} else {
		double erreur_emotion[Full_Database.number_of_categories] = {0.0};
		int index_erreur_min = 0;
		int index_max_sortie = 0;
		double max_sortie = 0;
		if(confusion) { //TEST DE CONFUSION
			for(int i=0 ; i<NB_OF_EMOTION ; i++)
			{
				Cellule_class* Temp_link_last_layer = Cells[Layer_Number_moins_un];
				do
				{
					erreur_emotion[i] += abs(Temp_link_last_layer->Sortie - Database_Test.Sequences[i][sequence_index].patterns_OUT[pattern_index][Temp_link_last_layer->index]);
					Temp_link_last_layer = Temp_link_last_layer->Next_Cell_Chain_NoBiais;
				}while(Temp_link_last_layer != NULL);
				if(erreur_emotion[i] < erreur_emotion[index_erreur_min]) index_erreur_min=i;
			}
			test_results->succes = (index_erreur_min == sequence->categorie);
		} else { //TEST DE CATEGORISATION
			Cellule_class* Temp_link_last_layer = Cells[Layer_Number_moins_un];
			do
			{
				erreur_emotion[sequence->categorie] += abs(Temp_link_last_layer->Sortie - sequence->patterns_OUT[pattern_index][Temp_link_last_layer->index]);
				if(max_sortie < Temp_link_last_layer->Sortie)
				{
					max_sortie = Temp_link_last_layer->Sortie;
					index_max_sortie = Temp_link_last_layer->index;
				}
				Temp_link_last_layer = Temp_link_last_layer->Next_Cell_Chain_NoBiais;
			}while(Temp_link_last_layer != NULL);
			test_results->categorisation = index_max_sortie;
			test_results->succes = (test_results->categorisation == sequence->categorie);
		}
		test_results->erreur[0] = erreur_emotion[sequence->categorie]/(Cells_Per_Layer.at(Layer_Number_moins_un));
	}

	//if(NB_OF_REINJECTIONS > 0 && (pattern_index != 0) != 0) Reinjection(emo_index, pat_no+1, test_results, 1);
}*/
