
#include <avr/io.h>
#include "slave.h"

typedef enum {
	CMDSTATE_READ_CMD_BYTE,
	CMDSTATE_READ_PARAM_BYTE,
	CMDSTATE_WRITE_RETURN_BYTE
} cmd_state_t;

static volatile cmd_state_t cmd_state = CMDSTATE_READ_CMD_BYTE;
static volatile int8_t      cmd_cnt   = 0;
extern volatile uint8_t     cmd_buffer[7];
extern volatile int8_t      cmd_args;
extern volatile int8_t      cmd_rets;
extern void               (*cmd_function)();
extern inline void          cmd_lookup(uint8_t cmd);

void usitwi_onStart(uint8_t read) {
	if (!read) {
		cmd_state = CMDSTATE_READ_CMD_BYTE;
	}
}

void usitwi_onStop() {
	cmd_state = CMDSTATE_READ_CMD_BYTE;
}

uint8_t usitwi_onRead() {
	if (cmd_state == CMDSTATE_WRITE_RETURN_BYTE) {
		cmd_cnt++;
		if (cmd_cnt > cmd_rets) {
			cmd_state = CMDSTATE_READ_CMD_BYTE;
		}
		return cmd_buffer[cmd_cnt-1];
	} else {
		return 0xff;
	}
}

inline static void cmd_run_function() {
	cmd_function();
	if (cmd_rets < 0) {
		cmd_state = CMDSTATE_READ_CMD_BYTE;
	} else {
		cmd_state = CMDSTATE_WRITE_RETURN_BYTE;
		cmd_cnt   = 0;
	}
}

void usitwi_onWrite(uint8_t value) {
	if (cmd_state == CMDSTATE_READ_CMD_BYTE) {
		cmd_lookup(value);
		if (cmd_function != 0) {
			if (cmd_args == -1) {
				cmd_run_function();
			} else {
				cmd_state = CMDSTATE_READ_PARAM_BYTE;
				cmd_cnt   = 0;
			}
		}
	} else if (cmd_state == CMDSTATE_READ_PARAM_BYTE) {
		cmd_buffer[cmd_cnt] = value;
		cmd_cnt++;
		if (cmd_cnt > cmd_args) {
			cmd_run_function();
		}
	}
}

