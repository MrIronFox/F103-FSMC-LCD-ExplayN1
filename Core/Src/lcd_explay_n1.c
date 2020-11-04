#include "lcd_explay_n1.h"


uint32_t disp_x_size = 479;
uint32_t disp_y_size = 319;
uint8_t orient = LANDSCAPE;

void LCD_Delay(uint32_t dly){
    uint32_t i;
    for(i = 0; i < dly; i++);
}

void DelayMicro(uint32_t micros){
    micros *=(SystemCoreClock/1000000)/5;
    while (micros--);
}

void SetXY(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	if (orient==LANDSCAPE){
		swap(uint16_t, x1, y1);
		swap(uint16_t, x2, y2)
		y1=disp_y_size-y1;
		y2=disp_y_size-y2;
		swap(uint16_t, y1, y2)
	}
	ADDR_CMD = 0x2a;
	ADDR_DATA = x1>>8;
	ADDR_DATA = x1;
	ADDR_DATA = x2>>8;
	ADDR_DATA = x2;
    ADDR_CMD = 0x2b;
    ADDR_DATA = y1>>8;
    ADDR_DATA = y1;
    ADDR_DATA = y2>>8;
    ADDR_DATA = y2;
    ADDR_CMD = 0x2c;
}

void clrXY(){
	if (orient==PORTRAIT)
		SetXY(0,0,disp_x_size,disp_y_size);
	else
		SetXY(0,0,disp_y_size,disp_x_size);
}

void LCD_fillScr(uint16_t color){
	uint32_t i;
	for (i=0; i<((disp_x_size+1)*(disp_y_size+1)); i++)
		ADDR_DATA16 = color;
}

void LCD_clrScr(){
	clrXY();
	for (uint32_t i=0; i<((disp_x_size+1)*(disp_y_size+1)); i++)
		ADDR_DATA16 = VGA_BLACK;
}

uint32_t LCD_Read(uint16_t RG){
	uint32_t id;
	uint16_t x=0;
	ADDR_CMD=0xD0;
	DelayMicro(50);
	x=ADDR_DATA;
	id=x;
	id<<=16;
	DelayMicro(1);
	x=ADDR_DATA;
	id|=x;
	if(RG==0xEF){
		id<<=8;
		DelayMicro(5);
		x=ADDR_DATA;
		id|=x;
	}
	DelayMicro(150);//stabilization time
	return id;
}

void setColorRGB(uint8_t r, uint8_t g, uint8_t b){
	f_color = ((r&248)<<8)|((g&252)<<3)|(b&248)>>3;
}

void setColor(uint16_t color){
	f_color = color;
}

uint16_t getColor(){
	return f_color;
}

void setBackColorRGB(uint8_t r, uint8_t g, uint8_t b){
	b_color = ((r&248)<<8)|((g&252)<<3)|(b&248)>>3;
	_transparent=false;
}

void fillScr(uint16_t color){
	long i;

	clrXY();
	for (i=0; i<((disp_x_size+1)*(disp_y_size+1)); i++)	{
		ADDR_DATA16 = color;
		}
}

void fillScrRGB(uint8_t r, uint8_t g, uint8_t b){
//	word color = ((r&248)<<8 | (g&252)<<3 | (b&248)>>3);
	fillScr((r&248)<<8 | (g&252)<<3 | (b&248)>>3);
}
void setBackColor(uint32_t color){
	if (color==VGA_TRANSPARENT)
		_transparent=true;
	else{
		b_color = color;
		_transparent=false;
	}
}

uint16_t getBackColor(){
	return b_color;
}

int getDisplayXSize(){
	if (orient==PORTRAIT)
		return disp_x_size+1;
	else
		return disp_y_size+1;
}

int getDisplayYSize(){
	if (orient==PORTRAIT)
		return disp_y_size+1;
	else
		return disp_x_size+1;
}

void setPixel(uint16_t color){
	ADDR_DATA16 = color;
}

void drawPixel(int x, int y){
	SetXY(x, y, x, y);
	setPixel(f_color);
	clrXY();
}

void drawHLine(int x, int y, int l){
	if (l<0){
		l = -l;
		x -= l;
	}
	SetXY(x, y, x+l, y);
	for (int i=0; i<l+1; i++){
		ADDR_DATA16 = f_color;
	}
	clrXY();
}

void drawVLine(int x, int y, int l){
	if (l<0){
		l = -l;
		y -= l;
	}
	SetXY(x, y, x, y+l);
	for (int i=0; i<l+1; i++){
		ADDR_DATA16 = f_color;
	}
	clrXY();
}

