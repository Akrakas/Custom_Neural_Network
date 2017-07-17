#include "Database_class.h"

Database_class::Database_class()
{
	Sequences = NULL;
}

Database_class::~Database_class()
{
	if(Sequences!=NULL){
		for(int i=0 ; i<number_of_categories ; i++)
		{
			for(int j=0 ; j<number_of_sequences ; j++)
			{
				Sequences[i][j].~Sequence();
			}
			free(Sequences[i]);
		}
		free(Sequences);
		Sequences = NULL;
	}
}

void Database_class::Clear_Database()
{
	if(Sequences!=NULL){
		for(int i=0 ; i<number_of_categories ; i++)
		{
			for(int j=0 ; j<number_of_sequences ; j++)
			{
				Sequences[i][j].~Sequence();
			}
			free(Sequences[i]);
		}
		free(Sequences);
		Sequences = NULL;
	}
}
