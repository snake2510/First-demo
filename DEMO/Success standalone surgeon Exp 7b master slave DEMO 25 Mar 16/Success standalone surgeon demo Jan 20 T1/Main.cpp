/*

Settings:
To establish runtime communication with the Arduino
1. Project-> Properties-> Configuration Properties-> General-> Common Language Runtime support-> /clr

2.

Reference:
www.drdobbs.com/cpp/ccli-threading-part-i/184402018

*/
#include "Exchange.h"
#include "ISR.h"

int main()
{
	cout << "Interventional Surgical Robot" << endl << endl;
	Exchange^ p = gcnew Exchange;

	ISR^ o1 = gcnew ISR('a', p);
	Thread^ t1 = gcnew Thread(gcnew ThreadStart(o1, &ISR::StartUp));

	ISR^ o2 = gcnew ISR('b', p);
	Thread^ t2 = gcnew Thread(gcnew ThreadStart(o2, &ISR::StartUp));

	ISR^ o3 = gcnew ISR('c', p);
	Thread^ t3 = gcnew Thread(gcnew ThreadStart(o3, &ISR::StartUp));

	ISR^ o4 = gcnew ISR('d', p);
	Thread^ t4 = gcnew Thread(gcnew ThreadStart(o4, &ISR::StartUp));

	t1->Start();
	t2->Start();
	t3->Start();
	t4->Start();

	t1->Join();
	t2->Join();
	t3->Join();
	t4->Join();

}