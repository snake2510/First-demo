#ifndef ISR_H
#define ISR_H

#pragma comment(lib,"EposCmd.lib")
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

//saving output
#include <fstream>
#include <sstream>

//#include <process.h>
#include "Definitions.h"		// Library Maxon EPOS 24/1 controller
#include "SerialClass.h"	

#pragma once
#include "Exchange.h" 

using namespace std;
using namespace System;
using namespace System::IO::Ports;
//using namespace System::Security::Permissions;
using namespace System::Threading;

public ref class ISR
{
	Exchange^ pnt;
	char LoopVar;

	// Ardunio variable
	static SerialPort^ _ArduinoSerial;
	static SerialPort^ _ArduinoSerial2;
	int STOPflag;

	// Maxon variable
	char *DeviceName;
	DWORD Baudrate;
	DWORD Timeout;

	char *ProtocolStackName;
	char *InterfaceName;
	char *PortName;
	char *ProtocolStackName2;
	char *InterfaceName2;
	char *PortName2;

	HANDLE KeyHandle;
	HANDLE KeyHandle2;
	DWORD *ErrorCode;
	DWORD *ErrorCode2;
	WORD NodeId1;
	WORD NodeId2;

	// velocity unit setting
	BYTE VelDimension;
	char VelNotation;

	DWORD* ProfileVelocity;
	DWORD* ProfileAcceleration;
	DWORD* ProfileDeceleration;

	__int8* Mode;
	bool Immediately;
	bool UpdateActive;

	float MotorSpeed;
	long* StartPosition;
	long TargetPosition;
	long TargetVelocity;
	long VelocityMust;
	long RpmDesired;
	long ActualValue;
	int Radio;


	int ModeType;
	int oldMT;
	char key;

	double Pos;
	double Vel;

public:
	ISR(char isMover, Exchange^ p);
	void StartUp();

	void Read();
	void MarshalString(String ^ s, string& os);
	void MarshalString2(String ^ s, string& os);

};

#endif