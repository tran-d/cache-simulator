/* ECE 250 -- HW5 -- Cache Simulation
 * David Tran
 * Due: April 14, 2017 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cachesim_structs_methods.h"

/* Declare Global Structs */
cache *cache_ptr = NULL;
address *main_memory = NULL;

/* Convert hexadecimal to binary */
char *hexToBinary(const char hex[]) {
	char* binAddress; char* bs;
	int i=0;
	int j=0;

	bs = (char*) malloc(sizeof(char) * 25);
	// char bs[25];
	bs[24] = '\0';
	// binAddress = bs;

    if(hex[i] == '0' && hex[i+1] == 'x') {		//skip 0x
    	i = i + 2;
    }

    while(hex[i]) {
    	switch(hex[i]) {
    		case '0': bs[j] = '0'; bs[j+1] = '0'; bs[j+2] = '0'; bs[j+3] = '0'; break;
    		case '1': bs[j] = '0'; bs[j+1] = '0'; bs[j+2] = '0'; bs[j+3] = '1'; break;
    		case '2': bs[j] = '0'; bs[j+1] = '0'; bs[j+2] = '1'; bs[j+3] = '0'; break;
    		case '3': bs[j] = '0'; bs[j+1] = '0'; bs[j+2] = '1'; bs[j+3] = '1'; break;
    		case '4': bs[j] = '0'; bs[j+1] = '1'; bs[j+2] = '0'; bs[j+3] = '0'; break;
    		case '5': bs[j] = '0'; bs[j+1] = '1'; bs[j+2] = '0'; bs[j+3] = '1'; break;
    		case '6': bs[j] = '0'; bs[j+1] = '1'; bs[j+2] = '1'; bs[j+3] = '0'; break;
    		case '7': bs[j] = '0'; bs[j+1] = '1'; bs[j+2] = '1'; bs[j+3] = '1'; break;
    		case '8': bs[j] = '1'; bs[j+1] = '0'; bs[j+2] = '0'; bs[j+3] = '0'; break;
    		case '9': bs[j] = '1'; bs[j+1] = '0'; bs[j+2] = '0'; bs[j+3] = '1'; break;
    		case 'a': bs[j] = '1'; bs[j+1] = '0'; bs[j+2] = '1'; bs[j+3] = '0'; break;
    		case 'b': bs[j] = '1'; bs[j+1] = '0'; bs[j+2] = '1'; bs[j+3] = '1'; break;
    		case 'c': bs[j] = '1'; bs[j+1] = '1'; bs[j+2] = '0'; bs[j+3] = '0'; break;
    		case 'd': bs[j] = '1'; bs[j+1] = '1'; bs[j+2] = '0'; bs[j+3] = '1'; break;
    		case 'e': bs[j] = '1'; bs[j+1] = '1'; bs[j+2] = '1'; bs[j+3] = '0'; break;
    		case 'f': bs[j] = '1'; bs[j+1] = '1'; bs[j+2] = '1'; bs[j+3] = '1'; break;
    		default: printf("Invalid Hex Address"); break;
    	}
    	i++;
    	j = j+4;
    }
    return bs;
};

int binaryDecConversion(char *binary_str) {
	int decimal = 0;
	long long n = strtoll(binary_str,NULL,2);
    decimal = (int) n;
}

/* Calculate Size of TAG, INDEX, OFFSET bits */
int calcTAG(address *main_memory) {
	return 24 - main_memory->no_index_bits - main_memory->no_offest_bits;
}

int calcINDEX() {
	return (int) log2(cache_ptr->num_sets);
}

int calcOFFSET() {
	return (int) log2(cache_ptr->block_size);
}

int calcNumSets() {
	int num_sets = (int) (cache_ptr -> cache_size) / ((cache_ptr -> block_size) * (cache_ptr -> assoc_val));
	return num_sets;
}

int calcTraceLines(FILE *file) {
	rewind(file);
	int num_lines = 0;
	char buffer[20];
	while(fscanf(file, "%s",buffer) != EOF) {
		if((strcmp(buffer,"store") == 0) || (strcmp(buffer,"load") == 0))
			num_lines++;
	}
	return num_lines;
}

void initCache() {
	cache_ptr = malloc(sizeof(cache));
	cache_ptr ->hits = 0;
	cache_ptr ->misses = 0;
	cache_ptr ->loads = 0;
	cache_ptr ->stores = 0;

	cache_ptr ->cache_size = 0;
	cache_ptr ->block_size = 0;

	cache_ptr ->set_size = 0;
	cache_ptr ->set_assoc = NULL;
	cache_ptr ->assoc_val = 0;
	cache_ptr ->num_sets = 0;
	cache_ptr -> sets = NULL;

	cache_ptr ->write_policy = NULL;
};

