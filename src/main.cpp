#include "mbed.h"

enum State
{
  UP,
  DOWN,
  RISE,
  FALL,
  MAX,
  MIN
};
State state = UP;

InterruptIn button(p5);
DigitalOut blue_led(p7);
DigitalOut orange_led(p6);
PwmOut light(p8);

Timeout timeout_led;
Timeout timeout_pressing;
Timeout timeout_increase_light;
Timeout timeout_decrease_light;

float time_press = 0;
bool press_flag = false;

void to_up();
void to_down();
void to_rise();
void to_fall();
void to_max();
void to_min();

void start_press();
void finish_press();

void clear_all();

int main()
{
  to_up();
  light = 0;
  button.rise(&start_press);
  button.fall(&finish_press);
}

void pressing()
{
  time_press += 0.1;

  if (light < 0)
    light = 0;
  else if (light > 1)
    light = 1;

  if (time_press >= 1)
    switch (state)
    {
    case UP: // tBt >= 1s && i < 1.0
      to_rise();
      break;
    case DOWN: // tBt >= 1s && i > 0.0
      to_fall();
      break;
    case RISE: // Bt = 1 && i = 1.0
      if (light >= 1)
        to_max();
      break;
    case FALL: // Bt = 1 && i = 0.0
      if (light <= 0)
        to_min();
      break;
    default:
      printf("pressing state exception\n");
      break;
    }
  if (press_flag)
    timeout_pressing.attach(&pressing, 0.1);
}
void start_press()
{
  printf("start_press\n");
  time_press = 0;
  press_flag = true;
  timeout_pressing.attach(&pressing, 0.1);
}

void finish_press()
{
  printf("finish_press\n");
  press_flag = false;
  timeout_pressing.detach();
  switch (state)
  {
  case UP: // tBt < 1s
    to_down();
    break;
  case DOWN: // tBt < 1s
    to_up();
    break;
  case RISE: // Bt release && i < 1.0
    to_up();
    break;
  case FALL: // Bt release && i > 0.0
    to_down();
    break;
  case MAX: // Bt release
    to_down();
    break;
  case MIN: // Bt release
    to_up();
    break;
  default:
    printf("finish_press state exception\n");
    break;
  }
}

void to_up()
{
  state = UP;
  printf("state UP\n");

  clear_all();
  blue_led = 1;
  timeout_increase_light.detach();
}

void to_down()
{
  state = DOWN;
  printf("state DOWN\n");

  clear_all();
  orange_led = 1;
  timeout_decrease_light.detach();
}

void increase_light()
{
  if (light < 1)
    light = light + 0.05;
  timeout_increase_light.attach(&increase_light, 1);
}

void blink_blue_led_02s_p_s()
{
  blue_led = 1;
  wait(0.2);
  blue_led = 0;
  if (state == RISE)
    timeout_led.attach(&blink_blue_led_02s_p_s, 0.8);
}

void to_rise()
{
  state = RISE;
  printf("state RISE\n");

  blink_blue_led_02s_p_s();
  timeout_increase_light.attach(&increase_light, 1);
}

void decrease_light()
{
  if (light > 0)
    light = light - 0.05;
  timeout_decrease_light.attach(&decrease_light, 1);
}

void blink_orange_led_02s_p_s()
{
  orange_led = 1;
  wait(0.2);
  orange_led = 0;
  if (state == FALL)
    timeout_led.attach(&blink_orange_led_02s_p_s, 0.8);
}

void to_fall()
{
  state = FALL;
  printf("state FALL\n");

  blink_orange_led_02s_p_s();
  timeout_decrease_light.attach(&decrease_light, 1);
}

void blink_blue_led_5Hz()
{
  blue_led = 1;
  wait(0.1);
  blue_led = 0;
  if (state == MAX)
    timeout_led.attach(&blink_blue_led_5Hz, 0.1);
}

void to_max()
{
  state = MAX;
  printf("state MAX\n");

  clear_all();
  blink_blue_led_5Hz();
  timeout_increase_light.detach();
}

void blink_orange_led_5Hz()
{
  orange_led = 1;
  wait(0.1);
  orange_led = 0;
  if (state == MIN)
    timeout_led.attach(&blink_orange_led_5Hz, 0.1);
}

void to_min()
{
  state = MIN;
  printf("state MIN\n");

  clear_all();
  blink_orange_led_5Hz();
  timeout_decrease_light.detach();
}

void clear_all()
{
  blue_led = 0;
  orange_led = 0;
  timeout_led.detach();
}
