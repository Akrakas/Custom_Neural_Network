#ifndef DATABASE_CLASS_H
#define DATABASE_CLASS_H
#include "def.h"
#include "function.h"
#include "Sequence.h"

class Database_class
{
    public :
		Sequence** Sequences;
		int number_of_inputs;
		vector<int> input_size;
		int output_size;
		int number_of_categories;
		int number_of_patterns;
		int number_of_sequences;
		string Previous_Database_Path;

		Database_class();
		~Database_class();
		void Clear_Database();
};

#endif // DATABASE_CLASS_H
