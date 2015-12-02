#include <stdio.h>
#include <stdlib.h>

// Byte type [0, 256)--used for conversion and some indices
typedef unsigned char uchar;

#define STATES 256

// State type--the type of an ephemeral state in the state machine
typedef struct s_state {
	uchar chmap[STATES];  // Map from inbyte -> outbyte
	uchar stmap[STATES];  // Map from inbyte -> next state
} state;

// Machine type (represents the key)
typedef struct s_machine {
	state states[STATES];  // Valid states in the machine
	uchar current;      // Current state index
} machine;

#define MACH_STATE(mach) ((mach)->states[(mach)->current])

// Do a round (encryption forward, decryption reverse)
uchar do_round(machine *mach, uchar inbyte) {
	// Follow to the next state
	uchar ret = MACH_STATE(mach).chmap[inbyte];
	mach->current = MACH_STATE(mach).stmap[inbyte];
	return ret;
}

// Reverse a machine -- alter it such that it permits decryption instead of encryption
void rev_mach(machine *mach) {
	int state, byte;
	uchar revch[STATES];  // Holds the actual reverse character mapping while it is being constructed
	uchar revst[STATES];  // Holds outbyte -> next state 
	
	// For each state...
	for(state = 0; state < STATES; state++) {
		// Construct the reversed mapping
		for(byte = 0; byte < STATES; byte++) {
			revch[mach->states[state].chmap[byte]] = byte;
			revst[mach->states[state].chmap[byte]] = mach->states[state].stmap[byte];
		}

		// ...and copy the reversed mapping into that state
		for(byte = 0; byte < STATES; byte++) {
			mach->states[state].chmap[byte] = revch[byte];
			mach->states[state].stmap[byte] = revst[byte];
		}
	}
}

// Initialize a machine to a new identity state
// WARNING: This machine does not perform effective confidentiality!
void init_mach(machine *mach) {
	int state, byte;
	
	for(state = 0; state < STATES; state++) {
		for(byte = 0; byte < STATES; byte++) {
			mach->states[state].chmap[byte] = byte;
			mach->states[state].stmap[byte] = byte;
		}
	}

	mach->current = 0;
}

// Initialize a machine to a new, random state (a new random key)
void rand_mach(machine *mach) {
	int iteration;
	uchar state, byte1, byte2, temp;

	// Initialize the machine to an identity...
	init_mach(mach);

	// ...then start swapping values to make random permutations
	// (it is important that these maps remain permutations such that encodings are unique)
	// (The upper bound is arguable.)
	for(iteration = 0; iteration < STATES * STATES; iteration++) {
		state = rand() % STATES;

		// Choose characters to swap
		byte1 = rand() % STATES;
		byte2 = rand() % STATES;
		// ...actually swap
		temp = mach->states[state].chmap[byte1];
		mach->states[state].chmap[byte1] = mach->states[state].chmap[byte2];
		mach->states[state].chmap[byte2] = temp;

		// Now swap transition states (with different values)
		byte1 = rand() % STATES;
		byte2 = rand() % STATES;
		temp = mach->states[state].stmap[byte1];
		mach->states[state].stmap[byte1] = mach->states[state].stmap[byte2];
		mach->states[state].stmap[byte2] = temp;
	}
}

void write_mach(machine *mach, FILE *out) {
	fwrite(mach, sizeof(machine), 1, out);
}

void read_mach(machine *mach, FILE *in) {
	fread(mach, sizeof(machine), 1, in);
}

void usage(void) {
	printf("Usage:\n"
		  " e <key>: Encrypt stdin to stdout\n"
		  " d <key>: Decrypt stdin to stdout\n"
		  " n: Write a new random key to stdout\n"
		  " i: Write an identity key to stdout\n");
}

int main(int argc, char **argv) {
	machine mach;
	FILE *key;
	char op;
	int byte;

	if(argc < 2) {
		printf("Improper number of arguments\n");
		usage();
		return 1;
	}

	op = argv[1][0];
	switch(op) {
		case 'e': case 'd':
			if(argc < 3) {
				printf("Missing key argument");
				usage();
				return 2;
			}
			key = fopen(argv[2], "rb");
			if(!key) {
				printf("Bad filename: %s\n", argv[2]);
				usage();
				return 3;
			}
			read_mach(&mach, key);
			if(op == 'd') {
				rev_mach(&mach);
			}
			while((byte = getchar()) != EOF) {
				putchar((int) do_round(&mach, (uchar) byte));
			}
			break;

		case 'n':
			rand_mach(&mach);
			write_mach(&mach, stdout);
			break;

		case 'i':
			init_mach(&mach);
			write_mach(&mach, stdout);
			break;
	}
	return 0;
}
