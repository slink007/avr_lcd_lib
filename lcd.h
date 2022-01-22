#ifndef LCD_H
#define LCD_H

#define DATAPORT PORTB
#define DATADIR DDRB
#define CONTROLPORT PORTD
#define CONTROLDIR DDRD
#define RS 2
#define EN 3

typedef enum {FIRST, SECOND} validRow_t;

/*
 * Sets up PortB as data bus for LCD.  PortD2 is RS and portD3 is EN.
 * 
 * LCD will be 2 lines (5 x 7), cursor begins at start of first row, display
 * is on and cleared, cursor is not shown.
 */
void lcd_init(void);


/*
 * Specify the row and column to start printing text and then send the
 * "str" to the LCD for display.
 * 
 * Returns the number of characters written to the LCD.
 */
char lcd_text(validRow_t row, char column, const char *str);


/*
 * Specify the row and column to start printing text.  Convert 'number'
 * to a string and send it to the LCD for display.
 * 
 * Returns the number of characters written to the LCD.
 */
char lcd_float(validRow_t row, char column, char precision, float number);



/* Remove all text from the LCD */
void clear_text(void);


/*
 * Removes all characters from the LCD row specified in the "row" input.
 */
void clear_row(validRow_t row);

#endif
