#include"ISR.h"

ISR::ISR(char isMover, Exchange^ p)
{
	LoopVar = isMover;
	pnt = p;

	//Maxon variable decleration
	DeviceName = "EPOS2";
	ProtocolStackName = "MAXON SERIAL V2";
	InterfaceName = "USB";
	PortName = "USB0";

	ProtocolStackName2 = "MAXON SERIAL V2"; //CANopen
	InterfaceName2 = "USB";					//IXXAT_USB-to-CAN compact 0
	PortName2 = "USB0";

	Baudrate = 1000000;		// 115200;
	Timeout = 500;

	DWORD PV = 5000;
	DWORD PA = 100;
	DWORD PD = 100;

	ProfileVelocity = &PV;
	ProfileAcceleration = &PA;
	ProfileDeceleration = &PD;

	NodeId1 = 1;
	NodeId2 = 2;

	VelDimension = 0xA4;
	VelNotation = 0;

	// Mode Setting variable
	const char VELOCITY_MODE = '-2';
	const char POSITION_MODE = '-1';

	Immediately = TRUE;
	UpdateActive = FALSE;

	RpmDesired = 5000;
	Radio = 0;

	key = 0;
	STOPflag = 0;
}

void ISR::StartUp()
{
	double scx = 10.0;
	double scy = 10.0;
	// 1. Arduino
	switch (LoopVar)
	{
	case 'a':
	{
		cout << " Arduino Started" << endl;
		ISR^ obj2 = gcnew ISR('d', pnt);
		Thread^	readThread = gcnew Thread(gcnew ThreadStart(obj2, &ISR::Read));

		_ArduinoSerial = gcnew SerialPort();
		_ArduinoSerial->PortName = "COM4";
		_ArduinoSerial->BaudRate = 9600;
		_ArduinoSerial->DataBits = 8;
		_ArduinoSerial->Parity = Parity::None;
		_ArduinoSerial->StopBits = StopBits::One;
		_ArduinoSerial->Handshake = Handshake::RequestToSend;
		_ArduinoSerial->ReadTimeout = 500;
		_ArduinoSerial->WriteTimeout = 500;

		_ArduinoSerial->Open();
		readThread->Start();

		double zTorque = 0.0;
		while (1)
		{
			_ArduinoSerial->Write(String::Format("<{0}>: {1}", "s", "\n"));

		}
		readThread->Join();
		_ArduinoSerial->Close();
	}
	break;

	// 2. Maxon one Linear motion
	case 'b':
	{

		double x = 0.0;
		double oldx = 0.0;
		double oldxx = 0.0;
		int exitflag = 0;
		double diffx = 0;
		int signx = 1;

		KeyHandle = VCS_OpenDevice(DeviceName, ProtocolStackName, InterfaceName, PortName, ErrorCode);
		if (KeyHandle)
		{
			cout << " Initiating motor 1 " << endl;
			if (VCS_SetProtocolStackSettings(KeyHandle, Baudrate, Timeout, ErrorCode))
			{
				if (VCS_SetProtocolStackSettings(KeyHandle, Baudrate, Timeout, ErrorCode))
				{
					if (VCS_SetProtocolStackSettings(KeyHandle, Baudrate, Timeout, ErrorCode))
					{
						if (VCS_ClearFault(KeyHandle, NodeId1, ErrorCode))
						{
							if (VCS_GetOperationMode(KeyHandle, NodeId1, Mode, ErrorCode))
							{
								if (VCS_GetPositionProfile(KeyHandle, NodeId1, ProfileVelocity, ProfileAcceleration, ProfileDeceleration, ErrorCode))
								{
									__int8 VMode = -2;	 // VELOCITY_MODE
									if (VCS_SetOperationMode(KeyHandle, NodeId1, VMode, ErrorCode))
									{	//Velocity mode 
										cout << endl << " VELOCITY_MODE 1" << endl;
										if (VCS_ActivateVelocityMode(KeyHandle, NodeId1, ErrorCode))
										{
											BOOL oFault = FALSE;
											if (!VCS_GetFaultState(KeyHandle, NodeId1, &oFault, ErrorCode))
											{ {printf("error.1a\n");exitflag += 1;}
											}
											if (oFault)
											{
												if (!VCS_ClearFault(KeyHandle, NodeId1, ErrorCode))
												{ {printf("error.2a\n");exitflag += 1;}
												}
											}
											if (!VCS_SetEnableState(KeyHandle, NodeId1, ErrorCode))
											{ {printf("error.3a\n");exitflag += 1;}
											}

											cout << " Motor 1 setup done" << endl;

										}
										else { printf("error.4a\n"); }
									}
									else { printf("error.5a\n"); }
								}
								else { printf("error.6a\n"); }
							}
							else { printf("error.7a\n"); }
						}
						else { printf("error.8a  Check Power Supply\n"); }
					}
					else { printf("error.9a\n"); }
				}
				else { printf("error.10a\n"); }
			}
			else { printf("error.11a\n"); }
		}
		else { printf("error.12a\n"); }

		if (exitflag == 0)
		{
			cout << " Motor 1 Control Begins " << endl;

			double Torquefilt = 0.0; // Actual system Torque 
			double Kt2 = 0.051;	// Torque constant Unit: Nm
			double TorqueEMF = 0.0;
			double TorqueM = 0.0;	//Torque of motor
			double Jm = 0.0091;//92.5*(10^-5)*9.81; // moment of inertia of motor from datasheet //gcm^2 to Kgm^2 to Nm^2
			double Rpm2rad_ms = 0.1047197*0.001;
			double oldvelA = 0;
			double tm = 17.2; //mechanical time constant ms
			double Bm1 = Jm / tm; // Damping coefficient of motor from calculation 
			int count = 1;
			short CurrentA;// current variables	
			long VelocityA;// Velocity variables
			long PositionIs;
			long PositionIsStart;
			double x = 0.0;
			double velx = 0.0;

			double posx_encoder = 0.0; // for encoder data
			double posx_intit = 0.0; // for encoder data
			double posx_error = 0.0; // for encoder data

			BOOL oR1;
			BOOL oR2;
			BOOL oR3;

			double xn = 0;
			double xn_1 = 0;
			double xn_2 = 0;

			double yn = 0;
			double yn_1 = 0;
			double yn_2 = 0;
			double Currentfilt = 0;

			double a[3] = { 1,-1.2796,0.4775 };
			double b[3] = { 0.0494,0.0989,0.0494 };

			//double a[3] = {1,-1.279632424997809,0.477592250072517};
			//double b[3] = {0.049489956268677,0.098979912537354,0.049489956268677};
			
			//LinearForce_mapping_Y_phantom_01.txt

			//cout << "Getting File Information..." << endl;
			ofstream file;
			file.open("CalibTorque_springtip_Translation_GW_01.txt");    // open file , ios::in // Current Torque toerqueEmf Load Velocity
			cout << " Translation File Ready" << endl;

			// Intial values 
			clock_t start = clock();
			oR3 = VCS_GetPositionIs(KeyHandle, NodeId1, &PositionIsStart, ErrorCode); // pos value of motor 
			// resolution of encoder count 8192
			// circumference 2*pi*r  where r= 5mm
			// 1 count = 2*pi*r/8192 = 0.0038mm 
			
			posx_intit = pnt->Max1pos(); // pos value on the encoder
			// resolution of encoder count 2048
			// circumference 2*pi*r  where r= 5mm
			// 1 count = 2*pi*r/2048 = 0.015mm 

			while (1)
			{
				velx = pnt->Max1s();
				//	velx=(60.0*1000.0*x)/(R*1); // cal part ofr RPM 
				//	cout<<"   V:  "<<x<<"   VV:  "<<velx<<endl;
				//	cout << "   V:  " << x << "   VV:  " << velx << endl;

				diffx = velx - oldx;
				signx = diffx<0 ? -1 : 1;
				diffx = abs(diffx);
				float sscx = diffx<100 ? 5 : diffx / 5;
				scx = diffx / sscx;

				if (diffx != 0)
				{
					for (double ss = 0; ss <= diffx;ss = ss + (diffx / scx))
					{
						oldxx = oldx + (signx*ss);
						oldxx = abs(oldxx)<0.001 ? 0 : oldxx;
						
						if (posx_error > 0)
						{
							oldxx = oldxx + posx_error;
						}

						if (!VCS_SetVelocityMust(KeyHandle, NodeId1, oldxx, ErrorCode))
						{
							printf("error.4a\n");
						}

						oR1 = VCS_GetVelocityIs(KeyHandle, NodeId1, &VelocityA, ErrorCode);
						oR2 = VCS_GetCurrentIs(KeyHandle, NodeId1, &CurrentA, ErrorCode);
						oR3 = VCS_GetPositionIs(KeyHandle, NodeId1, &PositionIs, ErrorCode);

						xn = CurrentA;
						//yn= (1/a[0])*( b[0]*xn + b[1]*xn_1 + b[2]*xn_2 - a[1]*yn_1 -a[1]*yn_2);  :: a[0]=1
						yn = (b[0] * xn + b[1] * xn_1 + b[2] * xn_2 - a[1] * yn_1 - a[2] * yn_2);
						Currentfilt = yn;
						Torquefilt = Kt2*Currentfilt;
						// Motor specification part # 339281 + encoder
						// Speed constant: 187rpm/V
						// Terminal resistance phase to phase: 4.83 ohm
						TorqueEMF = Kt2*((abs(VelocityA) / 187.) / (4.83)); //current=(Vs-Vemf)/R=(Vs-Ki*w)/R 
						TorqueM = (Jm*(VelocityA - oldvelA)*Rpm2rad_ms) + (Bm1*VelocityA*Rpm2rad_ms);

						//get encoder position value from the arduino
						posx_encoder = pnt->Max1pos();
						file << CurrentA << " " << Kt2*CurrentA << " " << Torquefilt << " " << TorqueEMF << " " << TorqueM << " " << (Torquefilt - TorqueM) << " " << (double(clock() - start) / double(CLOCKS_PER_SEC)) << " " << (PositionIs - PositionIsStart)*0.0038 << " " << VelocityA << " "<< posx_encoder -posx_intit<<" "<< oldxx <<" "<< velx <<" " << VelocityA <<endl;
						pnt->MoveTorque(Torquefilt);	
						posx_error =(posx_encoder - posx_intit)- ((PositionIs - PositionIsStart)*0.0038);
						//cout<<velx<<" ss "<<ss<<" vel "<<oldxx<<" D  "<<diffx<<endl;
						// file<<CurrentA<<" "<<Currentfilt<<" "<<Torque<<" "<<TorqueEMF<<" "<<TorqueM<<" "<<(Torque-TorqueM)<<" "<<double((clock()-start)/CLOCKS_PER_SEC)<<" "<<PositionIs-PositionIsStart<<" "<<VelocityA<<endl;
						// if(Torque>5000){Beep(523,50);}
						// cout << PositionIs - PositionIsStart << endl;
						// cout << (double(clock() - start) / double(CLOCKS_PER_SEC)) <<endl;
					}

					xn_2 = xn_1;
					xn_1 = xn;
					yn_2 = yn_1;
					yn_1 = yn;

					diffx = 0;
					oldxx = 0;
					oldx = velx;
					oldvelA = VelocityA;

					if (!VCS_SetVelocityMust(KeyHandle, NodeId1, oldxx, ErrorCode))
					{
						printf("error.4a\n");
					}
				}
			}
			file.close();

		}
	}
	break;

	// 3. Maxon two rotary motion
	case 'c':
	{
		double y = 0.0;
		double oldy = 0.0;
		double oldyy = 0.0;
		double diffy = 0;
		int signy = 1;
		int ex = 0;
		int exitflag2 = 0;

		KeyHandle2 = VCS_OpenDevice(DeviceName, ProtocolStackName2, InterfaceName2, PortName2, ErrorCode2);
		if (KeyHandle2)
		{
			cout << " Initiating Motors 2 " << endl;
			if (VCS_SetProtocolStackSettings(KeyHandle2, Baudrate, Timeout, ErrorCode2))
			{
				if (VCS_SetProtocolStackSettings(KeyHandle2, Baudrate, Timeout, ErrorCode2))
				{
					if (VCS_SetProtocolStackSettings(KeyHandle2, Baudrate, Timeout, ErrorCode2))
					{
						if (VCS_ClearFault(KeyHandle2, NodeId2, ErrorCode2))
						{
							if (VCS_GetOperationMode(KeyHandle2, NodeId2, Mode, ErrorCode2))
							{
								if (VCS_GetPositionProfile(KeyHandle2, NodeId2, ProfileVelocity, ProfileAcceleration, ProfileDeceleration, ErrorCode2))
								{
									__int8 VMode = -2;	 // VELOCITY_MODE
									if (VCS_SetOperationMode(KeyHandle2, NodeId2, VMode, ErrorCode2))
									{	//Velocity mode 
										cout << endl << " VELOCITY_MODE 2" << endl;
										if (VCS_ActivateVelocityMode(KeyHandle2, NodeId2, ErrorCode2))
										{
											BOOL oFault2 = FALSE;
											if (!VCS_GetFaultState(KeyHandle2, NodeId2, &oFault2, ErrorCode2))
											{ {printf("error.1b\n");exitflag2 += 1;}
											}
											if (oFault2)
											{
												if (!VCS_ClearFault(KeyHandle2, NodeId2, ErrorCode2))
												{ {printf("error.2b\n");exitflag2 += 1;}
												}
											}
											if (!VCS_SetEnableState(KeyHandle2, NodeId2, ErrorCode2))
											{ {printf("error.3b\n");exitflag2 += 1;}
											}
											cout << " Motor 2 setup done" << endl;
										}
										else { printf("error.4b\n"); }
									}
									else { printf("error.5b\n"); }
								}
								else { printf("error.6b\n"); }
							}
							else { printf("error.7b\n"); }
						}
						else { printf("error.8b Check Power Supply \n"); }
					}
					else { printf("error.9b\n"); }
				}
				else { printf("error.10b\n"); }
			}
			else { printf("error.11b\n"); }
		}
		else { printf("error.12b\n"); }

		if (exitflag2 == 0)
		{
			cout << " Motor 2 Control Begins " << endl;

			double y = 0.0;
			double vely = 0.0;
			
			double Torquefilt = 0.0; // Actual system Torque 
			double Kt2 = 0.051;	// Torque constant Unit: Nm
			double TorqueEMF = 0.0;
			double TorqueM = 0.0;	//Torque of motor
			double Jm = 0.0091;//92.5*(10^-5)*9.81; // moment of inertia of motor from datasheet //gcm^2 to Kgm^2 to Nm^2
			double Rpm2rad_ms = 0.1047197*0.001;
			double oldvelA = 0;
			double tm = 17.2; //mechanical time constant ms
			double Bm1 = Jm / tm; // Damping coefficient of motor from calculation 
			int count = 1;
			short Current_R;// current variables	
			long Velocity_R;// Velocity variables
			long PositionIs_R;
			long PositionIsStart_R;
			double x_R= 0.0;
			double velx_R = 0.0;

			double posx_R = 0.0; // for encoder data
			double posx_intit_R = 0.0; // for encoder data
			BOOL oR1_R;
			BOOL oR2_R;
			BOOL oR3_R;

			double xn = 0;
			double xn_1 = 0;
			double xn_2 = 0;

			double yn = 0;
			double yn_1 = 0;
			double yn_2 = 0;
			double Currentfilt = 0;

			double a[3] = { 1,-1.2796,0.4775 };
			double b[3] = { 0.0494,0.0989,0.0494 };

			//cout << "Getting File Information..." << endl;
			ofstream file;
			file.open("CalibTorque_springtip_Rotational_GW_01.txt");    // open file , ios::in // Current Torque toerqueEmf Load Velocity
			cout << " Rotation File Ready" << endl;

			// Intial values 
			clock_t start_R = clock();
			oR3_R = VCS_GetPositionIs(KeyHandle, NodeId1, &PositionIsStart_R, ErrorCode); // pos value of motor 
			// resolution of encoder count 8192
			// circumference 2*pi*r  where r= 5mm
			// 1 count = 2*pi*r/8192 = 0.0038mm 

			posx_intit_R = pnt->Max1pos(); // pos value on the encoder
			// resolution of encoder count 2048
			// circumference 2*pi*r  where r= 5mm
			// 1 count = 2*pi*r/2048 = 0.015mm 


			while (1)
			{
				vely = pnt->Max2s();
				//cout<<"org "<<y<<"  velS  "<<vely<<endl;
				diffy = vely - oldy;
				signy = diffy<0 ? -1 : 1;
				diffy = abs(diffy);
				float sscy = diffy<100 ? 5 : diffy / 5;  //float sscy = diffy<100 ? 10 : diffy / 5;
				scy = diffy / sscy;

				if (diffy != 0)
				{
					for (double ss = 0; ss <= diffy;ss = ss + (diffy / scy))
					{
						oldyy = oldy + (signy*ss);
						oldyy = abs(oldyy)<0.001 ? 0 : oldyy;

						if (!VCS_SetVelocityMust(KeyHandle2, NodeId2, oldyy, ErrorCode2))
						{
							printf("error.4b\n");
						}
					}
					diffy = 0;
					oldyy = 0;
					oldy = vely;

					if (!VCS_SetVelocityMust(KeyHandle2, NodeId2, oldyy, ErrorCode2))
					{
						printf("error.4b\n");
					}
				}

			}
		}
	}
	break;
	//Arduino 2 Haptic feedback
	case 'd':
	{
		cout << " Arduino 2 Haptic feedback Started" << endl;
		_ArduinoSerial2 = gcnew SerialPort();
		_ArduinoSerial2->PortName = "COM3";
		//_ArduinoSerial2->BaudRate = 115200;
		_ArduinoSerial2->BaudRate = 9600;
		_ArduinoSerial2->DataBits = 8;
		_ArduinoSerial2->Parity = Parity::None;
		_ArduinoSerial2->StopBits = StopBits::One;
		_ArduinoSerial2->Handshake = Handshake::RequestToSend;
		_ArduinoSerial2->ReadTimeout = 500;
		_ArduinoSerial2->WriteTimeout = 500;

		_ArduinoSerial2->Open();

		double zTorque = 0.0;
		String^ message2;
		static string s32;
		std::string s33 = std::string("a");

		while (1)
		{
			
			if (s32.compare(s33))
			{
				zTorque = pnt->Max3();
				//cout << zTorque<<endl;
				if (zTorque>9)		//10500   //9500
				{
					_ArduinoSerial2->Write(String::Format("<{0}>: {1}", "m", "\n"));
				}

			}

			try
			{
				message2 = _ArduinoSerial2->ReadLine();
				MarshalString2(message2, s32);
			}
			catch (TimeoutException ^) {}


		}
		_ArduinoSerial2->Close();

	}
	break;
	}
}

//Arduino Reading Encoder
void ISR::Read()
{
	double Vel1 = 0;
	double Vel2 = 0;

	cout << " Arduino Started Reading Encoder " << endl;
	while (1)
	{   // Arduino
		try
		{
			String^ message = _ArduinoSerial->ReadLine();
			//  Console::WriteLine(message);
			pnt->Move(message);
		}
		catch (TimeoutException ^) {}
	}
}

void ISR::MarshalString(String ^ s, string& os)
{
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}
void ISR::MarshalString2(String ^ s, string& os)
{
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}
