/* PDS2PNG 
  Converts color images stored in NASA's Planetary Data System format from .RED/.GRN/.BLU files to PNG files.

 By: Fredrik Hultin (noname@nurd.se)
 https://github.com/noname22/pds2png 
 License: See LICENSE (3-clause BSD)
*/

//#include <SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <png.h>

#define AssertMsg(_w, ...) if(!(_w)){ printf(__VA_ARGS__); puts(""); exit(1); }
#define AssertExp(_e) AssertMsg(_e, #_e " failed at " __FILE__ ":%d", __LINE__);

typedef struct {
	int w, h;
	uint8_t* pixels;	
} Image;

void WriteImage(Image* image, const char* filename)
{
	png_structp png_ptr;
	png_infop info_ptr;
	int x, y;
	png_bytep * row_pointers;
	
	/* create file */
	FILE *fp = fopen(filename, "wb");
	AssertMsg(fp, "could not open file for writing: %s", filename);

	AssertExp( row_pointers = (png_bytepp) malloc (image->h * sizeof (png_bytep)) );

	uint8_t* p = image->pixels;

	for (y = 0; y < image->h; y++) {
		row_pointers[y] = (png_bytep) malloc(image->w * sizeof(png_byte) * 3);

		for(x = 0; x < image->w; x++){
			row_pointers[y][x * 3] = *p++;
			row_pointers[y][x * 3 + 1] = *p++;
			row_pointers[y][x * 3 + 2] = *p++;
		}
	}

	// initialize stuff
	AssertExp( png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL) );
	AssertExp( info_ptr = png_create_info_struct(png_ptr) );
	AssertExp( !setjmp(png_jmpbuf(png_ptr)) );

	png_init_io(png_ptr, fp);

	// write header
	AssertExp( !setjmp(png_jmpbuf(png_ptr)) );

	png_set_IHDR(png_ptr, info_ptr, image->w, image->h,
		     8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	// write bytes
	AssertExp( (!setjmp(png_jmpbuf(png_ptr))) );

	png_write_image(png_ptr, row_pointers);

	// end write
	AssertExp( !setjmp(png_jmpbuf(png_ptr)) );

	png_write_end(png_ptr, NULL);

        fclose(fp);

	for (y=0; y<image->h; y++) free(row_pointers[y]);
	
	free(row_pointers);
}

Image* ReadImage(Image* image, const char* filename, int channel)
{
	//printf("loading channel %d from file %s\n", channel, filename);
	FILE* f = fopen(filename, "r");
	AssertMsg(f, "could not open file: %s", filename);
	int imageIndex = -1;
	bool inImageObj = false;
	int recordBytes = -1;

	if(!image) image = calloc(1, sizeof(Image));
	AssertMsg(image, "could not allocate image");

	// Read the header
	while(true){
		char type[512], val[512];
		fscanf(f, " %s = %s", type, val);

		if(inImageObj){
			if(!strcmp(type, "END_OBJECT") && !strcmp(val, "IMAGE")){
				inImageObj = false;
			}
			
			else if(!strcmp(type, "LINES")){
				if(image->h){
					AssertMsg(image->h == atoi(val), "unexpected image height in file: %s", filename);
				} else
					image->h = atoi(val);
			}
			
			else if(!strcmp(type, "LINE_SAMPLES")){
				if(image->w) {
					AssertMsg(image->w == atoi(val), "unexpected image width in file: %s", filename);
				} else
					image->w = atoi(val);
			}
		}else{	
			if(!strcmp(type, "END")){
				break;
			}
			
			else if(!strcmp(type, "RECORD_BYTES")){
				recordBytes = atoi(val);
			}

			else if(!strcmp(type, "^IMAGE")){
				//printf("value: '%s'\n", val);
				imageIndex = atoi(val);
				//printf("index: %d\n", imageIndex);
			}

			else if(!strcmp(type, "OBJECT") && !strcmp(val, "IMAGE")){
				inImageObj = true;
			}
		}
	}

	AssertMsg(imageIndex != -1, "could not find a pointer to image data");
	AssertMsg(recordBytes != -1, "could not determine record size");

	//printf("image: %d x %d @ %d\n", image->w, image->h, imageIndex * recordBytes);

	// Seek to the image data
	fseek(f, imageIndex * recordBytes, SEEK_SET);

	if(!image->pixels)
		AssertMsg( image->pixels = calloc(1, image->w * image->h * 3), "could not allocate pixel data" );

	//printf("loading channel %d\n", channel);

	for(int i = 0; i < image->w * image->h; i++){
		image->pixels[i * 3 + channel] = fgetc(f);
	}

	return image;
}

int main(int argc, const char* const* argv)
{
	AssertMsg(argc == 5, "usage: %s [RED file] [BLU file] [GRN file] [png output]", argv[0]);

	Image* image = ReadImage(NULL, argv[1], 0);
	ReadImage(image, argv[2], 1);
	ReadImage(image, argv[3], 2);

	WriteImage(image, argv[4]);
/*
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface* screen = SDL_SetVideoMode(1024, 768, 0, SDL_SWSURFACE);

	SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE, image->w, image->h, 24,
		0xff, 0xff00, 0xff0000, 0);

	memcpy(surface->pixels, image->pixels, image->w * image->h * 3);

	bool done = false;
	
	SDL_BlitSurface(surface, NULL, screen, NULL);
	SDL_Flip(screen);

	while(!done){
		SDL_Event event;
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
				done = true;
		}
		SDL_Delay(16);
	}

	SDL_Quit();*/

	return 0;
}
