// ParticleEngine.cpp : Defines the entry point for the console application.
//

//#define ZLIB_WINAPI

#include <Windows.h>
#include <Flick\GroupClass.h>
#include <random>
#include <iostream>

#include "DirectXDeviceClass.h"
#include "WindowClass.h"
#include "DefinitionsClass.h"
#include "CameraClass.h"
#include "Timer.h"

#include "FlexibleIndexClass.h"
#include "ParticleClass.h"
#include "ParticleshaderClass.h"

#include "LogClass.h"

//////////////////
// System Class //
//////////////////
class SystemClass
{
public:

	SystemClass(){};
	SystemClass(const SystemClass&){};
	~SystemClass(){};

	// The frame function
	void Frame(void* data);

	// The render function
	void Render(void* data);
};

/////////////
// Globals //
/////////////

DefinitionsClass Definitions;
WindowClass Window;
DirectXDeviceClass directX;
CameraClass camera;
Timer timer;

group<ParticleClass> Particles;

FlexibleIndexClass indexes;
ParticleShaderClass Particleshader;
ParticleInstance* particleInstance;

// Data
float factor = 5;
bool mouseDown = false;
int multiplyValue = 1;

float quadLength = 0.5;
bool limit = true;
bool gravity = false;
bool resistence = true;
int area = 300;
float resistenceForce = 0.7f;
float resistenceRealValue = 0;

using namespace std;

// Function prototypes
void Initialization();
void PromptData();

void cls()
{
   COORD coordScreen = { 0, 0 };    // home for the cursor 
   DWORD cCharsWritten;
   CONSOLE_SCREEN_BUFFER_INFO csbi; 
   DWORD dwConSize;
    HANDLE hConsole;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Get the number of character cells in the current buffer. 

   if( !GetConsoleScreenBufferInfo( hConsole, &csbi ))
      return;
   dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

   // Fill the entire screen with blanks.

   if( !FillConsoleOutputCharacter( hConsole, (TCHAR) ' ',
      dwConSize, coordScreen, &cCharsWritten ))
      return;

   // Get the current text attribute.

   if( !GetConsoleScreenBufferInfo( hConsole, &csbi ))
      return;

   // Set the buffer's attributes accordingly.

   if( !FillConsoleOutputAttribute( hConsole, csbi.wAttributes,
      dwConSize, coordScreen, &cCharsWritten ))
      return;

   // Put the cursor at its home coordinates.

   SetConsoleCursorPosition( hConsole, coordScreen );
}

int main()
{
	bool result;

	// Call the initialization function
	Initialization();

	// Erase the debug log
	DebugLog.Erase();

	// Alocate space for all particles
	particleInstance = new ParticleInstance[TotalParticles];
	if(!particleInstance)
	{
		return false;
	}

	// Log msg
	DebugLog << "Inicializando o sistema";

	// Initialize the Definitions
	result = Definitions.Initialize();
	if(!result)
	{
		return false;
	}

	// Log msg
	DebugLog << "Inicializando a janela";	

	// Initialize the Window
	result = Window.Initialize();
	if(!result)
	{
		return false;
	}

	// Log msg
	DebugLog << "Inicializando o directX 11";

	// Intialize the directX
	result = directX.Initialize();
	if(!result)
	{
		return false;
	}

	// Initialize the camera
	camera.Initialize(1.0f, 100000.0f);

	// Start the timer
	timer.startTimer();

	///////////////
	// Particles //
	///////////////

	// Log msg
	DebugLog << "Carregando particulas";

	unsigned i, square;
	float x, y;

	// Get the square from the total number of Particles
	square = sqrt(TotalParticles);

	// Set the expand size for the particle array
	Particles.SetExpandSize(TotalParticles);

	float rndX, rndY, rndZ, length;
	std::tr1::mt19937 eng;
	std::tr1::uniform_real_distribution<float> dist(-100, 100);

	for(int i = 0; i < TotalParticles; ++i)
	{
		ParticleClass newParticle;

		rndX = dist(eng);
		rndY = dist(eng);
		rndZ = dist(eng);

		// Set the particle position
		newParticle.SetPosition(D3DXVECTOR3(rndX, rndY, rndZ));

		// Add the new particle to the array
		Particles.Add(newParticle);

		// Put the data into the array
		particleInstance[i].position.x = Particles[i].GetPosition().x;
		particleInstance[i].position.y = Particles[i].GetPosition().y;
		particleInstance[i].position.z = Particles[i].GetPosition().z;
		particleInstance[i].position.w = 1;
		particleInstance[i].lastPosition.x = Particles[i].GetPosition().x;
		particleInstance[i].lastPosition.y = Particles[i].GetPosition().y;
		particleInstance[i].lastPosition.z = Particles[i].GetPosition().z;
		particleInstance[i].acceleration.x = Particles[i].GetAcceleration().x;
		particleInstance[i].acceleration.y = Particles[i].GetAcceleration().y;
		particleInstance[i].acceleration.z = Particles[i].GetAcceleration().z;
		particleInstance[i].acceleration.w = 0;
	}

	// Add the index
	i = 0;
	indexes.Add(&i);
	
	/////////////////////
	// Particle Shader //
	/////////////////////

	// Log msg
	DebugLog << "Iniciando o shader";

	// Initialize the particle shader
	result = Particleshader.Initialize();
	if(!result)
	{
		return false;
	}

	// Create the instance buffer
	result = Particleshader.CreateInstanceBuffer(DirectX->GetDevice(), particleInstance, TotalParticles);
	if(!result)
	{
		return false;
	}

	////////////////
	// Frame Call //
	////////////////

	SystemClass system;
	CallbackClass callbackFunction;

	// Create the frame callback
	callbackFunction.Set(system, &SystemClass::Frame);

	// Log msg
	DebugLog << "Preparando para entrar no loop principal";

	// Set the callback function at the Window class
	Window.Run(callbackFunction);

	return 0;
}

