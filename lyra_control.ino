#include "Arduino.h"
#include "Wire.h"
#include "FastLED.h"

#define LEDS_CONTROL_CPP

#define STAR_NUM			29//16
#define PIX_IN_STAR_NUM		4
#define PIX_NUM				PIX_IN_STAR_NUM * STAR_NUM
#define	COL_NUM				23//8
#define LED_DATA_PIN		3

#define LYRA_ENA_PIN			7
#define LOOPER_REC_PIN			9
#define LOOPER_ERASE_PIN		11
#define LOOPER_REC_HOLD_TIME	1000
#define LOOPER_ERASE_HOLD_TIME	6000
#define LOOPER_REC_TIME			30000

#define EFF_UPD_PERIOD			40
#define EFF_FLASH_RISE_DUR		100
#define EFF_FLASH_FALL_DUR		500
#define EFF_IDLE_COLOR			0, 3, 3
#define EFF_FLASH_COLOR1		0, 100, 100//100, 80, 0
#define EFF_FLASH_COLOR2		100, 80, 0
#define EFF_REC_COLOR			0, 50, 0
#define EFF_PLAY_COLOR			50, 0, 0

// Timers
//----------------------
enum
{
	SYSTEM_TIMER_UPD,
	SYSTEM_TIMER_LOOPER,
	SYSTEM_TIMER_REC,
	SYSTEM_TIMER_FADE,
	SYSTEM_TIMER_NUM
};
uint16_t 	SYSTEM_TimerArray[SYSTEM_TIMER_NUM];

//Exhibit States
//----------------------
enum
{
	EXH_STATE_INIT,
	EXH_STATE_IDLE,
	EXH_STATE_REC,
	EXH_STATE_PLAY,
	EXH_STATE_ERASE
};
uint8_t	_exh_state; 
uint8_t	_exh_proc;

//Effect states
//----------------------
enum
{
	EFF_STATE_IDLE,
	EFF_STATE_WAVE,
	EFF_STATE_REC,
	EFF_STATE_PLAY
};
uint8_t	_eff_state; 
uint8_t	_eff_proc; 

CRGB		_led_arr[PIX_NUM];

enum
{
	STAR_EFF_NO,
	STAR_EFF_FLASH
};
typedef struct
{
	uint8_t 	eff_state;
	uint16_t 	eff_tmr;
	uint8_t 	eff_steps_num;
	//uint16_t	rise_dur;
	//uint16_t	fall_dur;
	float		r_step;
	float		g_step;
	float		b_step;
	float		r_cur;
	float		g_cur;
	float		b_cur;
	uint8_t		r_tar;
	uint8_t		g_tar;
	uint8_t		b_tar;
	uint8_t		r_last;
	uint8_t		g_last;
	uint8_t		b_last;
} star_eff_struct_type;
star_eff_struct_type _star_arr[STAR_NUM];

typedef struct 
{
	uint8_t	st_num;
	uint8_t	stars[2];
} col_srtuct_type;
col_srtuct_type _col_arr[COL_NUM];