/* creates each "way" in a set */
cache_row* initLine() {
	cache_row *ptr;
	ptr = malloc(sizeof(cache_row));
	ptr->valid = 0;
	ptr->dirty = 0;
	ptr->tag_bits = (char*) malloc(sizeof(char) * 25);
	ptr -> LRU = 0;
	ptr->cache_data = malloc(sizeof(char) * cache_ptr->block_size * 2 + 1);
	ptr->set_bits = malloc(sizeof(char) * calcINDEX());
	ptr->offset_bits = malloc(sizeof(char) * calcINDEX());
	return ptr;
};

void initSet(cache_set *set) {
	set->rows = malloc(sizeof(cache_row) * cache_ptr->set_size);
}

void initEmptyCache() {
	int i, row;
	cache_ptr ->sets = malloc(sizeof(cache_set*) * cache_ptr->num_sets);
	for (i=0; i < cache_ptr->num_sets; i++) {
		cache_ptr->sets[i] = malloc(sizeof(cache_set)); //issue
		initSet(cache_ptr->sets[i]);
		for(row = 0; row < cache_ptr->set_size; row++) {
			cache_ptr->sets[i]->rows[row] = initLine();
		}
	}
}

memory_block* initMemBlocks() {
	memory_block *mem_block_ptr;
	mem_block_ptr =  NULL; //(struct memory_block*) malloc(sizeof(struct memory_block));
	mem_block_ptr -> dec_address = 0;			//address
	mem_block_ptr -> data_bits = NULL;			//data: 2 characters (1 byte) * "block_size" per block
	mem_block_ptr -> next_block = NULL; //linked-list of blocks
	return mem_block_ptr;
}

void initAddress(address *main_memory) {
	main_memory -> no_offest_bits = 0;
	main_memory -> no_index_bits = 0;
	main_memory -> no_tag_bits = 0;

	main_memory -> tag_bits = NULL;
	main_memory -> set_bits = NULL;
	main_memory -> offset_bits = NULL;

	main_memory -> loadOrStore = NULL;
	main_memory -> DSetIndex = 0;
}

void initEmptyMemory() {
	main_memory = malloc(sizeof(address));
}

void fillMemBlock(address *main_memory, char* hexAddress, char* sw_data) {
	// char *bs = hexToBinary(hexAddress);
	// int DecAddress = binaryDecConversion(bs);
	// int NodeDecAddress = DecAddress/cache_ptr->block_size;
	// int DecOffset_bits= binaryDecConversion(main_memory -> offset_bits);
	// int data_index = DecAddress % cache_ptr->block_size;
	// free(bs);
	insertMemBlock(main_memory, hexAddress, sw_data);
}

void writeBackMemory(address *main_memory, char* hexAddress, char* cache_data) {
	char *bs = hexToBinary(hexAddress);
	int DecAddress = binaryDecConversion(bs);
	int NodeDecAddress = DecAddress/cache_ptr->block_size;
	// int DecOffset_bits= binaryDecConversion(main_memory -> offset_bits);
	int data_index = DecAddress % cache_ptr->block_size;
	int i;
	free(bs);
	// if(data_index == DecOffset_bits)


	// printf("Cache-Node Address: %d\n",NodeDecAddress);

	// printf("cache_data in Mem: %s\n",cache_data);
	if (main_memory -> blocks == NULL) {
		main_memory -> blocks = malloc(sizeof(struct memory_block));
		main_memory -> blocks -> data_bits = malloc(sizeof(char)*cache_ptr->block_size*2 + 1);
		main_memory -> blocks -> dec_address = NodeDecAddress;
		strcpy(main_memory -> blocks -> data_bits, cache_data);
		main_memory -> blocks -> next_block = NULL;
		main_memory -> blocks -> data_bits[cache_ptr->block_size*2] = '\0';
		// printf("FIRST NODE\n");
		// printf("Head Node: %s\n",main_memory -> blocks -> data_bits);
	}

	else {
		if(NodeDecAddress == main_memory -> blocks -> dec_address)		//node exists then update
			strcpy(main_memory -> blocks -> data_bits, cache_data);
		else if (NodeDecAddress < main_memory -> blocks -> dec_address) {	// insert before head node
			memory_block *temp2 = malloc(sizeof(struct memory_block));
			temp2 -> dec_address = NodeDecAddress;
			temp2 -> data_bits = malloc(sizeof(char)*cache_ptr->block_size*2 + 1);
			strcpy(temp2 -> data_bits, cache_data);
			temp2 -> data_bits[cache_ptr->block_size*2] = '\0';
			temp2 -> next_block = main_memory -> blocks;	//link 1st node to 2nd node
			main_memory -> blocks = temp2;
		}
		else if (NodeDecAddress > main_memory -> blocks -> dec_address) {
			memory_block *temp2 = main_memory -> blocks;					//start from beginning and find node
			memory_block *insert = malloc(sizeof(struct memory_block));
			
			insert -> dec_address = NodeDecAddress;
			insert -> data_bits = malloc(sizeof(char)*cache_ptr->block_size*2 + 1);
			strcpy(insert -> data_bits, cache_data);
			insert -> data_bits[cache_ptr->block_size*2] = '\0';

			i = 0;
			while (temp2 -> next_block != NULL) {
				if (insert -> dec_address > temp2 -> dec_address && insert -> dec_address < temp2 -> next_block -> dec_address) {
					insert -> next_block = temp2 -> next_block;
					temp2 -> next_block = insert;
					break;
				}
				else if (insert -> dec_address == temp2 -> dec_address) {
					strcpy(temp2 -> data_bits, cache_data);
					free(insert);
					break;
				}
				else {
					temp2 = temp2 -> next_block;
				}
			}


			if(temp2 -> next_block == NULL) {
				if(insert -> dec_address == temp2 -> dec_address) {
					strcpy(temp2 -> data_bits, cache_data);
					free(insert);
				}
				else temp2 -> next_block = insert;		// insert last
			}
			// printf("Written to memory node: %s\n", insert -> data_bits);
			// printf("Temp data bits %s\nInserted Cache data bits: %s\n",temp2 -> data_bits, insert -> data_bits);
		}
	}
	// printMem(main_memory);
}

