// /*

// Customized Ray Tracer for BareMetal OS

// Based on Andrew Kensler's business card sized ray tracer
// Breakdown by Fabien Sanglard here: https://fabiensanglard.net/rayTracing_back_of_business_card/

// - Converted from C++ to C
// - Uses built-in functions for pow, sqrt, ceil, and rand

// Upon execution it will render the image directly to the screen
// The first run will use only one CPU core
// The second run will use all available CPU cores

// */

// #include "libBareMetal.h"

// #define R1 1103515245
// #define R2 12345
// #define R3 2147483648 // 2^31
// #define RAND_MAX 32767

// typedef int i;
// typedef float f;
// u8 *frame_buffer, *cpu_table;
// u16 X, Y;
// u32 progress = 0;
// u64 next = 1; // For rand()
// u64 lock = 0, TOTALCORES = 0, BSP;

// // Custom pow
// double bpow(double x, double y) {
// 	double result = 1.0;
// 	i neg_exp_flag = 0;

// 	if (y < 0) {
// 		neg_exp_flag = 1;
// 		y = -y;
// 	}

// 	for (i c=0; c < y; c++)
// 		result *= x;

// 	if (neg_exp_flag)
// 		result = 1.0 / result;

// 	return result;
// }

// // Custom sqrt
// double bsqrt(double num) {
// 	if (num < 0)
// 		return -1;

// 	double e = 0.00001;
// 	double guess = num / 2.0;

// 	while (1) {
// 		double new_guess = 0.5 * (guess + num / guess);

// 		if (guess - new_guess < e && guess - new_guess > -e)
// 			break;

// 		guess = new_guess;
// 	}

// 	return guess;
// }

// // Custom ceil
// double bceil(double num) {
// 	double part = (i)num;
// 	if (num > part)
// 		return part + 1;
// 	else
// 		return part;
// }

// // Custom rand
// uint64_t rand() {
// 	next = (R1 * next + R2) % R3;
// 	return next % (RAND_MAX + 1);
// }

// // Vector Structure
// typedef struct {
// 	f x, y, z;
// } vector;

// // Vector Add
// vector v_add(vector a, vector b) {
// 	vector result = {a.x + b.x, a.y + b.y, a.z + b.z};
// 	return result;
// }

// // Vector Multiply/Scale
// vector v_mul(vector a, f b) {
// 	vector result = { a.x * b, a.y * b, a.z * b };
// 	return result;
// }

// // Vector Dot Product
// f v_dot(vector a, vector b) {
// 	return a.x * b.x + a.y * b.y + a.z * b.z;
// }

// // Vector Cross-Product
// vector v_cross(vector a, vector b) {
// 	vector result = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
// 	return result;
// }

// // Vector Normalize
// vector v_norm(vector a) {
// 	f mag = bsqrt(v_dot(a, a));
// 	return v_mul(a, 1 / mag);
// }

// // Vector Initialization
// vector v_init(f a, f b, f c) {
// 	vector result = {a, b, c};
// 	return result;
// }

// // Array of spheres (displaying 'Hi!')
// i G[] = {280336, 279040, 279040, 279056, 509456, 278544, 278544, 279056, 278544};

// // Random generator, return a float within range [0-1]
// f R() {
// 	return (f)rand() / RAND_MAX;
// }

// // Tracer
// // Return 0 if no hit was found but ray goes up
// // Return 1 if no hit was found but ray goes down
// // Return 2 if a hit was found (and also return distance t and bouncing ray n)
// i T(vector o, vector d, f *t, vector *n) {
// 	*t = 1e9;
// 	i m = 0;
// 	f p = -o.z / d.z;
// 	if (.01 < p) {
// 		*t = p;
// 		*n = v_init(0, 0, 1);
// 		m = 1;
// 	}

// 	for (i k = 19; k--;)
// 		for (i j = 9; j--;)
// 			if (G[j] & 1 << k) {
// 				vector p = v_add(o, v_init(-k, 0, -j - 4));
// 				f b = v_dot(p, d), c = v_dot(p, p) - 1, q = b * b - c;
// 				if (q > 0) {
// 					f s = -b - bsqrt(q);
// 					if (s < *t && s > .01) {
// 						*t = s;
// 						*n = v_norm(v_add(p, v_mul(d, *t)));
// 						m = 2;
// 					}
// 				}
// 			}

// 	return m;
// }

