#ifndef CELLULE_CLASS_H
#define CELLULE_CLASS_H
#include "def.h"
#include "function.h"

class Link_class;
class Cellule_class
{
    public :
        Cellule_class();
        ~Cellule_class();

		Link_class* Pre_Link_Chain;
		Link_class* Post_Link_Chain;
		Cellule_class* Next_Cell_Chain_WithBiais;
		Cellule_class* Next_Cell_Chain_NoBiais;
		Cellule_class* Next_Cell_Chain_NoReinjection;
		long double* Weight;
		long double* Weight_Variation;
        long double Alpha;
        long double Sortie;
        long double Delta;
		int number_of_connexions;
		int number_of_cells_in_layer;
		int index;
		int layer_index;
};

#endif // CELLULE_CLASS_H