void insertMemBlock(address *main_memory, char* hexAddress, char* sw_data) {
	char *bs = hexToBinary(hexAddress);
	int DecAddress = binaryDecConversion(bs);
	int NodeDecAddress = DecAddress/cache_ptr->block_size;
	int DecOffset_bits= binaryDecConversion(main_memory -> offset_bits);
	int data_index = DecAddress % cache_ptr->block_size;
	int i = 0; int j = 0;
	free(bs);
	// printf("Binary Address: %s\n", hexToBinary(hexAddress));
	// printf("DecAddress: %d\n", DecAddress);
	// printf("NODE MEM ADDRESS: %d\n", NodeDecAddress);
	// printf("Data Index in Node: %d\n", data_index);

	if (main_memory -> blocks == NULL) {
		main_memory -> blocks = malloc(sizeof(struct memory_block));
		main_memory -> blocks -> dec_address = NodeDecAddress;
		main_memory -> blocks -> data_bits = malloc(sizeof(char)*cache_ptr->block_size*2 + 1);
		main_memory -> blocks -> next_block = NULL;
		for (i= 0; i<cache_ptr->block_size*2;i++) {
			if (i < data_index*2 || i >= (data_index*2 + strlen(sw_data)))
				main_memory -> blocks -> data_bits[i] = '0';
			else {
				main_memory -> blocks -> data_bits[i] = sw_data[j];
				j++;
			}
		}
		main_memory -> blocks -> data_bits[cache_ptr->block_size * 2 ] = '\0';
		// printf("FIRST NODE\n");
		// printf("Head Node: %s\n",main_memory -> blocks -> data_bits);
		// printMem(main_memory);
	}

	memory_block *temp2 = main_memory -> blocks;

	if (main_memory->blocks != NULL) {	
		// printf("Initial TEMP Node Address: %d\n", temp -> dec_address);
		// printf("Initial TEMP Node DataBits: %s\n", temp -> data_bits);
		// printf("Initial HEAD Node Address: %d\n", temp2 -> dec_address);
		// printf("Initial HEAD Node DataBits: %s\n", temp2 -> data_bits);
	}	

	if(NodeDecAddress == main_memory -> blocks -> dec_address) {		//node exists
		updateMem(main_memory, hexAddress, sw_data);
	}

	else if (NodeDecAddress < main_memory -> blocks -> dec_address) {		//insert before
		memory_block *temp2 = malloc(sizeof(struct memory_block));

		temp2 -> dec_address = NodeDecAddress;
		temp2 -> data_bits = malloc(sizeof(char)*cache_ptr->block_size*2 + 1);
		temp2 -> next_block = main_memory -> blocks;	//link 1st node to 2nd node

		for (i= 0; i<cache_ptr->block_size*2;i++) {
			if (i < data_index*2 || i >= (data_index*2 + strlen(sw_data)))
				temp2 -> data_bits[i] = '0';
			else {
				temp2 -> data_bits[i] = sw_data[j];
				j++;
			}
		}
		temp2 -> data_bits[cache_ptr->block_size*2 ] = '\0';
		main_memory -> blocks = temp2;	//link main_memory to new 1st node


		// printf("Final TEMP Node Address: %d\n", temp2 -> dec_address);
		// printf("Final TEMP Node DataBits: %s\n", temp2 -> data_bits);
		// printf("Final HEAD Node Address: %d\n", main_memory -> blocks -> dec_address);
		// printf("Final HEAD Node DataBits: %s\n\n", main_memory -> blocks -> data_bits);
	}
	else if (NodeDecAddress > main_memory -> blocks -> dec_address) {
		memory_block *temp2 = main_memory -> blocks;		//start from beginning and find node
		memory_block *insert = malloc(sizeof(struct memory_block));
		insert -> dec_address = NodeDecAddress;
		insert -> data_bits = malloc(sizeof(char)*cache_ptr->block_size*2 + 1);
			
		for (i= 0; i<cache_ptr->block_size*2;i++) {
			if (i < data_index*2 || i >= (data_index*2 + strlen(sw_data)))
				insert -> data_bits[i] = '0';
			else {
				insert -> data_bits[i] = sw_data[j];
				j++;
			}
		}
		insert -> data_bits[cache_ptr->block_size * 2 ] = '\0'; // not + 1

		i = 0;
		while (temp2 -> next_block != NULL) {
			if (insert -> dec_address > temp2 -> dec_address && insert -> dec_address < temp2 -> next_block -> dec_address) {
				insert -> next_block = temp2 -> next_block;
				temp2 -> next_block = insert;
				break;
			}
			else if (insert -> dec_address == temp2 -> dec_address) {
				updateMem(main_memory,hexAddress,sw_data);
				free(insert);
				break;
			}
			else {
				temp2 = temp2 -> next_block;
			}
		}
		if(temp2 -> next_block == NULL) {
				if(insert -> dec_address == temp2 -> dec_address) {
					updateMem(main_memory,hexAddress,sw_data);
					free(insert);
				}
				else
					temp2 -> next_block = insert;		// insert last
		}
	}
	// printMem(main_memory);
}

