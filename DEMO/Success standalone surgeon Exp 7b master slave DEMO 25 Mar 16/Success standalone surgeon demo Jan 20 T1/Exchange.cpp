// modified to get pos and vel
#include "Exchange.h"

void Exchange::Move(String^ message)
{
	Xs = message;
	Ys = message;
	
}

double Exchange::Max1s()
{
	//Maxon one linear motion
	double scale1 = 0.01*29.3; // scaling factor for velocity  (60.0*1000.0*y)/(R*1); R = 2048.0;

	double xCopy;
	Monitor::Enter(this);

	static string s3;
	MarshalString3(Xs, s3);

	int index1;
	int index2;

	// modified to get pos and vel
	std::string search_str1 = std::string("a");
	std::string search_str2 = std::string("b");

	index1 = s3.find_first_of(search_str1);
	index2 = s3.find_first_of(search_str2);

	std::string str1 = s3.substr(1, index2 - 1);
	xCopy = atof(str1.c_str());

	Monitor::Exit(this);
	//cout << xCopy;
	return (scale1*xCopy);
}

double Exchange::Max2s()
{
	//Maxon two rotary motion
	double scale2 = 0.04*29.3; // scaling factor for velocity double R=2048.0;   0.04*29.3

	double yCopy;
	Monitor::Enter(this);

	static string s3;
	MarshalString4(Ys, s3);
 
	int index2;
	int index3;

	std::string search_str2 = std::string("b");
	std::string search_str3 = std::string("c");

	index2 = s3.find_first_of(search_str2);
	index3 = s3.find_first_of(search_str3);

	std::string str2 = s3.substr(index2 + 1, index3 - 1);
	yCopy = atof(str2.c_str());
	
	Monitor::Exit(this);
	return (scale2*yCopy);
}


double Exchange::Max1pos()
{
	//Maxon one linear motion
	double scale3 = 0.015; 
	// resolution of encoder count 2048
	// circumference 2*pi*r  where r= 5mm
	// 1 count = 2*pi*r/2048 = 0.015mm 
	double xpCopy;
	Monitor::Enter(this);

	static string s3;
	MarshalString5(Xs, s3);

	int index3;
	int index4;

	std::string search_str3 = std::string("c");
	std::string search_str4 = std::string("d");

	index3 = s3.find_first_of(search_str3);
	index4 = s3.find_first_of(search_str4);	
	 
	std::string str3 = s3.substr(index3 + 1, index4 - 1);
	xpCopy = atof(str3.c_str());

	Monitor::Exit(this);
	return (scale3*xpCopy);
}

void Exchange::MoveTorque(double zor)
{

	Monitor::Enter(this);
	Z = zor;
	Monitor::Exit(this);
}
double Exchange::Max3()
{
	double zCopy;
	Monitor::Enter(this);
	zCopy = Z;
	Monitor::Exit(this);
	return zCopy;
}


void Exchange::MarshalString3(String ^ s, string& os)
{
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

void Exchange::MarshalString4(String ^ s, string& os)
{
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

void Exchange::MarshalString5(String ^ s, string& os)
{
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}