void drawLine(int x1, int y1, int x2, int y2){
	if (y1==y2)
		drawHLine(x1, y1, x2-x1);
	else if (x1==x2)
		drawVLine(x1, y1, y2-y1);
	else{
		unsigned int	dx = (x2 > x1 ? x2 - x1 : x1 - x2);
		short			xstep =  x2 > x1 ? 1 : -1;
		unsigned int	dy = (y2 > y1 ? y2 - y1 : y1 - y2);
		short			ystep =  y2 > y1 ? 1 : -1;
		int				col = x1, row = y1;

		if (dx < dy){
			int t = - (dy >> 1);
			while (1){
				SetXY (col, row, col, row);
				ADDR_DATA16 = f_color;
				if (row == y2)
					return;
				row += ystep;
				t += dx;
				if (t >= 0){
					col += xstep;
					t   -= dy;
				}
			}
		}
		else{
			int t = - (dx >> 1);
			while (1){
				SetXY (col, row, col, row);
				ADDR_DATA16 = f_color;
				if (col == x2)
					return;
				col += xstep;
				t += dy;
				if (t >= 0){
					row += ystep;
					t   -= dx;
				}
			}
		}
	}
	clrXY();
}

void drawRect(int x1, int y1, int x2, int y2){
	if (x1>x2){
		swap(int, x1, x2);
	}
	if (y1>y2){
		swap(int, y1, y2);
	}
	drawHLine(x1, y1, x2-x1);
	drawHLine(x1, y2, x2-x1);
	drawVLine(x1, y1, y2-y1);
	drawVLine(x2, y1, y2-y1);
}

void drawRoundRect(int x1, int y1, int x2, int y2){
	if (x1>x2){
		swap(int, x1, x2);
	}
	if (y1>y2){
		swap(int, y1, y2);
	}
	if ((x2-x1)>4 && (y2-y1)>4){
		drawPixel(x1+1,y1+1);
		drawPixel(x2-1,y1+1);
		drawPixel(x1+1,y2-1);
		drawPixel(x2-1,y2-1);
		drawHLine(x1+2, y1, x2-x1-4);
		drawHLine(x1+2, y2, x2-x1-4);
		drawVLine(x1, y1+2, y2-y1-4);
		drawVLine(x2, y1+2, y2-y1-4);
	}
}

void fillRect(int x1, int y1, int x2, int y2){
	if (x1>x2){
		swap(int, x1, x2);
	}
	if (y1>y2){
		swap(int, y1, y2);
	}
	if (orient==PORTRAIT){
		for (int i=0; i<((y2-y1)/2)+1; i++){
			drawHLine(x1, y1+i, x2-x1);
			drawHLine(x1, y2-i, x2-x1);
		}
	}
	else{
		for (int i=0; i<((x2-x1)/2)+1; i++){
			drawVLine(x1+i, y1, y2-y1);
			drawVLine(x2-i, y1, y2-y1);
		}
	}
}

