#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>

#define NORMAL_WATCH_MODE	0
#define STOP_WATCH_MODE		1
#define CHANGE_TIME_MODE	2
#define CHANGE_DATE_MODE	3

#define CHANGE_HR_FORMAT	1
#define CHANGE_SEC		2
#define CHANGE_MINS		3
#define CHANGE_HOURS		0

#define CHANGE_MDATE		1
#define CHANGE_MONTH		2
#define CHANGE_YEAR		3
#define CHANGE_DAY		0

#define NO_OF_MODES 		4

int sec;
int mins;
int hours;
int m_sec;

bool in_change_time_mode = false;
bool blink_hr_format = false;
bool blink_sec = false;
bool blink_mins = false;
bool blink_hours = false;

int change_set = 0;

bool in_change_date_mode = false;
bool blink_mdate = false;
bool blink_month = false;
bool blink_year = false;
bool blink_day = false;

int change_set_1 = 0;

static char months[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
static char days[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();

bool loadMedia();

static void close();

SDL_Window* watch_window = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font *time_font = NULL;

TTF_Font *date_font = NULL;

TTF_Font *instructions_font = NULL;
SDL_Texture* hours_texture = NULL;
SDL_Texture* mins_texture = NULL;
SDL_Texture* sec_texture = NULL;
SDL_Texture* hr_format_texture = NULL;
SDL_Texture* instructions_texture = NULL;
SDL_Texture* mdate_texture = NULL;
SDL_Texture* month_texture = NULL;
SDL_Texture* year_texture = NULL;
SDL_Texture* day_texture = NULL;

int hours_width = 0;
int hours_height = 0;
int mins_width = 0;
int mins_height = 0;
int sec_width = 0;
int sec_height = 0;
int hr_format_width = 0;
int hr_format_height = 0;

int instructions_width = 0;
int instructions_height = 0;
int mdate_width = 0;
int mdate_height = 0;

int month_width = 0;
int month_height = 0;

int year_width = 0;
int year_height = 0;

int day_width = 0;
int day_height = 0;

bool hr_format = true;

SDL_Texture* loadTexture( const char* path )
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
                return NULL;
	}

	//Color key image
	SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

	//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
	if( newTexture == NULL )
	{
		printf( "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError() );
                return NULL;
	}

	//Get image dimensions
	hours_width = loadedSurface->w;
	hours_height = loadedSurface->h;
	mins_width = loadedSurface->w;
	mins_height = loadedSurface->h;
	sec_width = loadedSurface->w;
	sec_height = loadedSurface->h;
	
	mdate_width = loadedSurface->w;
	mdate_height = loadedSurface->h;
	
	month_width = loadedSurface->w;
	month_height = loadedSurface->h;
	
	year_width = loadedSurface->w;
	year_height = loadedSurface->h;
	
	day_width = loadedSurface->w;
	day_height = loadedSurface->h;
	
	instructions_width = loadedSurface->w;
	instructions_height = loadedSurface->h;
	if(hr_format)
	{
		hr_format_width = loadedSurface->w;
		hr_format_height = loadedSurface->h;
	}

	//Get rid of old loaded surface
	SDL_FreeSurface( loadedSurface );

	//Return success
	return newTexture;
}

void render( int x, int y)
{
        // C lacks default params, lazy workaround
        SDL_Rect* clip=NULL;
        double angle = 0.0;
        SDL_Point* center = NULL;
        SDL_RendererFlip flip = SDL_FLIP_NONE;

	//Set rendering space and render to screen
	SDL_Rect renderQuad_hours = { x, y, hours_width, hours_height};
	SDL_Rect renderQuad_mins = { x, y, mins_width, mins_height };
	SDL_Rect renderQuad_sec = { x, y, sec_width, sec_height };

	SDL_Rect renderQuad_instructions = { x, y, instructions_width, instructions_height };
	SDL_Rect renderQuad_mdate = { x, y, mdate_width, mdate_height };
	SDL_Rect renderQuad_month = { x, y, month_width, month_height };
	SDL_Rect renderQuad_year = { x, y, year_width, year_height };
	SDL_Rect renderQuad_day = { x, y, day_width, day_height };
//	if (hr_format)
	SDL_Rect renderQuad_hr_format = { x, y, hr_format_width, hr_format_height };
		

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad_hours.w = clip->w;
		renderQuad_hours.h = clip->h;
		renderQuad_mins.w = clip->w;
		renderQuad_mins.h = clip->h;
		renderQuad_sec.w = clip->w;
		renderQuad_sec.h = clip->h;

		renderQuad_instructions.w = clip->w;
		renderQuad_instructions.h = clip->h;

		renderQuad_mdate.w = clip->w;
		renderQuad_mdate.h = clip->h;
		renderQuad_month.w = clip->w;
		renderQuad_month.h = clip->h;
		renderQuad_year.w = clip->w;
		renderQuad_year.h = clip->h;
		renderQuad_day.w = clip->w;
		renderQuad_day.h = clip->h;
		if(hr_format)
		{
			renderQuad_hr_format.w = clip->w;
			renderQuad_hr_format.h = clip->h;
		}
	}
	
	renderQuad_mins.x = x + 100;
	renderQuad_mins.y = y;
	renderQuad_sec.x = x + 200;
	renderQuad_sec.y = y;

	renderQuad_instructions.x = 0;
	renderQuad_instructions.y = SCREEN_HEIGHT - 30;
	renderQuad_mdate.x = x + 50;
	renderQuad_mdate.y = y + 100;
	
	renderQuad_month.x = x + 100;
	renderQuad_month.y = y + 100;
	
	renderQuad_year.x = x + 180;
	renderQuad_year.y = y + 100;
	
	renderQuad_day.x = x + 270;
	renderQuad_day.y = y + 100;
	if(hr_format)
	{
		renderQuad_hr_format.x = x + 300;
		renderQuad_hr_format.y = y;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, hours_texture, clip, &renderQuad_hours, angle, center, flip );
	SDL_RenderCopyEx( gRenderer, mins_texture, clip, &renderQuad_mins, angle, center, flip );
	SDL_RenderCopyEx( gRenderer, sec_texture, clip, &renderQuad_sec, angle, center, flip );
	SDL_RenderCopyEx( gRenderer, instructions_texture, clip, &renderQuad_instructions, angle, center, flip );
	SDL_RenderCopyEx( gRenderer, mdate_texture, clip, &renderQuad_mdate, angle, center, flip );
	SDL_RenderCopyEx( gRenderer, month_texture, clip, &renderQuad_month, angle, center, flip );
	SDL_RenderCopyEx( gRenderer, year_texture, clip, &renderQuad_year, angle, center, flip );
	SDL_RenderCopyEx( gRenderer, day_texture, clip, &renderQuad_day, angle, center, flip );
	if(hr_format)
		SDL_RenderCopyEx( gRenderer, hr_format_texture, clip, &renderQuad_hr_format, angle, center, flip );
}

