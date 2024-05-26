/* 20220127 Yoojin Lim */
#include "cachelab.h"
#include "stdio.h"
#include "getopt.h"
#include "stdlib.h"

//define struct
typedef struct line {
    char valid;
    int count;
    unsigned long tag;
} Line;

//function declaration
int string_to_num(char* c);
void cache_line_match(Line* cache, int E, int count, unsigned long tag, int* hit_count, int* miss_count, int* eviction_count);
int power(int num, int exp);

int main(int argc, char *argv[])
{
    //define variables
    int hit_count = 0, miss_count = 0, eviction_count = 0, count = 0;
    int i, j;
    int s, E, b;
    int opt;
    int data_size, set_num;
    char* trace_file;
    FILE* fp;
    char op;
    unsigned long mask, tag, address, block_offset, set;
    Line** cache_line;

    //parse command line argument
    while((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch(opt) {
            case 'h':
                break;
            case 'v':
                break;
            case 's':
                s = string_to_num(optarg);
                break;
            case 'E':
                E = string_to_num(optarg);
                break;
            case 'b':
                b = string_to_num(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;   
        }
    }

    //memory allocation for cache
    set_num = power(2, s);  
    cache_line = (Line**)malloc(sizeof(Line*) * set_num);
    for(i = 0; i < set_num; i++) {
        cache_line[i] = (Line*)malloc(sizeof(Line) * E);
    }

    //cache initialization
    for(i = 0; i < set_num; i++) {
        for(j = 0; j < E; j++) {
            cache_line[i][j].valid = 0;
            cache_line[i][j].tag = 0;
            cache_line[i][j].count = 0;
        }
    }

    //file open
    fp = fopen(trace_file, "r");
    
    //file read
    while(fscanf(fp, " %c %lx,%d", &op, &address, &data_size) != -1) {
        mask = 0xffffffffffffffff << (b + s);
        tag = address & mask;
        mask = ~(0xffffffffffffffff << b);
        block_offset = address & mask;
        mask = tag | block_offset;
        set = (address ^ mask) >> b;
        tag >>= (b + s);

        switch(op) {
            case 'I':
                continue;
            case 'S':
                cache_line_match(cache_line[set], E, count, tag, &hit_count, &miss_count, &eviction_count);
                break;
            case 'L':
                cache_line_match(cache_line[set], E, count, tag, &hit_count, &miss_count, &eviction_count);
                break;
            case 'M':
                cache_line_match(cache_line[set], E, count, tag, &hit_count, &miss_count, &eviction_count);
                cache_line_match(cache_line[set], E, count, tag, &hit_count, &miss_count, &eviction_count);
                break;
        }
        count++;
    }

    //deallocate memory
    for (i = 0; i < set_num; i++) {
        free(cache_line[i]);
    }
    free(cache_line);

    //print Summary & file close
    printSummary(hit_count, miss_count, eviction_count);
    fclose(fp);

    return 0;
}

//function definition
int string_to_num(char* c) {
    int i = 0;
    int num = 0;
    while(c[i] != '\0') {
        num = num * 10 + (c[i] - 48);
        i++;
    }
    return num;
}

int power(int num, int exp) {
    int result = 1;
    while(exp > 0) {
        result *= num;
        exp -= 1;
    }
    return result;
}

void cache_line_match(Line* cache, int E, int count, unsigned long tag, int* hit_count, int* miss_count, int* eviction_count) {
    int min = cache[0].count, min_idx = 0;
    int full = 1;
    int i;
    for (i = 0; i < E; i++) {
        if(cache[i].valid && (cache[i].tag == tag)) { //match
            *hit_count += 1;
            cache[i].count = count;
            return;
        }
        if (!cache[i].valid) {
            full = 0;
        }
        if (min > cache[i].count) {
            min = cache[i].count;
            min_idx = i;
        }
    } 

    //no match
    *miss_count += 1;
    if (full) {
        cache[min_idx].valid = 1;
        cache[min_idx].tag = tag;
        cache[min_idx].count = count;
        *eviction_count += 1;
    } else {
        for (i = 0; i < E; i++) {
            if(!cache[i].valid) {
                cache[i].count = count;
                cache[i].valid = 1;
                cache[i].tag = tag;
                break;
            }
        }
    }
    return;
}