void printMem(address *main_memory) {
	memory_block *temp = main_memory -> blocks;
	printf("MAIN MEMORY NODES: \n");
	while (temp != NULL) {
		printf("Node Dec. Address: %d\n", temp -> dec_address);
		printf("Node Data: %s\n", temp -> data_bits);
		temp = temp -> next_block;
	}
	printf("\n");
}

void updateMem(address *main_memory, char* hexAddress, char* sw_data) {
	char *bs = hexToBinary(hexAddress);
	int DecAddress = binaryDecConversion(bs);
	int NodeDecAddress = DecAddress/cache_ptr->block_size;
	int DecOffset_bits= binaryDecConversion(main_memory -> offset_bits);
	int data_index = DecAddress % cache_ptr->block_size;
	int i = 0; int j = 0;
	free(bs);
	// printf("SW_DATA: %s\n",sw_data);

	// printf("Update Binary Address: %s\n", hexToBinary(hexAddress));
	// printf("Update DecAddress: %d\n", DecAddress);
	// printf("Update NODE MEM ADDRESS: %d\n", NodeDecAddress);
	// printf("Update Data Index in Node: %d\n", data_index);
	memory_block *temp = main_memory -> blocks;

	while(temp != NULL){
		if (NodeDecAddress == temp -> dec_address) {
			for (i = 0; i < cache_ptr->block_size*2; i++) {
				if (i >= data_index*2 && i < data_index*2 + strlen(sw_data)) {
					temp -> data_bits[i] = sw_data[j];
					// printf("SW_DATA: %c\n",sw_data[j]);
					j++;
				}
			}
			temp -> data_bits[cache_ptr->block_size*2] = '\0';
			break;
		}
		else
			temp = temp -> next_block;
	}
	// printf("UPDATED TEMP Node Address: %d\n", temp -> dec_address);
	// printf("UPDATED TEMP Node DataBits: %s\n", temp -> data_bits);
}

void destroyMemory() {
	struct memory_block *head = main_memory -> blocks;
	struct memory_block *curr = main_memory -> blocks;
	while(head != NULL) {
		curr = head -> next_block;
		free(head -> data_bits);
		free(head);
		head = curr;;
	}
	// free(main_memory -> blocks);
	free(main_memory -> tag_bits);
	free(main_memory -> set_bits);
	free(main_memory -> offset_bits);
	free(main_memory);
}


void concat(char* str, char c) {
	int length = strlen(str);
	str[length] = c;
	str[length+1] = '\0';
}


