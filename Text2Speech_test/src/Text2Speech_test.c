#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "mraa.h"

#include <unistd.h>

mraa_uart_context uart0;

void pr(mraa_result_t result){
	switch (result){
	case MRAA_SUCCESS:
		fprintf(stdout, "MRAA_SUCCESS\n");
		return;
	case MRAA_ERROR_INVALID_PARAMETER:
		fprintf(stdout, "MRAA_ERROR_INVALID_PARAMETER\n");
		return;
	case MRAA_ERROR_PLATFORM_NOT_INITIALISED:
		fprintf(stdout, "MRAA_ERROR_PLATFORM_NOT_INITIALISED\n");
		return;
	case MRAA_ERROR_PLATFORM_ALREADY_INITIALISED:
		fprintf(stdout, "MRAA_ERROR_PLATFORM_ALREADY_INITIALISED\n");
		return;
	default:
		fprintf(stdout, "MRAA ERROR: %d\n", (int)result);
		return;
	}
}

#define DEFAULT_VOICE	0
#define DEFAULT_VOLUME	0
#define DEFAULT_RATE	200


int EMIC_setup(void){
	uart0 = mraa_uart_init(0);
	if (uart0 == NULL){
		fprintf(stderr, "UART init failed!\n");
		return -1;
	}
	fprintf(stdout, "UART initialized!\n");

	pr(mraa_uart_set_baudrate(uart0, 9600));

	// 8N1
	pr(mraa_uart_set_mode(uart0, 8, MRAA_UART_PARITY_NONE, 1));

	char buffer[] = "R\r\n";
	mraa_uart_write(uart0, buffer, strlen(buffer));

	return 0;
}

int EMIC_speak(char* text){

	char buffer[1023];

	// max length allowed by module is 1023 characters
	if (strlen(text) > 1023)
		return -1;

	sprintf(buffer, "S%s\r\n", text);
	if (mraa_uart_write(uart0, buffer, strlen(buffer)) != strlen(buffer))
		return -1;

	return 0;
}

void EMIC_stop(void){
	char buffer[] = "X\r\n";
	mraa_uart_write(uart0, buffer, strlen(buffer));
}

void EMIC_playpause(void){
	char buffer[] = "Z\r\n";
	mraa_uart_write(uart0, buffer, strlen(buffer));
}

#define VOICE_PERFECT_PAUL		0
#define VOICE_HUGE_HARRY		1
#define VOICE_BEAUTIFUL_BETTY	2
#define VOICE_UPPITY_URSULA		3
#define VOICE_DOCTOR_DENNIS		4
#define VOICE_KIT_THE_KID		5
#define VOICE_FRAIL_FRANK		6
#define VOICE_ROUGH_RITA		7
#define VOICE_WHISPERING_WENDY	8

int EMIC_selectVoice(int voice){
	char buffer[10];

	if (voice < 0 || voice > 8)
		return -1;

	sprintf(buffer, "N%d\r\n", voice);
	mraa_uart_write(uart0, buffer, strlen(buffer));

	fprintf(stdout, "Voice # %d\n", voice);

	return 0;
}

#define VOLUME_MIN	-48
#define VOLUME_MAX	18
void EMIC_setVolume_dB(int volume){
	char buffer[10];

	if (volume < VOLUME_MIN)
		volume = VOLUME_MIN;

	if (volume > VOLUME_MAX)
		volume = VOLUME_MAX;

	sprintf(buffer, "V%d\r\n", volume);
	mraa_uart_write(uart0, buffer, strlen(buffer));

}

// speed in words/minute
#define SPEED_MIN	75
#define SPEED_MAX	600
void EMIC_setSpeed(int speed){
	char buffer[10];
	if (speed < SPEED_MIN)
		speed = SPEED_MIN;
	if (speed > SPEED_MAX)
		speed = SPEED_MAX;

	sprintf(buffer, "W%d\r\n", speed);
	mraa_uart_write(uart0, buffer, strlen(buffer));
}


int main() {
	char inputString[120];

	if (EMIC_setup())
		return EXIT_FAILURE;

	EMIC_setVolume_dB(3);

	int voice = 1;
	while (1){
		if (++voice == 9) voice = 0;
		EMIC_selectVoice(voice);

		sprintf(inputString, "I'm sorry Alex! I can't let you do that!");

		if (EMIC_speak(inputString)){
			return EXIT_FAILURE;
		}


		sleep(5);
	}

	pr(mraa_uart_stop(uart0));

	return 0;
}
