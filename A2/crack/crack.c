#include <gmp.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

// This is pretty inefficient in terms of space/speed -- 
// sorry about that, but time was short!


// Also, this program assumes that the first line of input will be your exponent,
// and the second line is the RSA modulus number (n).

int main(){
	char exponentString[4097];
	char nString[4097];
	char inputString[4097];

	mpz_t exponent; // RSA Exponent
	mpz_t* iArray; // Literally 1-127 but mpz_t
	mpz_t modn; // RSA modulus number
	mpz_t* asciiMap; // All ascii characters raised to exponent mod n
	mpz_t current; // Current input for consideration

	asciiMap = malloc(sizeof(mpz_t) * 127); // 127 standard ascii characters
	iArray = malloc(sizeof(mpz_t) * 127); // 1-127...
	int flag;
	
	printf("Give me your exponent:\n");
	scanf("%4096s", exponentString);

	printf("Alright. Give me your n:\n");
	scanf("%4096s", nString);

	printf("Initializing exponent...\n");
	// Initialize exponent
	mpz_init(exponent);
	mpz_set_ui(exponent, 0);

	printf("Initializing n...\n");
	// Initialize n
	mpz_init(modn);
	mpz_set_ui(modn, 0);

	printf("Initializing ASCII table and filling i array...\n");
	// Initialize ascii placeholders, fill iArray
	for (unsigned long int i = 0; i < 127; i++){
		mpz_init(asciiMap[i]);
		mpz_set_ui(asciiMap[i], 0);

		mpz_init(iArray[i]);
		mpz_set_ui(iArray[i], i);
	} 

	printf("Filling exponent...\n");
	// Parse string into e
	flag = mpz_set_str(exponent, exponentString, 10);
	assert (flag == 0);

	printf("Filling n...\n");
	// Parse string into n
	flag = mpz_set_str(modn, nString, 10);
	assert (flag == 0);


	printf("Filling ASCII table...\n");
	// Fill out ASCII Array
	for (int i = 0; i < 127; i++){
		mpz_powm(asciiMap[i], iArray[i], exponent, modn);	
	}

	
	mpz_init(current);
	mpz_set_ui(current, 0);	

	while(mpz_cmp_ui(current, -1) != 0) {
		scanf("%4096s", inputString);// Take input
	

		flag = mpz_set_str(current, inputString, 10);
		assert (flag == 0);

		unsigned char found = 0;
		for(int i = 0; i < 127; i++){
			if (mpz_cmp (current, asciiMap[i]) == 0){
				printf("%c", i);
				found = 1;
				break;
			}
		}
		if(found == 0) {
			printf("End of stream!\n"); //Note: Will also print if ASCII character not found
			break;
		}
	}

	// Clear out memory
	mpz_clear(exponent);
	free(asciiMap);
	free(iArray);
}