void fillRoundRect(int x1, int y1, int x2, int y2){
	if (x1>x2){
		swap(int, x1, x2);
	}
	if (y1>y2){
		swap(int, y1, y2);
	}
	if ((x2-x1)>4 && (y2-y1)>4){
		for (int i=0; i<((y2-y1)/2)+1; i++)
		{
			switch(i){
			case 0:
				drawHLine(x1+2, y1+i, x2-x1-4);
				drawHLine(x1+2, y2-i, x2-x1-4);
				break;
			case 1:
				drawHLine(x1+1, y1+i, x2-x1-2);
				drawHLine(x1+1, y2-i, x2-x1-2);
				break;
			default:
				drawHLine(x1, y1+i, x2-x1);
				drawHLine(x1, y2-i, x2-x1);
			}
		}
	}
}
void drawCircle(int x, int y, int radius){
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;

	SetXY(x, y + radius, x, y + radius);
	ADDR_DATA16 = f_color;
	SetXY(x, y - radius, x, y - radius);
	ADDR_DATA16 = f_color;
	SetXY(x + radius, y, x + radius, y);
	ADDR_DATA16 = f_color;
	SetXY(x - radius, y, x - radius, y);
	ADDR_DATA16 = f_color;
	while(x1 < y1){
		if(f >= 0){
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;
		SetXY(x + x1, y + y1, x + x1, y + y1);
		ADDR_DATA16 = f_color;
		SetXY(x - x1, y + y1, x - x1, y + y1);
		ADDR_DATA16 = f_color;
		SetXY(x + x1, y - y1, x + x1, y - y1);
		ADDR_DATA16 = f_color;
		SetXY(x - x1, y - y1, x - x1, y - y1);
		ADDR_DATA16 = f_color;
		SetXY(x + y1, y + x1, x + y1, y + x1);
		ADDR_DATA16 = f_color;
		SetXY(x - y1, y + x1, x - y1, y + x1);
		ADDR_DATA16 = f_color;
		SetXY(x + y1, y - x1, x + y1, y - x1);
		ADDR_DATA16 = f_color;
		SetXY(x - y1, y - x1, x - y1, y - x1);
		ADDR_DATA16 = f_color;
	}
	clrXY();
}

void fillCircle(int x, int y, int radius){
	for(int y1=-radius; y1<=0; y1++)
		for(int x1=-radius; x1<=0; x1++)
			if(x1*x1+y1*y1 <= radius*radius)
			{
				drawHLine(x+x1, y+y1, 2*(-x1));
				drawHLine(x+x1, y-y1, 2*(-x1));
				break;
			}
}

void printChar(uint8_t c, int x, int y){
	uint8_t i,ch;
	uint16_t j;
	uint16_t temp;
	if (!_transparent){
		if (orient==PORTRAIT){
			SetXY(x,y,x+cfont.x_size-1,y+cfont.y_size-1);
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j++){
				ch=pgm_read_byte(&cfont.font[temp]);
				for(i=0;i<8;i++){
					if((ch&(1<<(7-i)))!=0){
						setPixel(f_color);
					}
					else{
						setPixel(b_color);
					}
				}
				temp++;
			}
		}
		else{
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j+=(cfont.x_size/8)){
				SetXY(x,y+(j/(cfont.x_size/8)),x+cfont.x_size-1,y+(j/(cfont.x_size/8)));
				for (int zz=(cfont.x_size/8)-1; zz>=0; zz--){
					ch=pgm_read_byte(&cfont.font[temp+zz]);
					for(i=0;i<8;i++){
						if((ch&(1<<i))!=0){
							setPixel(f_color);
						}
						else{
							setPixel(b_color);
						}
					}
				}
				temp+=(cfont.x_size/8);
			}
		}
	}
	else{
		temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
		for(j=0;j<cfont.y_size;j++){
			for (int zz=0; zz<(cfont.x_size/8); zz++){
				ch=pgm_read_byte(&cfont.font[temp+zz]);
				for(i=0;i<8;i++){
					SetXY(x+i+(zz*8),y+j,x+i+(zz*8)+1,y+j+1);
					if((ch&(1<<(7-i)))!=0){
						setPixel(f_color);
					}
				}
			}
			temp+=(cfont.x_size/8);
		}
	}
	clrXY();
}

void rotateChar(uint8_t c, int x, int y, int pos, int deg){
	uint8_t i,j,ch;
	uint16_t temp;
	int newx,newy;
	double radian;
	radian=deg*0.0175;
	temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
	for(j=0;j<cfont.y_size;j++){
		for (int zz=0; zz<(cfont.x_size/8); zz++){
			ch=pgm_read_byte(&cfont.font[temp+zz]);
			for(i=0;i<8;i++){
				newx=x+(((i+(zz*8)+(pos*cfont.x_size))*cos(radian))-((j)*sin(radian)));
				newy=y+(((j)*cos(radian))+((i+(zz*8)+(pos*cfont.x_size))*sin(radian)));

				SetXY(newx,newy,newx+1,newy+1);

				if((ch&(1<<(7-i)))!=0){
					setPixel(f_color);
				}
				else{
					if (!_transparent)
						setPixel(b_color);
				}
			}
		}
		temp+=(cfont.x_size/8);
	}
	clrXY();
}

void print(char *st, int x, int y, int deg){
	int stl, i;
	stl = strlen(st);
	if (orient==PORTRAIT){
		if (x==RIGHT)
			x=(disp_x_size+1)-(stl*cfont.x_size);
		if (x==CENTER)
			x=((disp_x_size+1)-(stl*cfont.x_size))/2;
	}
	else{
		if (x==RIGHT)
			x=(disp_y_size+1)-(stl*cfont.x_size);
		if (x==CENTER)
			x=((disp_y_size+1)-(stl*cfont.x_size))/2;
	}
	for (i=0; i<stl; i++)
		if (deg==0)
			printChar(*st++, x + (i*(cfont.x_size)), y);
		else
			rotateChar(*st++, x, y, i, deg);
}

/*
void lprint(char *st, int x, int y, int deg){
	char buf[st.length()+1];
	st.toCharArray(buf, st.length()+1);
	print(buf, x, y, deg);
}
*/

