#pragma once
#include <list>
#include "Tangle.h"

using namespace std;

class Dag
{
public:
	list<Tangle*> Tangles;
	Dag();
};

