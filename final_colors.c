#define CHAIN 1, 4, 6, 2, 5, 3

#include "kilolib.h"
#include "common_functions.h"
#include "stdlib.h"

int new_message = 0, message_sent = 0, delay_time = 300, pos = 0, mod_pos = 0;
int seed = 0, white_counter = 0, random_counter = 0, count_limit = 0;
int chain[6] = {CHAIN}, done[6] = {0, 0, 0, 0, 0, 0};
message_t transmit_msg;

void message_rx(message_t *msg, distance_measurement_t *dist) {
  pos = (msg->data[0]);
  mod_pos = pos;
  if (pos > 6) {
    mod_pos = (pos % 6) - 1;
  }
  done[chain[mod_pos] - 1] = 1;
  seed = msg->data[1];
  new_message = 1;
}

message_t *message_tx() {
  return &transmit_msg;
}

void message_tx_success() {
  message_sent = 1;
}

void color_function(int seed) {
    if (seed == 7) {
       set_color(RED);
    } else if (seed == 8) {
      set_color(YELLOW);
    } else if (seed == 2) {
       set_color(GREEN);
    } else if (seed == 3) {
       set_color(BLUE);
    } else if (seed == 4) {
       set_color(CYAN);
    } else {
       set_color(VIOLET);
    }
}

int blink(int count_limit) {
  int rand = rand_hard();
  if (kilo_uid == 1) {
    seed = (rand % 6);
  }

  while (random_counter < count_limit) {
      color_function(seed);
      delay(delay_time);

      set_color(OFF);
      delay(delay_time);

      random_counter += 1;
  }
  return seed;
}

void flash(int count_limit) {
  if (done[kilo_uid - 1] == 1) {
    delay(delay_time);
    seed = blink(count_limit);
    pos += 1;

    done[kilo_uid - 1] = 0;

    transmit_msg.type = NORMAL;
    transmit_msg.data[0] = pos;
    transmit_msg.data[1] = seed;
    transmit_msg.crc = message_crc(&transmit_msg);

    while (message_sent == 1) {
      message_sent = 0;
      set_color(OFF);
      //if (pos > 6) {
        break;
      //}
    }
}

void setup() {
  if (kilo_uid == 1) {
    done[kilo_uid - 1] = 1;
  }
}

void loop() {
  if (kilo_uid == 1) {
    while (white_counter < 5) {
      set_color(WHITE);
      delay(delay_time);
      set_color(OFF);
      delay(delay_time);

      white_counter += 1;
    }
  }

  if (kilo_uid == 1) {
     count_limit = 2;
  } else {
     count_limit = 5;
  }

  flash(count_limit);

  if (new_message == 1) {
    new_message = 0;
  }
}

int main() {
  kilo_init();
  kilo_message_rx = message_rx;
  kilo_message_tx = message_tx;
  kilo_message_tx_success = message_tx_success;
  kilo_start(setup, loop);
  return 0;
}
