#include <iostream>
#include <array>
#include <fstream>
#include <bitset>
#include <time.h>
#include "SDL.h"

#define fps 60

using namespace std;

int main(int argc, char *argv[]) {

	string filename;
	cout << "Enter file name..." << endl;
	cin >> filename;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cout << "SDL INIT FAIL" << endl;
	}

	SDL_Window* window;

	window = SDL_CreateWindow("Kano - Chip 8 Emulator",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1280, 640,
		SDL_WINDOW_RESIZABLE);
	
	SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (render == nullptr) {
		std::cout << "SDL_CreateRenderer Error" << endl;
		return 1;
	}

	SDL_Event event;
	bool running = true;
	Uint32 starting_tick;
	

	if (window == NULL) {
		cout << "Error initializing window" << endl;
	}

	int i;
	int winWidth = 64;
	int winHeight = 32;
	unsigned short opcode;
	unsigned short popcode = 123;
	unsigned char memory[4096];
	unsigned char V[16];
	unsigned short I = 0;
	unsigned short pc = 0x200;
	unsigned char gfx[64 * 32];
	unsigned char delay_timer = 0;
	unsigned char sound_timer = 0;
	unsigned short stack[16];
	unsigned short sp = 0;
	unsigned char key[16];
	unsigned short x;
	unsigned short y;
	unsigned short height;
	unsigned short pixel = 0;
	bool keyPress;
	char sprite = 0;
	char temp;
	bool drawFlag;

	for (i = 0; i < 16; i++) {
		key[i] = 0;
	}

	unsigned char chip8_fontset[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, //0
		0x20, 0x60, 0x20, 0x20, 0x70, //1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
		0x90, 0x90, 0xF0, 0x10, 0x10, //4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
		0xF0, 0x10, 0x20, 0x40, 0x40, //7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
		0xF0, 0x90, 0xF0, 0x90, 0x90, //A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
		0xF0, 0x80, 0x80, 0x80, 0xF0, //C
		0xE0, 0x90, 0x90, 0x90, 0xE0, //D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
		0xF0, 0x80, 0xF0, 0x80, 0x80  //F
	};

	pc = 0x200; //Program counter starts at 0x200
	opcode = 0; //Reset current opcode
	I = 0;		//Reset index register
	sp = 0;		//Reset stack pointer

	for (i = 0; i < 2048; i++) {//Clear display
		gfx[i] = 0;
	}
	for (i = 0; i < 16; i++) {		//Clear stack
		stack[i] = 0;
	}

	for (i = 0; i < 16; i++) {		//Clear keys
		key[i] = 0;
	}

	for (i = 0; i < 16; i++) {		//Clear registers V0-VF
		V[i] = 0;
	}
	for (i = 0; i < 4096; i++) {	//Clear memory
		memory[i] = 0;
	}
	for (i = 0; i < 80; i++) {	//Load fontset into memory
		memory[i] = chip8_fontset[i];
	}

	srand((unsigned int)time(NULL));


	FILE * pFile;
	pFile = fopen(filename.c_str (), "rb");

	if (pFile == NULL)
	{
		cout << filename.c_str() << endl;
		fputs("File error, file doesn't exist?", stderr);
		cin.get();
		return false;
	}

	// Check file size
	fseek(pFile, 0, SEEK_END);
	long lSize = ftell(pFile);
	rewind(pFile);
	//printf("Filesize: %d\n", (int)lSize);

	// Allocate memory to contain the whole file
	char * buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL)
	{
		fputs("Memory error", stderr);
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread(buffer, 1, lSize, pFile);
	if (result != lSize)
	{
		fputs("Reading error", stderr);
		return false;
	}

	// Copy buffer to Chip8 memory
	if ((4096 - 512) > lSize)
	{
		for (int i = 0; i < lSize; ++i)
			memory[i + 512] = buffer[i];
	}
	else
		printf("Error: ROM too big for memory");

	// Close file, free buffer
	fclose(pFile);
	free(buffer);

	//***EMULATION CYCLE***
	//***EMULATION CYCLE***
	//***EMULATION CYCLE***

	cout << "-KANO Error Menu-" << endl;
	cout << "Any errors will be displayed here" << endl;

	drawFlag = true;

	while (running) {

		drawFlag = false;
		starting_tick = SDL_GetTicks();

		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_1:
					key[1] = 1;
					break;
				case SDLK_2:
					key[2] = 1;
					break;
				case SDLK_3:
					key[3] = 1;
					break;
				case SDLK_4:
					key[0xC] = 1;
					break;
				case SDLK_q:
					key[4] = 1;
					break;
				case SDLK_w:
					key[5] = 1;
					break;
				case SDLK_e:
					key[6] = 1;
					break;
				case SDLK_r:
					key[0xD] = 1;
					break;
				case SDLK_a:
					key[7] = 1;
					break;
				case SDLK_s:
					key[8] = 1;
					break;
				case SDLK_d:
					key[9] = 1;
					break;
				case SDLK_f:
					key[0xE] = 1;
					break;
				case SDLK_z:
					key[0xA] = 1;
					break;
				case SDLK_x:
					key[0] = 1;
					break;
				case SDLK_c:
					key[0xB] = 1;
					break;
				case SDLK_v:
					key[0xF] = 1;
					break;
				}
			}
			if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym) {
				case SDLK_1:
					key[1] = 0;
					break;
				case SDLK_2:
					key[2] = 0;
					break;
				case SDLK_3:
					key[3] = 0;
					break;
				case SDLK_4:
					key[0xC] = 0;
					break;
				case SDLK_q:
					key[4] = 0;
					break;
				case SDLK_w:
					key[5] = 0;
					break;
				case SDLK_e:
					key[6] = 0;
					break;
				case SDLK_r:
					key[0xD] = 0;
					break;
				case SDLK_a:
					key[7] = 0;
					break;
				case SDLK_s:
					key[8] = 0;
					break;
				case SDLK_d:
					key[9] = 0;
					break;
				case SDLK_f:
					key[0xE] = 0;
					break;
				case SDLK_z:
					key[0xA] = 0;
					break;
				case SDLK_x:
					key[0] = 0;
					break;
				case SDLK_c:
					key[0xB] = 0;
					break;
				case SDLK_v:
					key[0xF] = 0;
					break;
				}
			}
		}
		opcode = memory[pc] << 8 | memory[pc + 1];
		
		//cout << hex << opcode << endl;

		switch (opcode & 0xF000)
		{
		case 0x0000:
			switch (opcode&0x00FF)
			{
			case 0x00E0:
				for (i = 0; i < 2048; i++) {
					gfx[i] = 0;
				}
				drawFlag = true;
				pc += 2;
				break;
			case 0x00EE:
				sp -= 1;
				pc = stack[sp];
				pc += 2;

				break;
			default:
				cout << "RCA Program currently not supported" << endl;
				cin.get();
			}
			break;
		case 0x1000:
			pc = opcode & 0x0FFF;
			break;
		case 0x2000:
			stack[sp] = pc;
			sp += 1;
			pc = opcode & 0x0FFF;
			
			break;
		case 0x3000:
			if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
			break;
		case 0x4000:
			if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
				pc += 4;
			}
			else { pc += 2; }
			break;
		case 0x5000:
			if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
				pc += 4;
			}
			else { pc += 2; }
			break;
		case 0x6000:
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			pc += 2;
			break;
		case 0x7000:
			V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			pc += 2;
			break;

		case 0x8000:
			switch (opcode & 0x000F)
			{
			case 0x0000:
				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0001:
				V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0002:
				V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0003:
				V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0004:
				if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
					V[0xF] = 1;
				}
				else {
					V[0xF] = 0;
				}
				V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0005:
				if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])  {
					V[0xF] = 0;
				}
				else { V[0xF] = 1; }
				V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0006:
				V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x1);
				V[(opcode & 0x0F00) >> 8] >>= 1;
				pc += 2;
				break;
			case 0x0007:
				if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
					V[0xF] = 0;
				}
				else {
					V[0xF] = 1;
				}
				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
				pc += 2;
				break;
			case 0x000E:
				V[0xF] = (V[(opcode & 0x0F00) >> 8] >> 7);
				V[(opcode & 0x0F00) >> 8] <<= 1;
				pc += 2;
				break;

			default:
				cout << "b" << endl;
				cout << "Unknown opcode:    " << hex << opcode << endl;
				cout << "Previous opcode:    " << hex << popcode << endl;
			}
			break;
		case 0x9000:
			if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;
		case 0xA000:
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		case 0xB000:
			pc = V[0] + (opcode & 0x0FFF);
			break;
		case 0xC000:
			cout << "Generating random number..." << endl;
			V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
			pc += 2;
			break;
		case 0xD000:  //Draw Sprite On Screen
			x = V[(opcode & 0x0F00) >> 8];
			y = V[(opcode & 0x00F0) >> 4];
			height = opcode & 0x000F;
			V[0xF] = 0;
			
			

			for (int yline = 0; yline < height; yline++)
			{
				sprite = memory[I + yline];
				for (int xline = 0; xline < 8; xline++)
				{
					if ((sprite & (0x80 >> xline)) != 0)
					{
						if (gfx[(x + xline + ((y + yline) * 64))] == 1)
						{
							V[0xF] = 1;
						}
						gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}

			drawFlag = true;
			pc += 2;
			break;
		case 0xE000:
			switch (opcode & 0x00FF)
			{
			case 0x009E:
				if(key[V[(opcode & 0x0F00) >> 8]] != 0)
						pc += 4;
				else
					pc += 2;
				break;
			case 0x00A1:
				if (key[V[(opcode & 0x0F00) >> 8]] == 0)
					pc += 4;
				else
					pc += 2;
				break;
			default:
				cout << "c" << endl;
				cout << "Unknown opcode:    " << hex << opcode << endl;
				cout << "Previous opcode:    " << hex << popcode << endl;
			}
			break;
		case 0xF000:
			switch (opcode & 0x00FF) {
			case 0x0007:
				V[(opcode & 0x0F00) >> 8] = delay_timer;
				pc += 2;
				break;
			case 0x000A:
				keyPress = false;

				for (int i = 0; i < 16; ++i)
				{
					if (key[i] != 0)
					{
						V[(opcode & 0x0F00) >> 8] = i;
						keyPress = true;
					}
				}

				if (!keyPress) {
					break;
				}

				pc += 2;
				break;
			case 0x0015:
				delay_timer = V[(opcode & 0x0F00)>>8];
				pc += 2;
				break;
			case 0x0018:
				sound_timer = V[(opcode & 0x0F00)>>8];
				pc += 2;
				break;
			case 0x001E:
				if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF) {
					V[0xF] = 1;
				}
				else {
					V[0xF] = 0;
				}
				I += V[(opcode & 0x0F00) >> 8];
				pc += 2;
				break;
			case 0x0029:
				I = V[(opcode & 0x0F00) >> 8] * 0x5;
				pc += 2;
				break;
			case 0x0033:
				memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
				memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
				memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
				pc += 2;
				break;
			case 0x0055:
				for (i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
					memory[I+i] = V[i];
				}

				I += ((opcode & 0x0F00) >> 8) + 1;

				pc += 2;
				break;
			case 0x0065:
				for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
					V[i] = memory[I + i];
				//I += ((opcode & 0x0F00) >> 8) + 1;
				pc += 2;
				break;
			default:
				cout << "d" << endl;
				cout << "Unknown opcode:    " << hex << opcode << endl;
				cout << "Previous opcode:    " << hex << popcode << endl;
				break;
			}
			break;
		default:
			std::cout << "Unkown opcode:" << endl;
			std::cout << hex << opcode << endl;

		}
		if (delay_timer > 0) {
			delay_timer -= 1;
		}
		if (sound_timer > 0) {
			if (sound_timer == 1) {
				std::cout << "BEEP!" << endl;
			}
			sound_timer -= 1;
		}
		
		if (drawFlag == true){
			for (x = 0; x < 64; x++) {
				for (y = 0; y < 32; y++) {
					if (gfx[x + (64 * y)] == 1) {
						SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);
						SDL_RenderDrawPoint(render, x, y);

					}
					if (gfx[x + (64 * y)] == 0) {
						SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
						SDL_RenderDrawPoint(render, x, y);
					}
				}
			}
			SDL_RenderSetScale(render, 20, 20);
			SDL_RenderPresent(render);
		}

		if ((1000 / fps) > SDL_GetTicks() - starting_tick) {
			//SDL_Delay(1000 / fps - (SDL_GetTicks() - starting_tick));
		}

		popcode = opcode;
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}