// // Sampler
// // Return the pixel color for a ray passing by point o (origin) and d (direction)
// vector S(vector o, vector d) {
// 	f t;
// 	vector n;
// 	i m = T(o, d, &t, &n);

// 	// Generate a sky color if no sphere is hit and the ray goes up
// 	if (!m)
// 		return v_mul(v_init(.7, .6, 1), bpow(1 - d.z, 4));

// 	vector h = v_add(o, v_mul(d, t));
// 	vector l = v_norm(v_add(v_init(9 + R(), 9 + R(), 16), v_mul(h, -1)));
// 	vector r = v_add(d, v_mul(n, v_dot(n, d) * -2));
// 	f b = v_dot(l, n);
// 	if (b < 0 || T(h, l, &t, &n))
// 		b = 0;

// 	f p = bpow(v_dot(l, r) * (b > 0), 99);

// 	// Generate a floor color if no sphere is hit and the ray goes down
// 	if (m & 1) {
// 		h = v_mul(h, .2);
// 		return v_mul(((i)(bceil(h.x) + bceil(h.y)) & 1) ? v_init(3, 1, 1) : v_init(3, 3, 3), b * .2 + .1);
// 	}

// 	// A sphere was hit.
// 	return v_add(v_init(p, p, p), v_mul(S(h, r), .5));
// }

// int render()
// {
// 	b_system(SMP_LOCK, lock, 0);
// 	int y = progress++; // Starting line number
// 	b_system(SMP_UNLOCK, lock, 0);
// 	vector g = v_norm(v_init(5, -28, 7)); // Camera direction (-/+ = Right/Left, ?/? , Down/Up)
// 	vector a = v_mul(v_norm(v_cross(v_init(0, 0, -1), g)), .002); // Camera up vector
// 	vector b = v_mul(v_norm(v_cross(g, a)), .002);
// 	vector c = v_add(v_add(v_mul(a, -256), v_mul(b, -256)), g);

// 	for (; y<Y; y+=TOTALCORES) // Increment by the number of CPUs
// 		for (i x = 0; x<X; x++) {
// 			int offset = (y * X + x) * 4; // Calculate the offset into video memory for this line

// 			vector p = v_init(13, 13, 13); // Reuse the vector class to store the RGB values of a pixel
// 			for (i r = 64; r--;) {
// 				vector t = v_add(v_mul(a, (R() - .5) * 99), v_mul(b, (R() - .5) * 99));
// 				p = v_add(v_mul(S(v_add(v_init(17, 16, 8), t), v_norm(v_add(v_mul(t, -1), v_mul(v_add(v_add(v_mul(a, R() + x), v_mul(b, y + R())), c), 16)))), 3.5), p);
// 			}

// 			frame_buffer[offset++] = (i)p.z; // Output RGB values directly to video memory
// 			frame_buffer[offset++] = (i)p.y;
// 			frame_buffer[offset++] = (i)p.x;
// 			frame_buffer[offset++] = 0;
// 		}
// }

// void cls()
// {
// 	u8 pixel = 0x40;
// 	for (int bytes = 0; bytes < (X * Y * 4); bytes++)
// 		frame_buffer[bytes] = pixel;
// }

// int main() {
// 	frame_buffer = (u8 *)b_system(SCREEN_LFB_GET, 0, 0); // Frame buffer address from kernel
// 	cpu_table = (u8 *)0x5100;
// 	X = b_system(SCREEN_X_GET, 0, 0); // Screen X
// 	Y = b_system(SCREEN_Y_GET, 0, 0); // Screen Y
// 	u8 tcore;
// 	u8 c;
// 	int busy;

// 	b_output("\nraytrace - First run will be using 1 CPU core\nPress any key to continue", 72);

// 	c = 0;
// 	do {
// 		c = b_input();
// 	} while (c == 0);

// 	cls();

// 	TOTALCORES = 1;
// 	render();

// 	TOTALCORES = b_system(SMP_NUMCORES, 0, 0); // Total cores in the system
// 	BSP = b_system(SMP_ID, 0, 0); // ID of the BSP

// 	b_output("\nRender complete. Second run will use all CPU cores\nPress any key to continue", 77);

// 	c = 0;
// 	do {
// 		c = b_input();
// 	} while (c == 0);

// 	cls();