void updateAddress(address *main_memory, char *address_bits) {
	int i;
	main_memory -> no_offest_bits = calcOFFSET();
	main_memory -> no_index_bits = calcINDEX();
	main_memory -> no_tag_bits = calcTAG(main_memory);

	
	// if (main_memory -> blocks != NULL)
	// 	printf("Before HEAD Node Address %s\n", main_memory -> blocks -> data_bits);
	// if (main_memory -> blocks != NULL)
	// 		printf("while: HEAD Node Address %d\n", main_memory -> blocks -> dec_address);
		

	/* alloc space for tag and EOS */
	main_memory -> tag_bits = malloc(sizeof(char)*(main_memory->no_tag_bits) + 1);
	// char tag[main_memory->no_tag_bits + 1];
	// main_memory -> tag_bits = tag;

	main_memory -> tag_bits[0] = '\0';
	for (i=0; i < main_memory->no_tag_bits; i++) {
		concat(main_memory->tag_bits, address_bits[i]);
	}

	main_memory -> set_bits = malloc(sizeof(char)*(main_memory->no_index_bits) + 1);
	// char set[main_memory->no_index_bits + 1];
	// main_memory -> set_bits = set;
	// printf("%d\n",main_memory->no_index_bits + main_memory->no_tag_bits);
	main_memory -> set_bits[0] = '\0';
	while(i < ((main_memory->no_index_bits) + (main_memory->no_tag_bits))) {
		concat(main_memory->set_bits, address_bits[i]);
		i++;
	}


	main_memory -> offset_bits = malloc(sizeof(char)*(main_memory->no_offest_bits) + 1);
	// char offset[main_memory -> no_offest_bits + 1];
	// main_memory -> offset_bits = offset;
	// printf("%d\n",main_memory->no_index_bits + main_memory->no_tag_bits);
	main_memory -> offset_bits[0] = '\0';
	while(i < ((main_memory->no_index_bits) + (main_memory->no_tag_bits) + (main_memory->no_offest_bits))) {
		concat(main_memory->offset_bits, address_bits[i]);
		i++;
	} 


	main_memory -> DSetIndex = binaryDecConversion(main_memory -> set_bits);
	// printf("Address Bits: %s\t",address_bits);
	// printf("DSetIndex: %d\t",main_memory->DSetIndex);
	// printf("Address Set Bits: %s\n",main_memory -> set_bits);

}

int findLRULocation(address *main_memory) {
	int i;
	int LRULocation = 0; int LRU_num = 0;
	for (i=0; i<cache_ptr-> set_size; i++) {
		if(cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> LRU > LRU_num) {
			LRU_num = cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> LRU; //find highest LRU num
			LRULocation = i;
		}
	}
	return LRULocation;
}

void updateLRU_MRU(address *main_memory) {
	int i;	
	int LRUlocation = findLRULocation(main_memory);
	for(i=0; i<cache_ptr->set_size; i++) {
		cache_ptr->sets[main_memory->DSetIndex] -> rows[i] ->LRU++; //increment LRU num
	}
	cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRUlocation] -> LRU = 0; //LRU will get kicked out--> new MRU has index 0
}

void WT_evictLRU_insertMRU(address *main_memory, char* hexAddress, int byteSize) {
	/* MISSES */
	int i;
	int LRU_num = 0;

	/* IF OPEN SPOT AVAILABLE -- THIS METHOD CALLED FOR LOADS ONLY */
	for (i = 0; i<cache_ptr->set_size; i++) {

		if(cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> valid == 0) {

			// For load, read data from lower memory into cache block
			char *load = loadIntoCacheBlockFromMemory(main_memory,hexAddress);
			strcpy(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data, load);
			if (checkForMalloc(load))
				load = NULL;
			
			// Print loaded data
			char *data = getData(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data,hexAddress,byteSize);
			printf("%s\n",data);
			free(data);

			// Cache block now valid
			cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> valid = 1;

			// Update cache block tag + address
			strcpy(cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> tag_bits, main_memory->tag_bits); //copy from main mem to cache block
			strcpy(cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> cache_address, hexAddress);

			// Update LRU, MRU
			updateLRU_MRU(main_memory);
			return;
		}
	}

	/* IF NO OPEN SPOT AVAILABLE */
	int LRULocation = findLRULocation(main_memory);

	// Locate cache block to use , load data from lower memory into cache block
	char *load = loadIntoCacheBlockFromMemory(main_memory,hexAddress);
			strcpy(cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_data, load);
			if (checkForMalloc(load))
				load = NULL;
	
	// printf("Written to cache block after load miss: %s\n", cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_data);
	char *data = getData(cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_data,hexAddress,byteSize);
	printf("%s\n",data);
	free(data);

	// Update cache block tag + address
	strcpy(cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> tag_bits,main_memory -> tag_bits); //evict LRU, insert MRU w/ index 0; increment other block LRUs
	strcpy(cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_address, hexAddress);
	
	// Update LRU, MRU
	updateLRU_MRU(main_memory);
}

void setMRU(address *main_memory, int MRUlocation) {
	cache_ptr->sets[main_memory->DSetIndex] -> rows[MRUlocation] -> LRU = 0;
}

void incrementLRU(address *main_memory, int MRULocation) {
	int i;
	for(i=0; i<cache_ptr->set_size; i++) {
		if(i != MRULocation)
			cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> LRU++;
	}
}