void setup() 
{
	//Reset all system timers
	memset( SYSTEM_TimerArray, 0, sizeof(SYSTEM_TimerArray) );
	
	pinMode(LYRA_ENA_PIN, OUTPUT);
	pinMode(LOOPER_REC_PIN, OUTPUT);
	pinMode(LOOPER_ERASE_PIN, OUTPUT);
	
	_exh_state = EXH_STATE_INIT;
	_exh_proc = 0;
	
	_eff_state = EFF_STATE_IDLE;
	_eff_proc = 0;
	
	memset( _star_arr, 0, sizeof(_star_arr) );
	
	Serial.begin(115200);
	
	randomSeed(analogRead(A1));
	
	//Init cols
	//#0
	_col_arr[0].st_num = 1;
	_col_arr[0].stars[0] = 0;	
	
	//#1
	_col_arr[1].st_num = 2;
	_col_arr[1].stars[0] = 1;
	_col_arr[1].stars[1] = 2;	

	//#2
	_col_arr[2].st_num = 2;
	_col_arr[2].stars[0] = 3;
	_col_arr[2].stars[1] = 4;	
	
	//#3
	_col_arr[3].st_num = 2;
	_col_arr[3].stars[0] = 5;
	_col_arr[3].stars[1] = 9;
	
	//#4
	_col_arr[4].st_num = 1;
	_col_arr[4].stars[0] = 6;
	
	//#5
	_col_arr[5].st_num = 1;
	_col_arr[5].stars[0] = 7;

	//#6
	_col_arr[6].st_num = 1;
	_col_arr[6].stars[0] = 8;
	
	//#7
	_col_arr[7].st_num = 1;
	_col_arr[7].stars[0] = 10;

	//#8
	_col_arr[8].st_num = 1;
	_col_arr[8].stars[0] = 11;

	//#9
	_col_arr[9].st_num = 1;
	_col_arr[9].stars[0] = 12;

	//#10
	_col_arr[10].st_num = 1;
	_col_arr[10].stars[0] = 13;

	//#11
	_col_arr[11].st_num = 1;
	_col_arr[11].stars[0] = 14;

	//#12
	_col_arr[12].st_num = 1;
	_col_arr[12].stars[0] = 19;

	//#13
	_col_arr[13].st_num = 2;
	_col_arr[13].stars[0] = 20;
	_col_arr[13].stars[1] = 15;
	
	//#14
	_col_arr[14].st_num = 2;
	_col_arr[14].stars[0] = 18;
	_col_arr[14].stars[1] = 17;
	
	//#15
	_col_arr[15].st_num = 1;
	_col_arr[15].stars[0] = 16;

	//#16
	_col_arr[16].st_num = 1;
	_col_arr[16].stars[0] = 21;

	//#17
	_col_arr[17].st_num = 1;
	_col_arr[17].stars[0] = 22;

	//#18
	_col_arr[18].st_num = 1;
	_col_arr[18].stars[0] = 23;

	//#19
	_col_arr[19].st_num = 1;
	_col_arr[19].stars[0] = 25;

	//#20
	_col_arr[20].st_num = 1;
	_col_arr[20].stars[0] = 24;

	//#21
	_col_arr[21].st_num = 1;
	_col_arr[21].stars[0] = 26;
	
	//#22
	_col_arr[22].st_num = 2;
	_col_arr[22].stars[0] = 27;
	_col_arr[22].stars[1] = 28;
	
	//TEST COLS INIT (MATRIX 8*8)
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//#0
	/*_col_arr[0].st_num = 2;
	_col_arr[0].stars[0] = 0;
	_col_arr[0].stars[1] = 1;	
	
	//#1
	_col_arr[1].st_num = 2;
	_col_arr[1].stars[0] = 2;
	_col_arr[1].stars[1] = 3;	

	//#2
	_col_arr[2].st_num = 2;
	_col_arr[2].stars[0] = 4;
	_col_arr[2].stars[1] = 5;	
	
	//#3
	_col_arr[3].st_num = 2;
	_col_arr[3].stars[0] = 6;
	_col_arr[3].stars[1] = 7;
	
	//#4
	_col_arr[4].st_num = 2;
	_col_arr[4].stars[0] = 8;
	_col_arr[4].stars[1] = 9;
	
	//#5
	_col_arr[5].st_num = 2;
	_col_arr[5].stars[0] = 10;
	_col_arr[5].stars[1] = 11;

	//#6
	_col_arr[6].st_num = 2;
	_col_arr[6].stars[0] = 12;
	_col_arr[6].stars[1] = 13;
	
	//#7
	_col_arr[7].st_num = 2;
	_col_arr[7].stars[0] = 14;
	_col_arr[7].stars[1] = 15;*/
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	
	FastLED.addLeds<NEOPIXEL, LED_DATA_PIN> (_led_arr, PIX_NUM);
	FastLED.setBrightness(255);
	FastLED.clear();
	FastLED.show();
	
	//Serial.println("Init done");

}

