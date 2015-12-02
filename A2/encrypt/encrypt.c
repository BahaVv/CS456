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
	char c;

	mpz_t exponent; // RSA Exponent
	mpz_t modn; // RSA modulus number
	mpz_t current; // Current input for consideration
	mpz_t tempBig;

	int flag;
	int temp;
	
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

	printf("Filling exponent...\n");
	// Parse string into e
	flag = mpz_set_str(exponent, exponentString, 10);
	assert (flag == 0);

	printf("Filling n...\n");
	// Parse string into n
	flag = mpz_set_str(modn, nString, 10);
	assert (flag == 0);

	mpz_init(current);
	mpz_set_ui(current, 0);

	mpz_init(tempBig);
	mpz_set_ui(tempBig, 0);

	c = getchar();

	while(c != EOF) {
		temp = c;
		mpz_set_ui(tempBig, temp);	
		mpz_powm(current, tempBig, exponent, modn);
		gmp_printf ("%Zd\n", current);	
		temp = 0;
		mpz_set_ui(tempBig, 0);
		c = getchar();
	}

	// Clear out memory
	mpz_clear(exponent);
	mpz_clear(modn);
	mpz_clear(current);
}