void writeThrough(address *main_memory, char* hexAddress, char* sw_data, int byteSize) {
	int i, j;
	for(i=0; i<cache_ptr->set_size;i++) {
		/*** HIT ***/
		if(cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> valid == 1 && \

			strcmp(main_memory->tag_bits, cache_ptr->sets[main_memory->DSetIndex]->rows[i]->tag_bits) == 0) {
			cache_ptr -> hits++;
			// printf("%s\n",main_memory->tag_bits);
			// printf("%d\n",cache_ptr->sets[main_memory->DSetIndex] -> rows[i]);

			if(strcmp(main_memory->loadOrStore,"store") == 0) {
				cache_ptr->stores++;

				// Store new data into cache block
				writeCacheBlock(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data, hexAddress, sw_data);
				
				// Write new data into memory
				writeBackMemory(main_memory, cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_address, cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> cache_data);
				// printf("Written to Memory after store hit: %s\n", cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> cache_data);
				// Update cache block address
				strcpy(cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> cache_address, hexAddress);
				
				printf("store ");
				printf("%s ",hexAddress);
				printf("hit\n");
				// printf("%s\n",sw_data);
			}

			else {
				// Load hit = Return data
				printf("load ");
				printf("%s ",hexAddress);
				printf("hit ");
				char *data = getData(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data,hexAddress,byteSize);
				printf("%s\n",data);
				free(data);
			}

			setMRU(main_memory,i);
			incrementLRU(main_memory,i); //increment all LRU except the MRU
			return;
		}	
	}

	/* MISS */
	cache_ptr->misses++;
	if (strcmp(main_memory->loadOrStore, "store") == 0) {
		cache_ptr -> stores++;
		printf("store ");
		printf("%s ",hexAddress);
		printf("miss\n");
		
		// Write Data into lower memory
		fillMemBlock(main_memory, hexAddress, sw_data);
	}
	else {
		printf("load ");
		printf("%s ",hexAddress);
		printf("miss ");
		// printf("%s\n",sw_data);

		// Read data from lower memory into cache block
		WT_evictLRU_insertMRU(main_memory, hexAddress, byteSize);
	}
	cache_ptr->loads++;
}

void WB_removeLRU_insertMRU(address *main_memory, char* hexAddress, char* sw_data, int byteSize) {
	int i;
	int LRULocation = 0;
	int LRU_num = 0;
	for(i = 0; i < cache_ptr -> set_size; i++) {

		/* IF THERE IS AN OPEN SPOT IN CACHE (VALID = 0) */
		if(cache_ptr -> sets[main_memory->DSetIndex]-> rows[i] -> valid == 0) {
			// printf("valid == 0\n");
			if (strcmp(main_memory->loadOrStore , "load") == 0) {

				
				// Load the data from memory
				char *load = loadIntoCacheBlockFromMemory(main_memory,hexAddress);
				strcpy(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data, load);
				if (checkForMalloc(load))
					load = NULL;
				// Loaded data is not dirty:
				cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> dirty = 0; //loading data from memory --> no more dirty
				
				// Print your specified loaded data
				char *data = getData(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data,hexAddress,byteSize);
				printf("%s\n",data);
				free(data);
			}

			else {
				
				// Load an empty cache block
				char *load = loadIntoCacheBlockFromMemory(main_memory,hexAddress);
				strcpy(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data, load);
				if (checkForMalloc(load))
					load = NULL;

				// Store new data into cache block
				writeCacheBlock(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data, hexAddress, sw_data);
				
				// Newly stored data is now dirty
				cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> dirty = 1;
				// printf("STORE IS DIRTY\n");
				
				// printf("Read Memory to Cache Block: %s\n",cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data);
				// printf("Written to Cache Block: %s\n",cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data);
			}

			// First cache block now has valid data
			cache_ptr-> sets[main_memory->DSetIndex] -> rows[i] -> valid = 1;
			// printf("valid == 1\n");

			// Update Cache block tag + address
			strcpy(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> tag_bits, main_memory->tag_bits);
			strcpy(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_address, hexAddress);
			
			// Increment LRU, MRU
			updateLRU_MRU(main_memory);
			return;
		}
	}

	/* IF THERE IS NO OPEN SPOT IN CACHE */
	// Find cache block to be removed
	LRULocation = findLRULocation(main_memory); 
	// printf("LRU Location: %d\n",LRULocation);
	
	/* If dirty --> write old data into memory */
	if (cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> dirty == 1) {
			cache_ptr -> stores++;
			printf("write back the dirty data\n");
			writeBackMemory(main_memory, cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_address, cache_ptr->sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_data); 
			// Free cache block for new data to be stored
			// free(cache_ptr->sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_data);

		}

		/* Read data from lower memory into cache block */
		char *load = loadIntoCacheBlockFromMemory(main_memory,hexAddress);
		strcpy(cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_data, load);
		if (checkForMalloc(load))
			load = NULL;

		if(strcmp(main_memory->loadOrStore, "load") == 0) {		
		
		// Loaded data from memory is not dirty
		cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> dirty = 0;
		
		// Print the specified loaded data using byteSize
		char *data = getData(cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_data,hexAddress,byteSize);
		printf("%s\n",data);
		free(data);
		
		// printf("Written to Memory: %s\n",cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation]-> cache_data);
		// printf("Loaded %s\n",getData(cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_data,hexAddress,byteSize));
	}

	else {

		// If store, write new data into cache block
		writeCacheBlock(cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_data, hexAddress, sw_data);
		
		// Newly stored data into cache is now dirty w.r.t. memory
		cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> dirty = 1;
		
		// printf("Written to Cache Block: %s\n",cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_data);
	}
	
	// Update Cache block tag and address
	cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> tag_bits = main_memory->tag_bits;
	strcpy(cache_ptr -> sets[main_memory->DSetIndex] -> rows[LRULocation] -> cache_address, hexAddress);
	
	// Increment LRU, MRU
	updateLRU_MRU(main_memory);
}