//
//-------------------------------------------------------------------------
void setStarColor(uint8_t t_star, uint8_t r_ch, uint8_t g_ch, uint8_t b_ch)
{
	uint8_t _star_addr, i;
	
	_star_addr = t_star * PIX_IN_STAR_NUM;
	if( (_star_addr + 3) > (PIX_NUM - 1) )
		return;
	for( i = 0; i < PIX_IN_STAR_NUM; i++ )
		_led_arr[_star_addr + i] = CRGB(r_ch, g_ch, b_ch);
	
	return;
}

//
//-------------------------------------------------------------------------
CRGB getStarColor(uint8_t t_star)
{
	return _led_arr[t_star * PIX_IN_STAR_NUM];
}

//
//-------------------------------------------------------------------------
void setColColor(uint8_t t_col, uint8_t r_ch, uint8_t g_ch, uint8_t b_ch)
{
	uint8_t i;
	
	if( t_col > (COL_NUM - 1) )
		return;
	
	for( i = 0; i < _col_arr[t_col].st_num; i++ )
		setStarColor(_col_arr[t_col].stars[i], r_ch, g_ch, b_ch);
}


//
//-------------------------------------------------------------------------
void runStarFlash(uint8_t t_star, uint8_t r_ch, uint8_t g_ch, uint8_t b_ch)
{
	if( t_star > (STAR_NUM - 1) )
		return;
	_star_arr[t_star].eff_state = 1;
	//_star_arr[t_star].rise_dur = t_rise_dur;
	//_star_arr[t_star].fall_dur = t_fall_dur;
	_star_arr[t_star].r_last = getStarColor(t_star).r;
	_star_arr[t_star].g_last = getStarColor(t_star).g;
	_star_arr[t_star].b_last = getStarColor(t_star).b;
	_star_arr[t_star].r_tar = r_ch;
	_star_arr[t_star].g_tar = g_ch;
	_star_arr[t_star].b_tar = b_ch;
	return;
}

//
//-------------------------------------------------------------------------
void runColFlash(uint32_t t_col, uint8_t r_ch, uint8_t g_ch, uint8_t b_ch)
{
	uint8_t i;
	
	if( t_col > (COL_NUM - 1) )
		return;
	for( i = 0; i < _col_arr[t_col].st_num; i++ )
		runStarFlash(_col_arr[t_col].stars[i], r_ch, g_ch, b_ch);
}