bool loadFromRenderedText( const char* time_str, const char* date_str, SDL_Color textColor )
{
	//Render text surface
	char hours_str[3] = {'\0','\0','\0'};
	char mins_str[3] = {'\0','\0','\0'};
	char sec_str[3] = {'\0','\0','\0'};
	char hr_format_str[3] = {'\0','\0','\0'};
	char mdate_str[3] = {'\0','\0','\0'};
	char month_str[4] = {'\0','\0','\0', '\0'};
	char year_str[5] = {'\0','\0','\0', '\0','\0'};
	char day_str[4] = {'\0','\0','\0','\0'};
	int i;
	int j = 0;
	SDL_Color textColor_white = { 255, 255, 255 };
	
	for (i = 0; i < 2; i++)
		hours_str[i] = time_str[j++];
	j++;
	for (i = 0; i < 2; i++)
		mins_str[i] = time_str[j++];
	j++;
	for (i = 0; i < 2; i++)
		sec_str[i] = time_str[j++];
	j++;

	if(hr_format)
	{
		for (i = 0; i < 2; i++)
			hr_format_str[i] = time_str[j++];
	}
	j = 0;
	for (i = 0; i < 2; i++)
		mdate_str[i] = date_str[j++];
	j++;
	for (i = 0; i < 3; i++)
		month_str[i] = date_str[j++];
	j++;
	for (i = 0; i < 4; i++)
		year_str[i] = date_str[j++];
	j++;
	for (i = 0; i < 3; i++)
		day_str[i] = date_str[j++];
	
	SDL_Surface* hours_surface = TTF_RenderText_Solid( time_font, hours_str, textColor );
	if(in_change_time_mode && blink_hours)
	{
		if(m_sec < 30)
			hours_surface = TTF_RenderText_Solid( time_font, hours_str, textColor_white );
		else	
			hours_surface = TTF_RenderText_Solid( time_font, hours_str, textColor );
	}
	SDL_Surface* mins_surface = TTF_RenderText_Solid( time_font, mins_str, textColor );
	if(in_change_time_mode && blink_mins)
	{
		if(m_sec < 30)
			mins_surface = TTF_RenderText_Solid( time_font, mins_str, textColor_white );
		else	
			mins_surface = TTF_RenderText_Solid( time_font, mins_str, textColor );
	}
	SDL_Surface* sec_surface = TTF_RenderText_Solid( time_font, sec_str, textColor );
	if(in_change_time_mode && blink_sec)
	{
		if(m_sec < 30)
			sec_surface = TTF_RenderText_Solid( time_font, sec_str, textColor_white );
		else	
			sec_surface = TTF_RenderText_Solid( time_font, sec_str, textColor );
	}
	SDL_Surface* hr_format_surface = TTF_RenderText_Solid( time_font, hr_format_str, textColor);
	if(in_change_time_mode && blink_hr_format)
	{
		if(m_sec < 30)
			hr_format_surface = TTF_RenderText_Solid( time_font, hr_format_str, textColor_white );
		else	
			hr_format_surface = TTF_RenderText_Solid( time_font, hr_format_str, textColor );
	}
	SDL_Surface* mdate_surface = TTF_RenderText_Solid( date_font, mdate_str, textColor );
	if(in_change_date_mode && blink_mdate)
	{
		if(m_sec < 30)
			mdate_surface = TTF_RenderText_Solid( date_font, mdate_str, textColor_white );
		else	
			mdate_surface = TTF_RenderText_Solid( date_font, mdate_str, textColor );
	}
	SDL_Surface* month_surface = TTF_RenderText_Solid( date_font, month_str, textColor );
	if(in_change_date_mode && blink_month)
	{
		if(m_sec < 30)
			month_surface = TTF_RenderText_Solid( date_font, month_str, textColor_white );
		else	
			month_surface = TTF_RenderText_Solid( date_font, month_str, textColor );
	}
	SDL_Surface* year_surface = TTF_RenderText_Solid( date_font, year_str, textColor );
	if(in_change_date_mode && blink_year)
	{
		if(m_sec < 30)
			year_surface = TTF_RenderText_Solid( date_font, year_str, textColor_white );
		else	
			year_surface = TTF_RenderText_Solid( date_font, year_str, textColor );
	}
	SDL_Surface* day_surface = TTF_RenderText_Solid( date_font, day_str, textColor );
	if(in_change_date_mode && blink_day)
	{
		if(m_sec < 30)
			day_surface = TTF_RenderText_Solid( date_font, day_str, textColor_white );
		else	
			day_surface = TTF_RenderText_Solid( date_font, day_str, textColor );
	}
	SDL_Surface* instructions_surface = TTF_RenderText_Solid( instructions_font, "RIGHT - next mode      UP - START/STOP/MODIFY      DOWN - RESET/MOVE", textColor);

	if( hours_surface == NULL || mins_surface == NULL || sec_surface == NULL || mdate_surface == NULL)
	{
		printf( "Unable to render time or date surface! SDL_ttf Error: %s\n", TTF_GetError() );
		return false;
	}

	//Create texture from surface pixels
	hours_texture = SDL_CreateTextureFromSurface( gRenderer, hours_surface );
	mins_texture = SDL_CreateTextureFromSurface( gRenderer, mins_surface );
	sec_texture = SDL_CreateTextureFromSurface( gRenderer, sec_surface );
	hr_format_texture = SDL_CreateTextureFromSurface( gRenderer, hr_format_surface );
	mdate_texture = SDL_CreateTextureFromSurface( gRenderer, mdate_surface );
	month_texture = SDL_CreateTextureFromSurface( gRenderer, month_surface );
	year_texture = SDL_CreateTextureFromSurface( gRenderer, year_surface );
	day_texture = SDL_CreateTextureFromSurface( gRenderer, day_surface );
	instructions_texture = SDL_CreateTextureFromSurface( gRenderer, instructions_surface );

	if( hours_texture  == NULL || mins_texture == NULL || sec_texture == NULL || mdate_texture == NULL)
	{
		printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	//Get image dimensions
	hours_width = hours_surface->w;
	hours_height = hours_surface->h;

	mins_width = mins_surface->w;
	mins_height = mins_surface->h;
	
	sec_width = sec_surface->w;
	sec_height = sec_surface->h;
	if(hr_format)
	{
		hr_format_width = hr_format_surface->w;
		hr_format_height = hr_format_surface->h;
	}
	
	
	instructions_width = instructions_surface->w;
	instructions_height = instructions_surface->h;

	mdate_width = mdate_surface->w;
	mdate_height = mdate_surface->h;

	month_width = month_surface->w;
	month_height = month_surface->h;
	
	year_width = year_surface->w;
	year_height = year_surface->h;
	
	day_width = day_surface->w;
	day_height = day_surface->h;
	//Get rid of old surface
	SDL_FreeSurface( hours_surface );
	SDL_FreeSurface( mins_surface );
	SDL_FreeSurface( sec_surface );
	
	SDL_FreeSurface( instructions_surface );
	SDL_FreeSurface( mdate_surface );
	SDL_FreeSurface( month_surface );
	SDL_FreeSurface( year_surface );
	SDL_FreeSurface( day_surface );
	if(hr_format)
		SDL_FreeSurface( hr_format_surface );

	//Return success
	return true;
}


bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
                return false;
	}

	//Set texture filtering to linear
	if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
	{
		printf( "Warning: Linear texture filtering not enabled!" );
	}

	//Create window
	watch_window = SDL_CreateWindow( "MY WATCH", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if( watch_window == NULL )
	{
		printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
                return false;
	}

	//Create vsynced renderer for window
	gRenderer = SDL_CreateRenderer( watch_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	if( gRenderer == NULL )
	{
	      printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
              return false;
	}

	//Initialize renderer color
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if( !( IMG_Init( imgFlags ) & imgFlags ) )
	{
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                return false;
	}

	  //Initialize SDL_ttf
	if( TTF_Init() == -1 )
	{
		printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                return false;
	}

        return true;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Open the font
	time_font = TTF_OpenFont( "DS-DIGIT.TTF", 80 );
	date_font = TTF_OpenFont( "DS-DIGIT.TTF", 40);
	instructions_font = TTF_OpenFont( "DS-DIGIT.TTF", 20);
	if( time_font == NULL || date_font == NULL)
	{
		printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
                return false;
	}

	//Render text
	SDL_Color textColor = { 0, 0, 0 };
	time_t now;
	struct tm tm;
	char time_str[20];
	char date_str[50];
	now = time(NULL);
	tm = *localtime(&now);
	sprintf(time_str, "%2d:%2d:%2d", tm.tm_hour, tm.tm_min, tm.tm_sec);
	sprintf(date_str, "%2d:%2d:%d:%2d", tm.tm_mday, tm.tm_mon, tm.tm_year + 1900, tm.tm_wday);
	
	if( !loadFromRenderedText( time_str, date_str, textColor ) )
	{
		printf( "Failed to render text texture!\n" );
                return false;
	}

	return true;
}

static void close(void)
{
	//Free loaded images
	free(hours_texture);
	free(mins_texture);
	free(sec_texture);
	free(hr_format_texture);
	free(mdate_texture);
	free(month_texture);
	free(year_texture);
	free(day_texture);

	//Free global font
	TTF_CloseFont( time_font );
	time_font = NULL;

	//Destroy window	
	//SDL_DestroyRenderer( gRenderer );
	printf("inside event\n");
	SDL_DestroyWindow( watch_window );
	watch_window = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void get_time(int *hours, int *mins, int *seconds, int *date, int *month, int *year, int *day)
{
	time_t now;
	struct tm tm;

	now = time(NULL);
	tm = *localtime(&now);

	*hours = tm.tm_hour;
	*mins = tm.tm_min;
	*seconds = tm.tm_sec;
	
	*date = tm.tm_mday;
	*month = tm.tm_mon;
	*year = tm.tm_year + 1900;
	*day = tm.tm_wday;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
		return 1;
	}

	//Load media
	if( !loadMedia() )
	{
		printf( "Failed to load media!\n" );
		return 1;
	}

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//While application is running
	SDL_Color textColor_black = { 0, 0, 0 };
	int date;
	int month;
	int year;
	int day;

	int sw_hours = 0;
	int sw_mins = 0;
	int sw_sec = 0;
	int sw_msec = 0;

	int c_sec = 0;
	int c_mins = 0;
	int c_hours = 0;
	int c_date = 0;
	int c_month = 0;
	int c_year = 0;
	int c_day = 0;
	
	char time_str[20];
	char stop_watch_str[20];
	char date_str[50];
	get_time(&hours, &mins, &sec, &date, &month, &year, &day);
    	struct timeb tp;
    	ftime(&tp);
    	m_sec = tp.millitm % 60;
	bool in_sw_mode = false;
	bool start_sw = false;
	int mode = 0;
	int s_hours = 0;
	int s_mins = 0;
	int s_sec = 0;
	while( !quit )
	{
		mode = mode % NO_OF_MODES;
		change_set = change_set % 4;
		change_set_1 = change_set_1 % 4;
		s_hours = (hours + c_hours) % 24;
		s_mins = (mins + c_mins) % 60;
		s_sec = (sec + c_sec) % 60;
		
		sprintf(time_str, "%2d:%2d:%2d", s_hours, s_mins, s_sec);
		
		if (hr_format)
		{
			if (s_hours == 12)
				sprintf(time_str, "%2d:%2d:%2d %s", 12, s_mins, s_sec, "PM");
			else if (s_hours > 12)
				sprintf(time_str, "%2d:%2d:%2d %s", (s_hours % 12), s_mins, s_sec, "PM");
			else if (s_hours == 0)
				sprintf(time_str, "%2d:%2d:%2d %s", 12, s_mins, s_sec, "AM");
			else
				sprintf(time_str, "%2d:%2d:%2d %s", s_hours, s_mins, s_sec, "AM");
		}
		
		sprintf(date_str, "%2d:%s:%d:%s", date, months[month], year, days[day]);
		loadFromRenderedText( time_str, date_str, textColor_black );
		if (in_sw_mode)
		{
			sprintf(stop_watch_str, "%2d:%2d:%2d:%2d", sw_hours, sw_mins, sw_sec, sw_msec);
			loadFromRenderedText( stop_watch_str, date_str, textColor_black );
		}
		//Handle events on queue
		while ( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit
			if( e.type == SDL_QUIT )
			{
				quit = true;
				break;
			}
			//User presses a key
			else if( e.type == SDL_KEYDOWN )
			{
				//Select surfaces based on key press
				switch( e.key.keysym.sym )
				{
					case SDLK_RIGHT:
						if(mode == NORMAL_WATCH_MODE)
						{
							blink_hr_format = false;
							blink_hours = false;
							blink_mins = false;
							blink_sec = false;
							blink_mdate = false;
							blink_month = false;
							blink_year = false;
							blink_day = false;
							change_set = 0;
							change_set_1 = 0;
							in_sw_mode = true;
							printf("stop watch mode\n");
							mode++;
						}
						else if(mode == STOP_WATCH_MODE)
						{
							in_sw_mode = false;
							in_change_time_mode = true;
							blink_hr_format = true;
							change_set++;
							mode++;
						}
						else if(mode == CHANGE_TIME_MODE)
						{
							in_change_time_mode = false;
							in_change_date_mode = true;
							blink_mdate = true;
							change_set_1++;
							mode++;
						}
						else if(mode == CHANGE_DATE_MODE)
						{
							in_change_date_mode = false;
							mode++;
						}
					break;
					case SDLK_UP:
						if(in_sw_mode && !start_sw)
						{
							start_sw = true;
						}
						else if (in_sw_mode && start_sw)
						{
							printf("stopped\n");
							start_sw = false;
						}
						else if(in_change_time_mode && change_set == CHANGE_HR_FORMAT)
						{
							hr_format = !hr_format;
						}
						else if(in_change_time_mode && change_set == CHANGE_SEC)
						{
							c_sec++;
						}
						else if(in_change_time_mode && change_set == CHANGE_MINS)
						{
							c_mins++;
						}
						else if(in_change_time_mode && change_set == 0)
						{
							c_hours++;
						}
					break;
					case SDLK_DOWN:
						if (in_sw_mode)
						{
							start_sw = false;
							sw_hours = 0;
							sw_mins = 0;
							sw_sec = 0;
							sw_msec = 0;
						}
						else if(in_change_time_mode && change_set == CHANGE_HOURS)
						{
							blink_hours = false;
							blink_hr_format = true;
							change_set++;
						}
						else if(in_change_time_mode && change_set == CHANGE_HR_FORMAT)
						{
							blink_hr_format = false;
							blink_sec = true;
							change_set++;
						}
						else if(in_change_time_mode && change_set == CHANGE_SEC)
						{
							blink_sec = false;
							blink_mins = true;
							change_set++;
						}
						else if(in_change_time_mode && change_set == CHANGE_MINS)
						{
							blink_mins = false;
							blink_hours = true;
							change_set++;
						}
						else if(in_change_date_mode && change_set_1 == CHANGE_DAY)
						{
							blink_day = false;
							blink_mdate = true;
							change_set_1++;
						}
						else if(in_change_date_mode && change_set_1 == CHANGE_MDATE)
						{
							blink_mdate = false;
							blink_month = true;
							change_set_1++;
						}
						else if(in_change_date_mode && change_set_1 == CHANGE_MONTH)
						{
							blink_month = false;
							blink_year = true;
							change_set_1++;
						}
						else if(in_change_date_mode && change_set_1 == CHANGE_YEAR)
						{
							blink_year = false;
							blink_day = true;
							change_set_1++;
						}
					break;
					case SDLK_h:
						hr_format = !hr_format;
					break;
					case SDLK_r:
						c_hours = 0;
						c_mins = 0;
						c_sec = 0;
					break;
					case SDLK_q:
						quit = true;
				}
			}	
		}
		//Clear screen
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( gRenderer );

		//Render current frame
		render( (SCREEN_WIDTH - 400)/ 2, (SCREEN_HEIGHT - 150) / 2 );
		SDL_FlushEvents(SDL_USEREVENT, SDL_LASTEVENT);
		//Update screen
		SDL_RenderPresent( gRenderer );
		m_sec++;
		if(m_sec == 60)
		{
			m_sec = 0;
			sec++;
		}
		if(sec == 60)
		{
			sec = 0;
			mins++;
		}
		if(mins == 60)
		{
			mins = 0;
			hours++;
		}
		if ((c_sec != 0) && (c_sec % 60) == 0)
			c_mins++;
		if ((c_mins != 0) && (c_mins % 60) == 0)
			c_hours++;
		if(start_sw)
		{
			sw_msec++;
			if(sw_msec == 60)
			{
				sw_msec = 0;
				sw_sec++;
			}
			if(sw_sec == 60)
			{
				sw_sec = 0;
				sw_mins++;
			}
			if(sw_mins == 60)
			{
				sw_mins = 0;
				sw_hours++;
			}
		}
		SDL_Delay(1);
	}

	//Free resources and close SDL
	close();

	return 0;
}

