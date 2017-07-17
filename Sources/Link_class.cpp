#include "Link_class.h"

Link_class::Link_class()
{
	From_cell = NULL;
	To_cell = NULL;
	Next_Pre_Link_Chain = NULL;
	Next_Post_Link_Chain = NULL;
	Weight = 0;
	Weight_Variation = 0;
}

Link_class::~Link_class()
{
}

void Link_class::Link_Init()
{
}
