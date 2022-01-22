#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"


#define rs_high() CONTROLPORT |= (1 << RS);
#define rs_low() CONTROLPORT &= ~(1 << RS);
#define en_high() CONTROLPORT |= (1 << EN);
#define en_low() CONTROLPORT &= ~(1 << EN);

// For HD44780 the rows are 16 characters long
#define MAXLENGTH 16
const char BLANKROW[] = "                ";

/*
 * Send command to LCD.  Put command onto data bus, pull RS low, pulse EN high,
 * return RS high so LCD will interpret future data as something to be displayed
 * on the LCD.
 */
void lcd_cmd(unsigned char cmd)
{
    DATAPORT = cmd;
    rs_low();       // rs = 0 to select the command register
    en_high();
    _delay_ms(1);
    en_low();
    rs_high();      // rs = 1 to select data register
}


/*
 *  Send text to the LCD for display.  Puts a character onto the data bus and 
 *  pulse EN high.
 */
void lcd_data(unsigned char text)
{
    DATAPORT = text;
    en_high();
    _delay_ms(1);
    en_low();
}


void lcd_init(void)
{
    DATADIR = 0xFF;
    CONTROLPORT |= (1 << RS);
    CONTROLPORT |= (1 << EN);

    lcd_cmd(0x38); _delay_ms(5);  // 2 lines, 5 x 7 matrix
    lcd_cmd(0x01); _delay_ms(5);  // clear screen
    lcd_cmd(0x0C); _delay_ms(5);  // display on, cursor off (0x0A for cursor on)
    lcd_cmd(0x06); _delay_ms(5);  // cursor shifts from left to right
    lcd_cmd(0x80); _delay_ms(5);  // cursor to beginning of first row
}


/*
 * Moves LCD cursor to the row/column passed to the function.
 * 
 * Return code 0 for cursor successfully moved.
 * Return code -1 for column out of range.
 */
int cursorxy(validRow_t row, unsigned char column)
{
    if ( (column < 1) || (column > MAXLENGTH) )
        return -1;      // column out of range

    if (row == FIRST)
        lcd_cmd(0x7F + column);
    else if (row == SECOND)
        lcd_cmd(0xBF + column);
    else
        return -2;

    return 0;
}


char lcd_text(validRow_t row, char column, const char *str)
{
    int numchars = 0;

    // Try to move cursor to the desired location
    int failure = cursorxy(row, column);

    switch(failure)
    {
        case -1:
            clear_text();
            return lcd_text(FIRST, 1, "Bad column index");
            break;
        case -2:
            clear_text();
            return lcd_text(FIRST, 1, "Bad row index");
            break;
        case 0:
        default:
            if (strlen(str) <= (MAXLENGTH - column - 1))
            {
		        // Send text to the LCD
		        while(*str)
		        {
		            lcd_data(*str);
		            str++;
		            numchars++;
		        }
		        return numchars;
            }
            else
            {
                clear_text();
                return lcd_text(FIRST, 1, "Err: Too Long");
            }
    }
}


char lcd_float(validRow_t row, char column, char precision, float number)
{
    char text[MAXLENGTH + 1] = {0};

    // Get the whole part of the float
    float remainder = floatToString(number, text);

    // We have enough room to display the number
    if (strlen(text) < (MAXLENGTH - 1 - precision))
    {
        strcat(text, ".");
        while (precision--)
        {
            remainder = floatToString(remainder, text);
        }
    }
    else
    {
        clear_text();
        strcpy(text, "Err: Too Long");  // number too long to display
    }

    lcd_text(row, column, text);
    return (char)strlen(text);
}


/* Helper function for lcd_float().
 *
 * Converts the whole part of 'number' into a string and appends it
 * onto str.  The remainder is multiplied by 10 and returned to the caller.
 * If/when the remainder is passed back into this function, the most
 * significant digit will then be the whole part, and it can be extracted
 * and appended to str.  Repeated calls allow you get as many digits from
 * the remainder as desired.
 */
float floatToString(float number, char *str)
{
    char temp[MAXLENGTH + 1] = {0};

    if (number < 0)  // supply negative sign if needed
    {
        strcat(str, "-");
        number = 0 - number;
    }

    unsigned int wholePart = (unsigned int)number;
    float remainder = number - (float)wholePart;
    sprintf(temp, "%d", wholePart);
    strcat(str, temp);

    return (remainder * 10);
}


void clear_text(void)
{
    lcd_cmd(0x01);
}


void clear_row(validRow_t row)
{
    lcd_text(row, 1, BLANKROW);
}
