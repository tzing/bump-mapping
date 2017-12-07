#pragma once

void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void reshape(int width, int height);
void camera_light_ball_move();
void draw_light_bulb(void);
void idle(void);
