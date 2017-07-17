#include "Sequence.h"

Sequence::Sequence()
{
	patterns_IN_vec.clear();
	patterns_OUT = NULL;
	Next_Sequence = NULL;
}

Sequence::~Sequence()
{	
	//cout << "Detruit ! " << endl;
	for(int i=0 ; i<patterns_IN_vec.size() ; i++)
	{
		for(int j=0 ; j<number_of_patterns ; j++)
		{
			if(patterns_IN_vec.at(i)[j] != NULL) {free(patterns_IN_vec.at(i)[j]);patterns_IN_vec.at(i)[j] = NULL;}
		}
		if(patterns_IN_vec.at(i) != NULL) {free(patterns_IN_vec.at(i));patterns_IN_vec.at(i) = NULL;}
	}
	patterns_IN_vec.clear();
	if(patterns_OUT != NULL)
	{
		for(int i=0 ; i<number_of_patterns ; i++)
		{
			if(patterns_OUT[i] != NULL) {free(patterns_OUT[i]);patterns_OUT[i] = NULL;}
		}
		if(patterns_OUT != NULL) {free(patterns_OUT);patterns_OUT = NULL;}
	}
}

Sequence& Sequence::operator=(const Sequence &other)
{
	if(this == &other) return *this;
	pattern_size_IN_vec.resize(other.pattern_size_IN_vec.size());
	for(int i=0 ; i<pattern_size_IN_vec.size() ; i++)
	{
		pattern_size_IN_vec.at(i) = other.pattern_size_IN_vec.at(i);
	}
	pattern_size_OUT = other.pattern_size_OUT;
	number_of_patterns = other.number_of_patterns;
	categorie = other.categorie;
	Next_Sequence = NULL;
	patterns_IN_vec.resize(other.patterns_IN_vec.size());
	for(int i=0 ; i<patterns_IN_vec.size() ; i++)
	{
		patterns_IN_vec.at(i) = (double**)calloc(number_of_patterns, sizeof(double*));
		for(int j=0 ; j<number_of_patterns ; j++)
		{
			patterns_IN_vec.at(i)[j] = (double*)calloc(pattern_size_IN_vec.at(i), sizeof(double));
			memcpy(patterns_IN_vec.at(i)[j], other.patterns_IN_vec.at(i)[j], sizeof(double)*pattern_size_IN_vec.at(i));
		}
	}
	patterns_OUT = (double**)calloc(number_of_patterns, sizeof(double*));
	for(int i=0 ; i<number_of_patterns ; i++)
	{
		patterns_OUT[i] = (double*)calloc(pattern_size_OUT, sizeof(double));
		memcpy(patterns_OUT[i], other.patterns_OUT[i], sizeof(double)*pattern_size_OUT);
	}
}

int Sequence::Get_in_sequence_from_file(string filename_in, int _categorie, int _number_of_patterns, int _pattern_size_in)
{
	categorie = _categorie;
	number_of_patterns = _number_of_patterns;

	pattern_size_IN_vec.push_back(_pattern_size_in);
	double** temp_pat_in = (double**)calloc(number_of_patterns, sizeof(double*));
	for(int j=0 ; j<number_of_patterns ; j++)
	{
		temp_pat_in[j] = (double*)calloc(pattern_size_IN_vec.back(), sizeof(double));
	}
	patterns_IN_vec.push_back(temp_pat_in);

    ifstream pat_in_file (filename_in, ios::in);
    if (pat_in_file.fail()){
        perror(("Can't find " + filename_in).c_str());
        return 0;
    }
    double pat_in_data;
	for(int j=0 ; j<number_of_patterns ; j++) {
		for(int k=0 ; k<pattern_size_IN_vec.back() ; k++) {
			pat_in_file >> pat_in_data;
	        patterns_IN_vec.back()[j][k] = pat_in_data;
		}
	}
    pat_in_file.close();
	return 1;
}

int Sequence::Get_out_sequence_from_file(string filename_out, int _categorie, int _number_of_patterns, int _pattern_size_out)
{
	categorie = _categorie;
	number_of_patterns = _number_of_patterns;
	pattern_size_OUT = _pattern_size_out;
	patterns_OUT = (double**)calloc(number_of_patterns, sizeof(double*));
	for(int i=0 ; i<number_of_patterns ; i++)
	{
		patterns_OUT[i] = (double*)calloc(pattern_size_OUT, sizeof(double));
	}

    ifstream pat_out_file (filename_out, ios::in);
    if (pat_out_file.fail()){
        perror(("Can't find " + filename_out).c_str());
        return 0;
    }
    double pat_out_data;
	for(int i=0 ; i<number_of_patterns ; i++) {
		for(int j=0 ; j<pattern_size_OUT ; j++) {
			pat_out_file >> pat_out_data;
            patterns_OUT[i][j] = pat_out_data;
		}
	}
    pat_out_file.close();
	return 1;
}