// 	for (u8 t=0; t<TOTALCORES; t++)
// 	{
// 		tcore = cpu_table[t]; // Location of the Active CPU IDs
// 		if (tcore != BSP)
// 			b_system(SMP_SET, (u64)render, tcore); // Have each AP render
// 	}
// 	render(); // Have the BSP render as well

// 	// Wait for all other cores to be finished
// 	do {
// 		busy = b_system(SMP_BUSY, 0, 0);
// 	} while (busy == 1);

// 	b_output("\nRender complete. Press any key to exit", 39);

// 	c = 0;
// 	do {
// 		c = b_input();
// 	} while (c == 0);

// 	cls();

// 	return 0;
// }

// // EOF



// #define __USE_MISC

// /* Global Includes */
// #include "libBareMetal.h"

// /* Global functions */
// void initialize_filesystem(u64 metadata_start, u64 metadata_size);
// void create_file(const char *filename);
// int write_to_file(const char *filename, const char *content);
// void strncpy(char *dest, const char *src, u64 n);
// int strcmp(const char *str1, const char *str2);
// int strlen(const char* s);

// /* Global Defines */
// #define MAX_FILENAME_LEN 64

// /* Global Variables */
// u64 metadata_start = 1;
// u64 metadata_size = 8192;

// /* Global structs */
// #pragma pack(1)
// typedef struct FileMetadata{
//     char filename[MAX_FILENAME_LEN];  // File name
//     u64 start_block;                  // Starting block of the file
//     u64 size;                         // Size of the file in bytes
// } FileMetadata;

// void itoa(unsigned int value, char* str, int base) {
//     char* ptr = str;
//     char* end = str;
//     char digits[] = "0123456789ABCDEF";

//     do {
//         *end++ = digits[value % base];
//         value /= base;
//     } while (value);

//     *end-- = '\0';

//     while (ptr < end) {
//         char temp = *ptr;
//         *ptr++ = *end;
//         *end-- = temp;
//     }
// }


// /* Main Code */
// int main() {
//     initialize_filesystem(metadata_start, metadata_size);
//     // create_file("file1.txt");
//     FileMetadata meta;
//     char file[64]="file1.txt";
//     u64 starting=metadata_size+1;
//     u64 size=0;
//     u64 result = b_storage_write(file, metadata_start, sizeof(meta.filename), 1);
//     char res[2];
//     itoa(result,res,10);
//     b_output(res,2);
//     if(result=sizeof(meta.filename)){
//         b_output("File Created",(unsigned long)strlen("File Created"));
//     }
//     b_storage_write(&starting, metadata_start + sizeof(meta.filename), sizeof(meta.start_block), 1);
//     b_storage_write(&size, metadata_start + sizeof(meta.filename) + sizeof(meta.size), sizeof(meta.size), 1);

//     // if (create_file("file1.txt") == 0) {
//         b_output("Success",(unsigned long)strlen("Success"));
//         // create_file("file1.txt");
//         // write_to_file("file1.txt", "Hello World");
//     // }
//     return 0;
// }

// void initialize_filesystem(u64 metadata_start, u64 metadata_size) {
//     char empty_block = '\0';
//     for (u64 i = 0; i < metadata_size; i++) {
//         b_storage_write(&empty_block, metadata_start + i, 1, 1);
//     }
//     b_output("\nInitialization of File System Done.\n", 
//              (unsigned long)strlen("\nInitialization of File System Done.\n"));
// }

// void create_file(const char *filename) {
//     FileMetadata meta;
//     u64 p = 0;

//     for (u64 i=0; i < (metadata_size / sizeof(FileMetadata)); i++) {
//         b_storage_read(meta.filename, metadata_start + (i * sizeof(meta)), sizeof(meta.filename), 1);

//         if (meta.filename[0] == '\0') {
//             strncpy(meta.filename, filename, sizeof(filename));
//             meta.start_block = metadata_size + 1;
//             meta.size = 0;
//             b_storage_write(meta.filename, metadata_start + (i * sizeof(meta)), sizeof(meta.filename), 1);
//             b_storage_write(&meta.start_block, metadata_start + (i * sizeof(meta)) + sizeof(meta.filename), sizeof(meta.start_block), 1);
//             b_storage_write(&meta.size, metadata_start + (i * sizeof(meta)) + sizeof(meta.filename) + sizeof(meta.size), sizeof(meta.size), 1);