void SystemClass::Frame(void* data)
{
	int i;
	D3DXVECTOR3 attractor;
	POINT p;
	float x, y;

	// Set the camera position
	camera.SetPosition(0, 0, -500); // 400, -500

	// Set the camera rotation
	camera.SetRotation(0, 0, 0); // 65

	// Get the cursor position
	GetCursorPos(&p);

	// Compute the correct valuer
	p.x -= 800;
	p.y -= 450;
	p.y *= -1;
	x = (float)p.x / 2.2f;
	y = (float)p.y / 2.2f;

	// Zero i
	i = 0;

	// Check the messages
	while(SystemWindow->messages.Size())
	{
		// Switch the message
		switch(SystemWindow->messages[i].message)
		{
			// Mouse down
			case LEFT_MOUSE_DOWN:
			{
				// Set mouse down to true
				mouseDown = true;

				break;
			}

			// Mouse up
			case LEFT_MOUSE_UP:
			{
				// Set mouse down to true
				mouseDown = false;

				break;
			}

			// Mouse down
			case RIGHT_MOUSE_DOWN:
			{
				// Set mouse down to true
				multiplyValue = -1;
				mouseDown = true;
				break;
			}

			// Mouse up
			case RIGHT_MOUSE_UP:
			{
				// Set mouse down to true
				multiplyValue = 1;
				mouseDown = false;

				break;
			}

			// Key press
			case KEY_PRESS:
			{
				WindowMessage te = SystemWindow->messages[i];
				switch(SystemWindow->messages[i].longParam)
				{
					case 71: // g
					{
						gravity = !gravity;
						break;
					}

					case 76: // l
					{
						limit = !limit;
						break;
					}	

					case 82: // r
					{
						resistence = !resistence;
						break;
					}

					case 107: // +
					{
						area = area + 10;
						break;
					}

					case 109: // -
					{
						if(area > 10) 
						{
							area = area - 10;	
						}

						break;
					}

					case 190: // >
					{
						resistenceForce = resistenceForce + 0.05f;
						break;
					}

					case 188: // <
					{
						if(resistenceForce > 0.05f) 
						{
							resistenceForce = resistenceForce - 0.05f;	
						}

						break;
					}				
				}
			}
		}		

		// Remove this message
		SystemWindow->messages.Remove(i);

		// Increment i
		i++;
	}

	// Subtract the resistence value
	if(resistence && resistenceRealValue > 0.01f)
	{
		resistenceRealValue -= 0.01f;
	}
	else
	{
		resistenceRealValue = 0;
	}

	// Reset the resistence value
	if(mouseDown)
	{
		resistenceRealValue = resistenceForce;
	}

	// Prompt the current data
	PromptData();

	// Call the render function
	Render(data);
}

void SystemClass::Render(void* data)
{
	D3DXVECTOR3 attractor;
	POINT p;
	float x, y;

	// Get the cursor position
	GetCursorPos(&p);

	// Compute the correct valuer
	p.x -= 800;
	p.y -= 450;
	p.y *= -1;
	x = (float)p.x / 2.2f;
	y = (float)p.y / 2.2f;

	// Render the camera
	camera.Render();

	// Put the index buffer at the render pipeline
	indexes.SetBuffers(DirectX->GetDeviceContext());

	// Set the render target
	DirectX->SetBackBufferRenderTarget();

	// Clear the buffers to begin the scene.
	DirectX->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	DirectX->TurnZBufferOff();

	// Turn on the alpha blending
	DirectX->TurnOnAlphaBlending();

	///////////////////
	// DX END CONFIG //
	///////////////////

	// Render the Particles
	Particleshader.Render(StatusData(timer.getTime(), timer.getFrameTime(), quadLength, D3DXVECTOR3(x, y, 0)), 
	SessionData(!mouseDown, limit, gravity, resistence, multiplyValue, resistenceRealValue, area), particleInstance, TotalParticles, indexes.GetIndexSize());

	///////////////
	// DX CONFIG //
	///////////////

	// Turn off the alpha blending
	DirectX->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	DirectX->TurnZBufferOn();
	
	// Present the rendered scene to the screen.
	DirectX->EndScene();
}

void Initialization()
{
	int temp = 0;

	while(temp < 1024 || temp > 10000000)
	{
		// Ask for the number of particles
		cout << "Digite o numero de particulas (min 1024): ";
		cin >> temp;

		SetTotalParticles(temp);
		TotalParticles = temp;

		// Clear the console
		cls();

		// Check if we are ok
		if(temp < 1024)
		{
			cout << "Valor invalido!" << endl;
		}
	}

	// Clear the console
	cls();
}

void PromptData()
{
	// Clear the console
	cls();

	// Mouse down
	cout << "Mouse state: ";
	if(mouseDown)
	{
		cout << "DOWN" << endl;
	}
	else
	{
		cout << "UP" << endl;
	}

	// Quad length
	cout << "Quad length: " << quadLength << endl;

	// Jump one line
	cout << endl;

	// Gravity
	cout << "Gravity: " << gravity << endl;

	// Resistence
	cout << "Resistence: " << resistence << "			Force: " << resistenceForce << endl;

	// Area limitation
	cout << "Area limitation: " << limit << "		Size: " << area << endl;
}