void writeBack(address *main_memory, char* hexAddress, char* sw_data, int byteSize) {
	int i, j;
	for (i=0; i < cache_ptr -> set_size; i++) {
		if (cache_ptr-> sets[main_memory->DSetIndex] -> rows[i] -> valid == 1 &&\
			strcmp(main_memory->tag_bits,cache_ptr->sets[main_memory->DSetIndex] -> rows[i] -> tag_bits) == 0) {
			
			/* HIT */
			cache_ptr-> hits++;
			// Set as MRU block
			cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> LRU = 0;
			
			if (strcmp(main_memory->loadOrStore, "store") == 0) {
				
				// Update cache block on hit
				writeCacheBlock(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data, hexAddress, sw_data);
				// Newly stored data is now dirty
				cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> dirty = 1; //set dirty for WB-WRITE-HIT (write new data in cache block) //never dirty for load
				
				printf("store ");
				printf("%s ",hexAddress);
				printf("hit\n");
				// printf("%s\n",sw_data);
			}
			else {
				printf("load ");
				printf("%s ",hexAddress);
				printf("hit ");
				// Print specified data
				char *data = getData(cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data,hexAddress,byteSize);
				printf("%s\n",data);
				free(data);
				// printf("Cache Block: %s\n\n",cache_ptr -> sets[main_memory->DSetIndex] -> rows[i] -> cache_data);
			}

			incrementLRU(main_memory, i);
			return;
		}
	}

	/* MISS */
	cache_ptr -> misses++;
	if (strcmp(main_memory->loadOrStore, "store") == 0) {
		printf("store ");
		printf("%s ",hexAddress);
		printf("miss\n");
	}
	else {
		printf("load ");
		printf("%s ",hexAddress);
		printf("miss ");
	}
	cache_ptr -> loads++;

	WB_removeLRU_insertMRU(main_memory, hexAddress, sw_data, byteSize);
}

void destroyCache() {
	int i, j;
	for(i = 0; i < cache_ptr -> num_sets; i++) {
		for(j=0; j< cache_ptr -> set_size; j++) {
			free(cache_ptr -> sets[i] -> rows[j] -> tag_bits);
			free(cache_ptr -> sets[i] -> rows[j] -> set_bits);
			free(cache_ptr -> sets[i] -> rows[j] -> offset_bits);
			free(cache_ptr -> sets[i] -> rows[j] -> cache_data);
			free(cache_ptr -> sets[i] -> rows[j]);
		}
		free(cache_ptr -> sets[i] -> rows);
		free(cache_ptr -> sets[i]);
	}
	free(cache_ptr -> sets);
	free(cache_ptr);
}

void updateCacheWithTrace(FILE *file, int num_lines) {
	rewind(file);
	char hexAddress[9];
	char lw_sw[9];
	char *binaryAddress;
	int byteSize;
	char sw_data[10];
	char access_size[5];

	//format: lw_sw | hexAddress | access_size | sw_data
	while(fscanf(file, "%s",lw_sw) != EOF) {

		/* CASE STORE */
		if(strcmp(lw_sw, "store") == 0) {
			fscanf(file,"%s",hexAddress);
			fscanf(file,"%s",access_size);
			fscanf(file,"%s",sw_data);
			// printf("Hex Address: \t\t\t%s\n",hexAddress);
			// printf("Size of Access in Bytes: \t%s\n",access_size);
			// printf("Store Data: \t\t\t%s\n",sw_data);
			initAddress(main_memory);
			main_memory -> loadOrStore = "store";
		}

		/* CASE LOAD */
		else if(strcmp(lw_sw,"load") == 0) {

			fscanf(file,"%s",hexAddress);
			fscanf(file,"%s",access_size);
			// printf("Hex Address: \t\t\t%s\n",hexAddress);
			// printf("Size of Access in Bytes: \t%s\n",access_size);
			initAddress(main_memory);
			main_memory -> loadOrStore = "load";
		}

		free(main_memory -> tag_bits);
		free(main_memory -> set_bits);
		free(main_memory -> offset_bits);
		binaryAddress = hexToBinary(hexAddress);
		byteSize = (int) strtoll(access_size, NULL, 10);
		
		

		// Update memory tag, set, offset bits
		updateAddress(main_memory,binaryAddress);

		if (strcmp(cache_ptr->write_policy, "wt") == 0) {
			writeThrough(main_memory, hexAddress, sw_data, byteSize);
		}
		else {
			writeBack(main_memory,hexAddress,sw_data, byteSize);
		}

		
		free(binaryAddress);
	}
}