//             b_output("File created\n", (unsigned long)strlen("File created\n"));
//             p=1;
//             break;
//         } else if (strcmp(meta.filename, filename) == 0) {
//             b_output("File already exists\n", (unsigned long)strlen("File already exists\n"));
//             p=-1;
//             break;
//         }
//     }
//     if(p==0){
//         b_output("No space for new files\n", strlen("No space for new files\n"));
//     }
//     b_output("Debug1\n",(unsigned long)strlen("Debug1\n"));
// }

// int write_to_file(const char *filename, const char *content) {
//     FileMetadata meta;
//     u64 i = 0;

//     while (i < metadata_size / sizeof(FileMetadata)) {
//         b_storage_read(&meta, metadata_start + (i * sizeof(meta)), sizeof(meta), 1);

//         if (strcmp(meta.filename, filename) == 0) {
//             u64 content_length = strlen(content);
//             b_storage_write((void *)content, meta.start_block, content_length, 1);
//             meta.size = content_length;
//             b_storage_write(&meta, metadata_start + (i * sizeof(meta)), sizeof(meta), 1);

//             b_output("Content written to file\n", strlen("Content written to file\n"));
//             return 0;
//         }
//         i++;
//     }

//     b_output("File not found\n", strlen("File not found\n"));
//     return -1;
// }


// void strncpy(char *dest, const char *src, u64 n) {
//     u64 i;
//     for (i = 0; i < n - 1 && src[i] != '\0'; i++) { 
//         dest[i] = src[i];
//     }
//     dest[i] = '\0'; 
// }


// int strcmp(const char *str1, const char *str2) {
//     while (*str1 && (*str1 == *str2)) {
//         str1++;
//         str2++;
//     }
//     return *(const unsigned char *)str1 - *(const unsigned char *)str2;
// }

// int strlen(const char* s) {
//     int r = 0;
//     while (*s++) r++;
//     return r;
// }

#include "libBareMetal.h"


int strlen(const char* s)
{
	int r = 0;

	for(; *s++ != 0; r++) { }

	return r;
}

void itoa(unsigned int value, char* str, int base) {
    char* ptr = str;
    char* end = str;
    char digits[] = "0123456789ABCDEF";

    do {
        *end++ = digits[value % base];
        value /= base;
    } while (value);

    *end-- = '\0';

    while (ptr < end) {
        char temp = *ptr;
        *ptr++ = *end;
        *end-- = temp;
    }
}

void write_to_storage(void *mem, u64 start, u64 num, u64 drivenum) {
    u64 result = b_storage_write(mem, start, num, drivenum);
    if (result == num) {
        b_output("\nWrite operation successful.\n", (unsigned long)strlen("\nWrite operation successful.\n"));
    } else {
        b_output("\nWrite operation failed.\n", (unsigned long)strlen("\nWrite operation failed.\n"));
    }
}

void read_from_storage(void *mem, u64 start, u64 num, u64 drivenum) {
    u64 result = b_storage_read(mem, start, num, drivenum);
    if (result == num) {
        b_output("Read operation successful.\n", (unsigned long)strlen("Read operation successful.\n"));
    } else {
        b_output("Read operation failed.\n", (unsigned long)strlen("Read operation failed.\n"));
    }
}

int main() {
    char buffer[4096];

    // char data_to_write[] = "Hello, BareMetal OS Storage!Hello, BareMetal OS Storage!Hello, BareMetal OS Storage!Hello, BareMetal OS Storage!Hello, BareMetal OS Storage!Hello, BareMetal OS Storage!Hello, BareMetal OS Storage!";

    // for (int i = 0; i < sizeof(data_to_write); i++) {
    //     buffer[i] = data_to_write[i];
    // }

    // for(int j=0;j<10;j++){
    //     write_to_storage(buffer, j, 1, 0);
    // }
    b_output("\n", 1);
    u64 lba=b_system(NVME_LBA, 0 , 0);

    char lba_str[16];
    itoa(lba, lba_str, 2);
    b_output(lba_str, strlen(lba_str));
    b_output("\n", 1);

    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = 0;
    }
    read_from_storage(buffer, 0, 1, 0);
    b_output("Data read from storage: ", (unsigned long)strlen("Data read from storage: "));
    b_output(buffer, sizeof(buffer));
    read_from_storage(buffer, 1, 1, 0);
    b_output("Data read from storage: ", (unsigned long)strlen("Data read from storage: "));
    b_output(buffer, sizeof(buffer));  

    return 0;
}