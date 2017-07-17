#ifndef LINK_CLASS_H
#define LINK_CLASS_H
#include "def.h"
#include "function.h"

class Cellule_class;
class Link_class
{
    public :
        Link_class();
        ~Link_class();
		void Link_Init();

		Cellule_class* From_cell;
		Cellule_class* To_cell;
		Link_class* Next_Pre_Link_Chain;
		Link_class* Next_Post_Link_Chain;
		long double Weight;
		long double Weight_Variation;
};

#endif // LINK_CLASS_H
