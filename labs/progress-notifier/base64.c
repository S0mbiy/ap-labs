#include <stdio.h>
#include "logger.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////Imported base64 code////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};


char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length) {

    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return encoded_data;
}


void build_decoding_table() {

    decoding_table = malloc(256);

    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}


unsigned char *base64_decode(const char *data,
                             size_t input_length,
                             size_t *output_length) {

    if (decoding_table == NULL) build_decoding_table();


    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}


void base64_cleanup() {
    free(decoding_table);
}
////////////////////////////////////////////////////////////////////////////////
// Imported base64 Code from:
// https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
// user: https://stackoverflow.com/users/282635/ryyst
////////////////////////////////////////////////////////////////////////////////
float count_lines, processed_lines;
void handle_sig(int sig)
{
	if(sig==2 || sig==31){
    	infof("Progress is: %.2f%%", (processed_lines/count_lines)*100);
	}
}
int main(int argc, char *argv[]){
    // Place your magic here
	initLogger("stdout");
	count_lines = 1.;
	processed_lines = 0.;
	signal(2, handle_sig);
	signal(31, handle_sig);
	if(argc<3){
	errorf("Usage: ./base64 --<encode/decode> <text.txt>");
	return 1;
	}
	if(strcmp(argv[1],"--encode")==0){
		FILE *outfp = fopen("encoded.txt", "w");
		FILE * fp;
	    char * line = NULL;
	    size_t len = 0;
	    ssize_t read;
 	    fp = fopen(argv[2], "r");
 	    if (fp == NULL)
		    exit(EXIT_FAILURE);
		char ch;
		while(!feof(fp))
		{
		  ch = fgetc(fp);
		  if(ch == '\n')
		  {
		    count_lines+=1;
		  }
		}
		fp = fopen(argv[2], "r");
		if (fp == NULL)
		    exit(EXIT_FAILURE);
		size_t output = 0;
	    while ((read = getline(&line, &len, fp)) != -1) {
			processed_lines+=1;
			fprintf(outfp, "%s\n", base64_encode(line, read, &output));
	   }
	   fclose(outfp);
	   fclose(fp);
	   if (line)
		   free(line);
	}else if(strcmp(argv[1],"--decode")==0){
		FILE *outfp = fopen("decoded.txt", "w");
		FILE * fp;
	    char * line = NULL;
	    size_t len = 0;
	    ssize_t read;
 	    fp = fopen(argv[2], "r");
 	    if (fp == NULL)
		    exit(EXIT_FAILURE);
		char ch;
		while(!feof(fp))
		{
		  ch = fgetc(fp);
		  if(ch == '\n')
		  {
		    count_lines+=1;
		  }
		}
		fp = fopen(argv[2], "r");
 	    if (fp == NULL)
		    exit(EXIT_FAILURE);
		size_t output = 0;
	    while ((read = getline(&line, &len, fp)) != -1) {
			processed_lines+=1;
			fprintf(outfp, "%s", base64_decode(line, read-1, &output));
	   }
	   fclose(outfp);
	   fclose(fp);
	   if (line)
		   free(line);
	   base64_cleanup();
	}else{
		errorf("Invalid option, use --encode or --decode only");
		return 1;
	}

    return 0;
}
