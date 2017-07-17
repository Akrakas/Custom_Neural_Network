#include "Cellule_class.h"

Cellule_class::Cellule_class()
{
	Next_Cell_Chain_WithBiais = NULL;
	Next_Cell_Chain_NoBiais = NULL;
	Next_Cell_Chain_NoReinjection = NULL;
	Pre_Link_Chain = NULL;
	Post_Link_Chain =NULL;
	Weight = NULL;
	Weight_Variation = NULL;
	Alpha = 1.0;//Rdm_Inter(0.5,1.0);
	Sortie = 1.0;
	Delta = 1.0;
	number_of_connexions = 0;
	index = -1;
	layer_index = -1;
}

Cellule_class::~Cellule_class()
{ 
}
