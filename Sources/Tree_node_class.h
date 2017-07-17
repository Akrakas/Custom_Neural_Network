#ifndef TREE_NODE_CLASS_H
#define TREE_NODE_CLASS_H
#include "def.h"
#include "function.h"
#include "Cellule.h"

class Database_class
{
    public :
		Sequence** Sequences;
		int input_size;
		int output_size;
		int number_of_categories;
		int number_of_patterns;
		int number_of_sequences;
		string Previous_Database_Path;

		Database_class();
		~Database_class();
		void Clear_Database();
};

#endif // TREE_NODE_CLASS_H