void loop() 
{
	uint32_t i = 0;
	static uint8_t cur_line = 0, rvd_data;
	static uint32_t last_ms_tmr_val = 0;
	
	//ms Timer Handler
    if( last_ms_tmr_val != millis() )
    {
        for( i = 0; i < SYSTEM_TIMER_NUM; i++ ) if( SYSTEM_TimerArray[i] > 0 ) 
			SYSTEM_TimerArray[i] = SYSTEM_TimerArray[i] - 1;
		//Star effect timers
		for( i = 0; i < STAR_NUM; i++ ) if( _star_arr[i].eff_tmr )
			_star_arr[i].eff_tmr--;
        last_ms_tmr_val = millis();
    }
	
	//Update LEDs
	if( !SYSTEM_TimerArray[SYSTEM_TIMER_UPD] )
	{
		SYSTEM_TimerArray[SYSTEM_TIMER_UPD] = 20;
		FastLED.show();
	}
	
	//EXHIBIT STATES
	//-------------------------------------------------
	//INIT state
	if( _exh_state == EXH_STATE_INIT )
	{
		//Enable LYRA
		digitalWrite(LYRA_ENA_PIN, 1);
		//REC off
		digitalWrite(LOOPER_REC_PIN, 0);
		//ERASE off
		digitalWrite(LOOPER_ERASE_PIN, 1);
		
		_exh_state = EXH_STATE_IDLE;
		_eff_state = EFF_STATE_IDLE;
	}
	
	//IDLE state
	else if( _exh_state == EXH_STATE_IDLE )
	{
		if( Serial.available() )
		{
			Serial.readBytes(&rvd_data, 1);
			switch( rvd_data )
			{
				//Show wave effect
				case 0x31:
					_eff_state = EFF_STATE_WAVE;
					_eff_proc = 0;
				break;
				//Start record 
				case 0x32:
					digitalWrite(LOOPER_REC_PIN, 1);
					SYSTEM_TimerArray[SYSTEM_TIMER_LOOPER] = LOOPER_REC_HOLD_TIME;
					SYSTEM_TimerArray[SYSTEM_TIMER_REC] = LOOPER_REC_TIME;
					_eff_state = EFF_STATE_REC;
					_eff_proc = 0;
					Serial.println(F("REC state"));
					_exh_state = EXH_STATE_REC;
				break;
				default:
				break;
			}
		}
	}
	
	//REC state
	else if( _exh_state == EXH_STATE_REC )
	{
		if( !SYSTEM_TimerArray[SYSTEM_TIMER_LOOPER] )
		{
			digitalWrite(LOOPER_REC_PIN, 0);
		}
		if( !SYSTEM_TimerArray[SYSTEM_TIMER_REC] )
		{
			digitalWrite(LOOPER_REC_PIN, 1);
			digitalWrite(LYRA_ENA_PIN, 0);
			SYSTEM_TimerArray[SYSTEM_TIMER_LOOPER] = LOOPER_REC_HOLD_TIME;
			SYSTEM_TimerArray[SYSTEM_TIMER_REC] = LOOPER_REC_TIME;
			_eff_state = EFF_STATE_PLAY;
			_eff_proc = 0;
			Serial.println(F("PLAY state"));
			_exh_state = EXH_STATE_PLAY;
		}
	}
	
	//PLAY state
	else if( _exh_state == EXH_STATE_PLAY )
	{
		if( !SYSTEM_TimerArray[SYSTEM_TIMER_LOOPER] )
		{
			digitalWrite(LOOPER_REC_PIN, 0);
		}
		if( !SYSTEM_TimerArray[SYSTEM_TIMER_REC] )
		{
			digitalWrite(LOOPER_ERASE_PIN, 0);
			digitalWrite(LYRA_ENA_PIN, 1);
			SYSTEM_TimerArray[SYSTEM_TIMER_LOOPER] = LOOPER_ERASE_HOLD_TIME;
			_eff_state = EFF_STATE_IDLE;
			_eff_proc = 0;
			Serial.println(F("ERASE state"));
			_exh_state = EXH_STATE_ERASE;
		}
	}
	
	//ERASE state
	else if( _exh_state == EXH_STATE_ERASE )
	{
		if( !SYSTEM_TimerArray[SYSTEM_TIMER_LOOPER] )
		{
			digitalWrite(LOOPER_ERASE_PIN, 1);
			Serial.println(F("IDLE state"));
			_exh_state = EXH_STATE_IDLE;
		}
	}
	
	//LIGHT EFFECT STATES
	//-------------------------------------------------
	//IDLE state
	if( _eff_state == EFF_STATE_IDLE )
	{
		if( _eff_proc == 0 )
		{
			for( i = 0; i < PIX_NUM; i++ )
				_led_arr[i] = CRGB(EFF_IDLE_COLOR);
			for( i = 0; i < STAR_NUM; i++ )
				_star_arr[i].eff_state = 0;
			SYSTEM_TimerArray[SYSTEM_TIMER_FADE] = random(100, 5000);
			_eff_proc = 1;
		}
		else if( _eff_proc == 1 )
		{
			if( !SYSTEM_TimerArray[SYSTEM_TIMER_FADE] )
			{
				SYSTEM_TimerArray[SYSTEM_TIMER_FADE] = random(100, 5000);
				cur_line = random(0, STAR_NUM - 1);
				if( !_star_arr[cur_line].eff_state )
				{
					if( random(0, 2) == 0 )
						runStarFlash(cur_line, EFF_FLASH_COLOR1);
					else
						runStarFlash(cur_line, EFF_FLASH_COLOR2);
				}
			}
		}
	}
	
	//REC state
	else if( _eff_state == EFF_STATE_REC )
	{
		if( _eff_proc == 0 )
		{
			for( i = 0; i < PIX_NUM; i++ )
				_led_arr[i] = CRGB(EFF_REC_COLOR);
			for( i = 0; i < STAR_NUM; i++ )
				_star_arr[i].eff_state = 0;
			_eff_proc = 1;
		}
	}
	
	//PLAY state
	else if( _eff_state == EFF_STATE_PLAY )
	{
		if( _eff_proc == 0 )
		{
			for( i = 0; i < PIX_NUM; i++ )
				_led_arr[i] = CRGB(EFF_PLAY_COLOR);
			for( i = 0; i < STAR_NUM; i++ )
				_star_arr[i].eff_state = 0;
			_eff_proc = 1;
		}
	}
	
	//WAVE state
	else if( _eff_state == EFF_STATE_WAVE )
	{
		if( _eff_proc == 0 )
		{
			for( i = 0; i < PIX_NUM; i++ )
				_led_arr[i] = CRGB(EFF_IDLE_COLOR);
			for( i = 0; i < STAR_NUM; i++ )
				_star_arr[i].eff_state = 0;
			_eff_proc = 1;
			cur_line = 0;
			SYSTEM_TimerArray[SYSTEM_TIMER_FADE] = 0;
		}
		else if( _eff_proc == 1 )
		{
			if( !SYSTEM_TimerArray[SYSTEM_TIMER_FADE] )
			{
				runColFlash(cur_line, EFF_FLASH_COLOR1);
				if( cur_line < COL_NUM )
				{
					cur_line += 1;
					SYSTEM_TimerArray[SYSTEM_TIMER_FADE] = 100;
				}
				else
				{	
					_eff_state = EFF_STATE_IDLE;
					_eff_proc = 0;
				}
				
			}
		}
	}
	
	//STAR EFFECT PROCESSING
	//-------------------------------------------------
	for( i = 0; i < STAR_NUM; i++ )
	{
		//Init flash effect
		if( (_star_arr[i].eff_state == 1) || (_star_arr[i].eff_state == 3) )
		{
			//Calculating steps number
			if( _star_arr[i].eff_state == 1 )
				_star_arr[i].eff_steps_num = /*_star_arr[i].rise_dur*/EFF_FLASH_RISE_DUR / EFF_UPD_PERIOD;
			else if( _star_arr[i].eff_state == 3 )
				_star_arr[i].eff_steps_num = /*_star_arr[i].fall_dur*/EFF_FLASH_FALL_DUR / EFF_UPD_PERIOD;
			if( !_star_arr[i].eff_steps_num )
				_star_arr[i].eff_steps_num = 1;
			//Serial.print("Steps num = ");
			//Serial.println(_star_arr[i].eff_steps_num);
			
			//Calculating steps for all channels
			_star_arr[i].r_cur = (float)_star_arr[i].r_last;
			_star_arr[i].r_step = (((float)_star_arr[i].r_tar - _star_arr[i].r_cur) 
									/ (float)_star_arr[i].eff_steps_num);
			if( !_star_arr[i].r_step )
				_star_arr[i].r_cur = (float)_star_arr[i].r_tar;
			//Serial.print("RED step = ");
			//Serial.println(_star_arr[i].r_step);
			
			_star_arr[i].g_cur = (float)_star_arr[i].g_last;
			_star_arr[i].g_step = (int16_t)(((float)_star_arr[i].g_tar - _star_arr[i].g_cur) 
									/ (float)_star_arr[i].eff_steps_num);
			if( !_star_arr[i].g_step )
				_star_arr[i].g_cur = (float)_star_arr[i].g_tar;
			//Serial.print("GREEN step = ");
			//Serial.println(_star_arr[i].g_step);
			
			_star_arr[i].b_cur = (float)_star_arr[i].b_last;
			_star_arr[i].b_step = (int16_t)(((float)_star_arr[i].b_tar - _star_arr[i].b_cur) 
									/ (float)_star_arr[i].eff_steps_num); 				
			if( !_star_arr[i].b_step )
				_star_arr[i].b_cur = (float)_star_arr[i].b_tar;
			//Serial.print("BLUE step = ");
			//Serial.println(_star_arr[i].b_step);
			
			if( _star_arr[i].eff_state == 1 )
				_star_arr[i].eff_state = 2;
			else if( _star_arr[i].eff_state == 3 )
				_star_arr[i].eff_state = 4;
		}
		else if( (_star_arr[i].eff_state == 2) || (_star_arr[i].eff_state == 4) )
		{
			if( !_star_arr[i].eff_tmr )
			{
				//END effect condition
				if( (_star_arr[i].r_cur == (float)_star_arr[i].r_tar) 
					&& (_star_arr[i].g_cur == (float)_star_arr[i].g_tar)
					&& (_star_arr[i].b_cur == (float)_star_arr[i].b_tar))
				{
					//Switch effect direction
					if( _star_arr[i].eff_state == 2 )
					{
						_star_arr[i].r_tar = _star_arr[i].r_last;
						_star_arr[i].g_tar = _star_arr[i].g_last;
						_star_arr[i].b_tar = _star_arr[i].b_last;
						_star_arr[i].r_last = (uint8_t)_star_arr[i].r_cur;
						_star_arr[i].g_last = (uint8_t)_star_arr[i].g_cur;
						_star_arr[i].b_last = (uint8_t)_star_arr[i].b_cur;
						_star_arr[i].eff_state = 3;
					}
					//End effect
					else
					{
						//_star_arr[i].cur_eff = STAR_EFF_NO;
						_star_arr[i].eff_state = 0;
					}
				}
				else
				{
					//R channel
					if( _star_arr[i].r_cur != (float)_star_arr[i].r_tar )
					{
						if( _star_arr[i].r_cur < (float)_star_arr[i].r_tar )
						{
							if( _star_arr[i].r_cur + _star_arr[i].r_step > (float)_star_arr[i].r_tar )
								_star_arr[i].r_cur = _star_arr[i].r_tar;
							else
								_star_arr[i].r_cur += _star_arr[i].r_step;
						}
						else
						{
							if( _star_arr[i].r_cur + _star_arr[i].r_step < (float)_star_arr[i].r_tar )
								_star_arr[i].r_cur = (float)_star_arr[i].r_tar;
							else
								_star_arr[i].r_cur += _star_arr[i].r_step;
						}
					}
					//G channel
					if( _star_arr[i].g_cur != (float)_star_arr[i].g_tar )
					{
						if( _star_arr[i].g_cur < (float)_star_arr[i].g_tar )
						{
							if( _star_arr[i].g_cur + _star_arr[i].g_step > (float)_star_arr[i].g_tar )
								_star_arr[i].g_cur = (float)_star_arr[i].g_tar;
							else
								_star_arr[i].g_cur += _star_arr[i].g_step;
						}
						else
						{
							if( _star_arr[i].g_cur + _star_arr[i].g_step < (float)_star_arr[i].g_tar )
								_star_arr[i].g_cur = (float)_star_arr[i].g_tar;
							else
								_star_arr[i].g_cur += _star_arr[i].g_step;
						}
					}
					//B channel
					if( _star_arr[i].b_cur != (float)_star_arr[i].b_tar )
					{
						if( _star_arr[i].b_cur < (float)_star_arr[i].b_tar )
						{
							if( _star_arr[i].b_cur + _star_arr[i].b_step > (float)_star_arr[i].b_tar )
								_star_arr[i].b_cur = (float)_star_arr[i].b_tar;
							else
								_star_arr[i].b_cur += _star_arr[i].b_step;
						}
						else
						{
							if( _star_arr[i].b_cur + _star_arr[i].b_step < (float)_star_arr[i].b_tar )
								_star_arr[i].b_cur = (float)_star_arr[i].b_tar;
							else
								_star_arr[i].b_cur += _star_arr[i].b_step;
						}
					}
				}
				setStarColor(i, (uint8_t)_star_arr[i].r_cur, (uint8_t)_star_arr[i].g_cur, (uint8_t)_star_arr[i].b_cur);
				_star_arr[i].eff_tmr = EFF_UPD_PERIOD;
			}
		}
	}
}
