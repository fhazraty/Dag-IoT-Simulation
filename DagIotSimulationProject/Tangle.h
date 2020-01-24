#pragma once
class Tangle
{
public:
	Tangle* ParentLeft;
	Tangle* ParentRight;
	int PayloadData;
	int confirmationLevel;
	//0- not confirmed
	//1- partially confirmed
	//2- fully confirmed
};

