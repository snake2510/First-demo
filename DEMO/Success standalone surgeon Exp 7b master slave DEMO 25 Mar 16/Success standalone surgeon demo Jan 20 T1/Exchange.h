#ifndef EXCHANGE_H
#define EXCHANGE_H

#using <System.dll>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <tchar.h>
#include <algorithm>
#include <time.h>
#include <windows.h>
#include <math.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <iomanip>

using namespace std;
using namespace System;
using namespace System::IO::Ports;
using namespace System::Threading;

public ref class Exchange
{
public:

	double z;
	String^ ys;
	String^ xs;

	// define read-write instance properties X and Y

	property String^ Xs
	{	String^ get() { return xs; }
	void set(String^ val) { xs = val; }
	}
	property String^ Ys
	{	String^ get() { return ys; }
	void set(String^ val) { ys = val; }
	}
	property double Z
	{	double get() { return z; }
	void set(double val) { z = val; }
	}

	void Move(String^ message);
	void MoveTorque(double zor);

	virtual double Max1s();
	virtual double Max1pos();
	virtual double Max2s();
	virtual double Max3();

	void MarshalString3(String ^ s, string& os);
	void MarshalString4(String ^ s, string& os);
	void MarshalString5(String ^ s, string& os);
};

#endif