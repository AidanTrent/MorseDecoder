// Aidan Trent
// Takes timings in PCM frames and interprets them into international morse code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"
#include "codes.h"

#define MAX_GROUP_LEN 8

#define ALPHA_CODES_NUM 26
const char* codesAlpha[] = {".-",	// A
							"-...",	// B
							"-.-.",	// C
							"-..",	// D
							".",	// E
							"..-.",	// F
							"--.",	// G
							"....",	// H
							"..",	// I
							".---",	// J
							"-.-",	// K
							".-..",	// L
							"--",	// M
							"-.",	// N
							"---",	// O
							".--.",	// P
							"--.-",	// Q
							".-.",	// R
							"...",	// S
							"-",	// T
							"..-",	// U
							"...-",	// V
							".--",	// W
							"-..-",	// X
							"-.--",  // Y
							"--.."};// Z

#define NUM_CODES_NUM 10
const char* codesNum[] = {"-----",	// 0
						".----",	// 1
						"..---",	// 2
						"...--",	// 3
						"....-",	// 4
						".....",	// 5
						"-....",	// 6
						"--...",	// 7
						"---..",	// 8
						"----.",};	// 9

#define PUNCT_CODES_NUM 15
const char* codesPunct[][2] = {{".-.-.-", "."},
							{"--..--", ","},
							{"..--..", "?"},
							{"-..-.", "/"},
							{"-.--.", "("},
							{"-.--.-", ")"},
							{".-...", "&"},
							{"---...", ":"},
							{"-.-.-.", ";"},
							{"-...-", "="},
							{".-.-.", "+"},
							{"-....-", "-"},
							{"..--.-", "_"},
							{".-..-.", "\""},
							{".--.-.", "@"}};

char morseToAscii(char* morse){
	for (int i = 0; i < ALPHA_CODES_NUM; i++){
		if (strcmp(morse, codesAlpha[i]) == 0){
			return(i + 'A');
		}
	}
	for (int i = 0; i < NUM_CODES_NUM; i++){
		if (strcmp(morse, codesNum[i]) == 0){
			return(i + '0');
		}
	}
	for (int i = 0; i < PUNCT_CODES_NUM; i++){
		if (strcmp(morse, codesPunct[i][0]) == 0){
			return(codesPunct[i][1][0]);
		}
	}
	return(0);
}

int decode(LList* timings){
	if (timings == NULL){
		fprintf(stderr, "decode: No timings found");
		return(1);
	}

	// Get averages
	Node* cur = timings->head;
	uint64_t avgLength = 0; // Average length of depression
	uint64_t lastR = 0; // Frame of last release
	uint64_t avgGap = 0; // Average frames between each timing
	for (int i = 1; cur != NULL; i++, cur = cur->next){
		if (lastR != 0){
			avgGap = ((avgGap * (i - 1)) + (cur->ent.frameD - lastR)) / i;
		}
		avgLength = ((avgLength * (i - 1)) + (cur->ent.frameR - cur->ent.frameD)) / i;
		lastR = cur->ent.frameR;
	}

	// Get dot, dot, dash, and gap
	char c;
	char group[MAX_GROUP_LEN];
	int groupI = 0; // Current index of group
	lastR = 0; // Frame of last release
	uint64_t gap = 0; // Gap between last release and cur depression
	cur = timings->head;
	while (cur != NULL){
		if (lastR != 0){
			gap = cur->ent.frameD - lastR;
			// Character complete
			if (gap > avgGap){
				group[groupI] = '\0';
				c = morseToAscii(group);
				if (c != 0){
					printf("%c", c);
				}

				groupI = 0;
			}
			// Between words TODO: Magic number
			if (gap > avgGap * 2.5){
				printf(" ");
			}
		}
		// On dash
		if (cur->ent.frameR - cur->ent.frameD >= avgLength){
			group[groupI] = '-';
			//printf("-");
		}
		// On dot
		else{
			group[groupI] = '.';
			//printf(".");
		}
		groupI++;
		if (groupI >= MAX_GROUP_LEN){
			fprintf(stderr, "decode: Possibly missing a gap\n");
			groupI = 0;
		}
		lastR = cur->ent.frameR;
		cur = cur->next;
	}
	// Get last group
	c = morseToAscii(group);
	if (c != 0){
		printf("%c", c);
	}
	printf("\n");
	return(0);
}