void printNumI(long num, int x, int y, int length, char filler){
	char buf[25];
	char st[27];
	bool neg=false;
	int c=0, f=0;
	if (num==0){
		if (length!=0){
			for (c=0; c<(length-1); c++)
				st[c]=filler;
			st[c]=48;
			st[c+1]=0;
		}
		else{
			st[0]=48;
			st[1]=0;
		}
	}
	else{
		if (num<0){
			neg=true;
			num=-num;
		}
		while (num>0){
			buf[c]=48+(num % 10);
			c++;
			num=(num-(num % 10))/10;
		}
		buf[c]=0;

		if (neg){
			st[0]=45;
		}
		if (length>(c+neg)){
			for (int i=0; i<(length-c-neg); i++){
				st[i+neg]=filler;
				f++;
			}
		}
		for (int i=0; i<c; i++){
			st[i+neg+f]=buf[c-i-1];
		}
		st[c+neg+f]=0;
	}
	print(st,x,y,0);
}

void _convert_float(char *buf, double num, int width, uint8_t prec){
	char format[10];
	sprintf(format, "%%%i.%if", width, prec);
	sprintf(buf, format, num);
}


void printNumF(double num, uint8_t dec, int x, int y, char divider, int length, char filler){
	char st[27];
	bool neg=false;
	if (dec<1)
		dec=1;
	else if (dec>5)
		dec=5;

	if (num<0)
		neg = true;

	_convert_float(st, num, length, dec);
	if (divider != '.'){
		for (int i=0; i<sizeof(st); i++)
			if (st[i]=='.')
				st[i]=divider;
	}
	if (filler != ' '){
		if (neg){
			st[0]='-';
			for (int i=1; i<sizeof(st); i++)
				if ((st[i]==' ') || (st[i]=='-'))
					st[i]=filler;
		}
		else{
			for (int i=0; i<sizeof(st); i++)
				if (st[i]==' ')
					st[i]=filler;
		}
	}

	print(st,x,y,0);
}

void setFont(uint8_t* font){
	cfont.font=font;
	cfont.x_size=fontbyte(0);
	cfont.y_size=fontbyte(1);
	cfont.offset=fontbyte(2);
	cfont.numchars=fontbyte(3);
}

uint8_t* getFont(){
	return cfont.font;
}

uint8_t getFontXsize(){
	return cfont.x_size;
}

uint8_t getFontYsize(){
	return cfont.y_size;
}

void InitLCD(){
/*
	ADDR_CMD = 0xE9;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x20;
	ADDR_CMD = 0x11; //Exit Sleep
	HAL_Delay(100);
	ADDR_CMD = 0xD1;//VCOM Control
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x71;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x19;

	ADDR_CMD = 0xD0;//Power_Setting
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x07;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x01;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x08;

	ADDR_CMD = 0x36;//set_address_mode
//	ADDR_DATA = 0x00;
	ADDR_DATA = 0xE8;//Page Address Order:Top to Bottom,Column Address Order:Right to Left,Page/Column Order:Normal Mode,Line Address Order:LCD Refresh Top to Bottom
	                              		//RGB/BGR Order:Pixels sent in BGR order, Display Data Latch Data Order:Not supported, Horizontal Flip:Normal display, Vertical Flip: Normal display

	ADDR_CMD = 0x3A;//set_pixel_format
//	ADDR_DATA = 0x00;
	ADDR_DATA = 0x05;//DBI:16bit/pixel (65,536 colors)

	ADDR_CMD = 0xC1;//Display_Timing_Setting for Normal/Partial Mode
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x10;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x10;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x02;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x02;

	ADDR_CMD = 0xC0; //Panel Driving Setting / Set Default Gamma
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x35;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x01;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x02;

	ADDR_CMD = 0xC5;  //Frame Rate Control / Set frame rate
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x04;//72Hz

	ADDR_CMD = 0xD2; //Power_Setting for Normal Mode / power setting
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x01;//Gamma Driver Amplifier:1.00, Source Driver Amplifier: 1.00
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x44;

	ADDR_CMD = 0xC8; //Gamma Setting / Set Gamma
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x04;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x67;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x35;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x04;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x08;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x06;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x24;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x01;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x37;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x40;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x03;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x10;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x08;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x80;
	ADDR_DATA = 0x00;
	ADDR_DATA = 0x00;

	ADDR_CMD = 0x29; //set_display_on / display on

	ADDR_CMD = 0x2C; //write_memory_start / display on
*/
	ADDR_CMD = 0x11; //Exit Sleep
	HAL_Delay(100);
	ADDR_CMD = 0x36;  // очень важен, иначе перевернуто и цветопередача не та
	ADDR_DATA = 0xE8;
	ADDR_CMD = 0x3A;  // очень важен, иначе черно-белые кракозябры
	ADDR_DATA = 0x05;
	ADDR_CMD = 0x29; //display on
	ADDR_CMD = 0x2C; //display on

	setColorRGB(255, 255, 255);
	setBackColorRGB(0, 0, 0);
	cfont.font=0;
	_transparent = false;
	orient = PORTRAIT;
}