char* loadIntoCacheBlockFromMemory(address *main_memory, char* hexAddress) {
	struct memory_block *temp = main_memory -> blocks;
	char *bs = hexToBinary(hexAddress);
	int DecAddress = binaryDecConversion(bs);
	int NodeDecAddress = DecAddress/cache_ptr->block_size;
	
	
	while(temp != NULL) {
		if (NodeDecAddress == temp -> dec_address) {
			// printf("Loaded Data Bits into Cache: %s\n", temp -> data_bits);
			free(bs);
			return temp -> data_bits;
		}
		temp = temp -> next_block;
	}
	free(bs);
	return returnEmptyBlock();
}

char* returnEmptyBlock() {
	char *out = malloc(sizeof(char)*cache_ptr->block_size*2 +1); 
	int i = 0;
	out[0] = '\0';
	for (i=0; i<cache_ptr->block_size*2;i++) {
		concat(out,'0');
	}
	// printf("empty: %s", out);
	return out;
}

bool checkForMalloc(char* str) {
	int i;
	for(i = 0; i < strlen(str); i++) {
		if (str[i] != '0')
			return false;
	}
	return true;
}

char* getData(char* cache_data, char *hexAddress, int byteSize) {
	char *bs = hexToBinary(hexAddress);
	int DecAddress = binaryDecConversion(bs);
	int NodeDecAddress = DecAddress/cache_ptr->block_size;
	int DecOffset_bits= binaryDecConversion(main_memory -> offset_bits);
	int data_index = DecAddress % cache_ptr->block_size;
	int i;
	char *data = malloc(sizeof(char)*byteSize*2 + 1);
	data[0] = '\0';
	// printf("Offset: %d\n",DecOffset_bits);
	
	for(i=0; i<strlen(cache_data);i++) {
		if(i >= data_index*2 && i<data_index*2 + byteSize*2)
			concat(data,cache_data[i]);
			// printf("Loaded Data: %c\n", cache_data[i]);
	}
	free(bs);
	return data;
}

void writeCacheBlock(char* cache_data, char* hexAddress, char* sw_data) {
	char *bs = hexToBinary(hexAddress);
	int DecAddress = binaryDecConversion(bs);
	int NodeDecAddress = DecAddress/cache_ptr->block_size;
	int DecOffset_bits= binaryDecConversion(main_memory -> offset_bits);
	int data_index = DecAddress % cache_ptr->block_size;
	int i; int j = 0;

	// printf("length: %d\n",strlen(cache_data));

	for(i=0; i<strlen(cache_data);i++) {
		if(i >= data_index*2 && i<data_index*2 + strlen(sw_data)) {
			cache_data[i] = sw_data[j];
			j++;
		}
		// printf("Fill Cache Block: %s\n",cache_data);
	}
	free(bs);
}

void printInfo();

int main(int argc, char *argv[]) {
	
	FILE *tracefile;
	char *writepol, *file_arg;
	int cacheSize, assocVal, blockSize, num_lines;
	
	/* FORMAT: tracefile | cache_size | assoc_val | write_policy | block_size */
	initCache();
	file_arg = argv[1];
	cacheSize = atoi(argv[2]);
	assocVal = atoi(argv[3]);
	writepol = argv[4];
	blockSize = atoi(argv[5]);
	// printf("File: %s\nCache Size: %dKB\nAssociativity: %d-way\nWrite Policy: %s\n"\
		"Block Size: %dB\n", file_arg, cacheSize, assocVal, writepol, blockSize);

	/* Set Cache Fields */
	cache_ptr -> cache_size = cacheSize*1024;
	cache_ptr -> assoc_val = assocVal;
	cache_ptr -> write_policy = writepol;
	cache_ptr -> block_size = blockSize;

	cache_ptr -> num_sets = calcNumSets();
	// printf("Number of Sets: %d\n", cache_ptr -> num_sets);

	cache_ptr -> set_size = cache_ptr -> assoc_val;
	// printf("Set Size: %d\n", cache_ptr -> set_size);

	initEmptyCache();
	initEmptyMemory();
	tracefile = fopen(file_arg,"r");
	num_lines = calcTraceLines(tracefile);

	updateCacheWithTrace(tracefile, num_lines);
	// printInfo();
	destroyCache();
	destroyMemory();

	return 0;
}

void printInfo() {
    	printf("Memory reads: %d\n", cache_ptr -> loads);
    	printf("Memory writes: %d\n", cache_ptr -> stores);
    	printf("Cache hits: %d\n", cache_ptr -> hits);
    	printf("Cache misses: %d\n", cache_ptr -> misses);
}


