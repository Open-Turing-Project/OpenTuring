
#ifndef _PANIC_H_
#define _PANIC_H_

/* ------------------------ Exported constants ------------------------- */

#define excpSIGBASE	150	/* Base exception number for signals */

#define excpSIGFPE	excpSIGBASE + 2
#define excpSIGILL	excpSIGBASE + 3
#define excpSIGSEGV	excpSIGBASE + 5

/* ------------------------ Exported variables ------------------------- */

/* None */

/* ------------------------ Exported functions ------------------------- */

void Panic(char *str);
void Warning(char *str);

#endif /* #ifdef _PANIC_H_ */
