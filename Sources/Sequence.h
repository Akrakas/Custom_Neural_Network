#ifndef SEQUENCE_H
#define SEQUENCE_H
#include "def.h"
#include "function.h"

class Sequence
{
	//Pour l'instant, les sequences ne gerent que les entrees multiples dans le reseau, et non les sorties multiples
    public :
        Sequence();
        ~Sequence();
		Sequence& operator=(const Sequence &other);

		vector<double**> patterns_IN_vec;
		double** patterns_OUT;
		vector<int> pattern_size_IN_vec;
		int pattern_size_OUT;
		int number_of_patterns;
		int categorie;
		Sequence* Next_Sequence;
		
		int Get_in_sequence_from_file(string filename_in, int _categorie, int _number_of_patterns, int _pattern_size_in);
		int Get_out_sequence_from_file(string filename_out, int _categorie, int _number_of_patterns, int _pattern_size_out);
};

#endif // SEQUENCE_H
