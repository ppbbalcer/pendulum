#include <stdlib.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>



#define g 9.81
double alpha, accl, angle_velocity, E;
struct timeval tv;
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME (1000 / SCREEN_FPS)
#define INITIAL_ALPHA 80
int length = 3;
double anchor_x = 400;
double anchor_y = 0;
double pendulum_x = 400;
double pendulum_y = 0;
double tightness = 0.8;
double separation = 100.0;
double damping = 0.02;
double velocity = 0;
SDL_Renderer* ren;

void draw_circle(double x, double y, double r) {
	glPointSize(20);
	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0xFF, 0xFF);
	SDL_RenderDrawPoint(ren, (int)x, (int)y);
	glPointSize(1);
}

void draw_line(double x, double y, double x2, double y2) {
	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0xFF, 0xFF);
	SDL_RenderDrawLine(ren, (int)x, (int)y, (int)x2, (int)y2);
}

#define SPRING_OFFSET 20
#define SPRING_ELEMENTS 20
void draw_spring(double x, double y) {
	int i;
	double h = y / SPRING_ELEMENTS;
	double old_x = x;
	double old_y = 0;
	for (i = 0; i < SPRING_ELEMENTS; ++i) {
		float diff;
		if (i == 0 || i == SPRING_ELEMENTS - 1) {
			diff = (i % 2) ? (SPRING_OFFSET/2) : -1*(SPRING_OFFSET/2);
		} else {
			diff = (i % 2) ? (SPRING_OFFSET) : -1*(SPRING_OFFSET);
		}
		draw_line(old_x, old_y, old_x + diff, old_y + (h));
		old_x += diff;
		old_y += (h);
	}
}

void update_pendulum_position(double dt) {
	double distance = pendulum_y;
	double f = -1 * tightness * (distance - separation) - damping * velocity;

	velocity += f * dt;
	pendulum_y += velocity * dt;
}


void render(double dt) {
	double x = pendulum_x + length*100 * sin(alpha);
	double y = pendulum_y + length*100 * cos(alpha);
	draw_spring(pendulum_x, pendulum_y);
	draw_line(pendulum_x, pendulum_y, x, y);
	draw_circle(pendulum_x, pendulum_y, 10);
	draw_circle(x, y, 10);
	accl = (-g / length) * sin(alpha);
	angle_velocity += accl * dt;
	alpha += angle_velocity * dt;

	if (length * g * (1 - cos(alpha)) >= E) {
		alpha = (alpha < 0 ? -1 : 1) * acos(1 - E / length / g);
		angle_velocity = 0;
	}

	update_pendulum_position(dt);
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* displayWindow;
	//alpha = INITIAL_ALPHA * (M_PI/180);

	int dt_mul = atoi(argv[1]);
	alpha = atoi(argv[2]) * (M_PI/180);
	length = atoi(argv[3]);
	separation = atoi(argv[4]);
	tightness = atoi(argv[5])/10.0;
	damping = atoi(argv[6])/100.0;

	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL, &displayWindow, &ren);

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glPointSize(20);


	E = length * g * (1 - cos(alpha));
	accl = 0;
	angle_velocity = 0;
	int quit = 0;
	SDL_Event e;
	int deltaTimer = SDL_GetTicks();
	int running = 1;
	while (!quit) {
		int capTimer = SDL_GetTicks();
		while (SDL_PollEvent( &e ) != 0) {
			if (e.type == SDL_QUIT) {
				quit = 1;
			} else if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
					case SDLK_SPACE:
						running *= -1;
						break;
					case SDLK_LEFT:
						running = 2;
						break;
					case SDLK_RIGHT:
						running = 1;
						break;
				}
			}
		}
		SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(ren);
		double dt = 0;
		if (running > 0)
			dt = (SDL_GetTicks() - deltaTimer) / 1000.f;
		if (running == 2)
			dt *= -1;
		dt *= dt_mul;
		deltaTimer = SDL_GetTicks();
		render(dt);
		SDL_RenderPresent(ren);
		int frameTicks = SDL_GetTicks() - capTimer;
		if (frameTicks < SCREEN_TICKS_PER_FRAME)
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
	}
	SDL_Quit();

	return 0;
}
