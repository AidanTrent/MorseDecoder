// Aidan Trent
// Decodes international morse from a audio file
// Currently uses differences from average volume to detect signals. Will
// eventually implement a better algorithim using FFT

#include <stdio.h>
#include <inttypes.h>
#include "linkedList.h"
#include "codes.h"

#define STB_VORBIS_HEADER_ONLY
#include "miniaudio/stb_vorbis.c"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#define RATE 48000

// Gets average abs value for each PCM frame in a ma_decoder
// Returns non-negative value on success
float getPCMAvg(ma_decoder* decoder){
	ma_result result;
	ma_uint64 totalFramesRead = 0;
	ma_uint64 framesRead = RATE;
	float frames[RATE];
	float sampleSum; // Sum of all abs frames in current sample
	float avg;

	ma_data_source_seek_to_pcm_frame(decoder, 0); // Reset read pos
	while (framesRead == RATE){
		result = ma_data_source_read_pcm_frames(decoder, frames, RATE, &framesRead);
		totalFramesRead += framesRead;
		if (result != MA_SUCCESS) {
			perror("ma_data_source_read_pcm_frames");
			return(-1);
		}

		sampleSum = 0;
		for (int i = 0; i < framesRead; i++){
			sampleSum += fabs(frames[i]);
		}
		avg = ((avg * (totalFramesRead - framesRead)) + sampleSum) / totalFramesRead;
	}
	ma_data_source_seek_to_pcm_frame(decoder, 0); // Reset read pos

	return(avg);
}

int main(int argc, char* argv[]){
	if (argc != 2){
		fprintf(stderr, "USAGE: morse FILENAME\n");
		return(EXIT_FAILURE);
	}

	// Init sound
	ma_result result;
	ma_engine engine;
	result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS) {
		perror("ma_engine_init");
		return(EXIT_FAILURE);
	}

	// Load sound file
	ma_decoder decoder;
	ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, RATE);
	result = ma_decoder_init_file(argv[1], &config, &decoder);
	if (result != MA_SUCCESS) {
		perror("ma_decoder_init_file");
		return(EXIT_FAILURE);
	}

	// Calculations
	LList* timings = NULL;
	Entity instanceTime = {0,0};

	float avg = getPCMAvg(&decoder);
	if (avg < 0){
		return(EXIT_FAILURE);
	}

	int diverge = 0; // Toggles on/off when over/under avg
	float sampleAvg = 0;
	ma_uint64 totalFramesRead = 0;
	ma_uint64 framesRead = RATE / 100;
	float frames[RATE];
	while (framesRead == RATE / 100){
		result = ma_data_source_read_pcm_frames(&decoder, frames, RATE / 100, &framesRead);
		totalFramesRead += framesRead;
		if (result != MA_SUCCESS) {
			perror("ma_data_source_read_pcm_frames");
			return(EXIT_FAILURE);
		}

		for (int i = 0; i < framesRead; i++){
			sampleAvg = ((sampleAvg * (i)) + fabs(frames[i])) / (i + 1); // Can probably replace with faster avg
		}
		if (sampleAvg > avg){
			//printf("-");
			if (!diverge){
				instanceTime.frameD = decoder.readPointerInPCMFrames;
				diverge = 1;
			}
		}
		else{
			//printf(".");
			if (diverge){
				instanceTime.frameR = decoder.readPointerInPCMFrames;
				if (timings == NULL){
					timings = makeList(&instanceTime);
				}
				else{
					insertTail(timings, &instanceTime);
				}
				diverge = 0;
			}
		}
	}
	printf("\n");
	ma_data_source_seek_to_pcm_frame(&decoder, 0); // Reset pos

	decode(timings);

	// Play sound file TODO: Make optional
	ma_sound snd;
	result = ma_sound_init_from_data_source(&engine, &decoder, 0, NULL, &snd);
	if (result != MA_SUCCESS) {
		perror("ma_sound_init_from_data_source");
		return(EXIT_FAILURE);
	}
	ma_sound_start(&snd);

	// Exit
	getchar();
	ma_decoder_uninit(&decoder);
	ma_engine_uninit(&engine);
	return(EXIT_SUCCESS);
}
