// I N C L U D E S ///////////////////////////////////////////////////////////
#include <dos.h>
//#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
// D E F I N E S  ////////////////////////////////////////////////////////////

// bitmasks for control keys/shift status
/*
#define SHIFT_R             0x0001
#define SHIFT_L             0x0002
#define CTRL                0x0004
#define ALT                 0x0008
#define SCROLL_LOCK_ON      0x0010
#define NUM_LOCK_ON         0x0020
#define CAPS_LOCK_ON        0x0040
#define INSERT_MODE         0x0080
#define CTRL_L              0x0100
#define ALT_L               0x0200
#define CTRL_R              0x0400
#define ALT_R               0x0800
#define SCROLL_LOCK_DWN     0x1000
#define NUM_LOCK_DWN        0x2000
#define CAPS_LOCK_DWN       0x4000
#define SYS_REQ_DWN         0x8000

// scan code values, note keys with two symbols on them are the same so I will
// consistantly try to use the lower symbol for example. the 1 key also has a
// ! above it, but we would just call it the SCAN_1 key.

#define SCAN_ESC          1
#define SCAN_1            2
#define SCAN_2            3
#define SCAN_3            4
#define SCAN_4            5
#define SCAN_5            6
#define SCAN_6            7
#define SCAN_7            8
#define SCAN_8            9
#define SCAN_9            10
#define SCAN_0            11
#define SCAN_MINUS        12
#define SCAN_EQUALS       13
#define SCAN_BKSP         14
#define SCAN_TAB          15
#define SCAN_Q            16
#define SCAN_W            17
#define SCAN_E            18
#define SCAN_R            19
#define SCAN_T            20
#define SCAN_Y            21
#define SCAN_U            22
#define SCAN_I            23
#define SCAN_O            24
#define SCAN_P            25
#define SCAN_LFT_BRACKET  26
#define SCAN_RGT_BRACKET  27
#define SCAN_ENTER        28
#define SCAN_CTRL         29

#define SCAN_A            30
#define SCAN_S            31
#define SCAN_D            32
#define SCAN            33
#define SCAN_G            34
#define SCAN_H            35
#define SCAN_J            36
#define SCAN_K            37
#define SCAN_L            38

#define SCAN_SEMI         39
#define SCAN_APOS         40
#define SCAN_TILDE        41

#define SCAN_LEFT_SHIFT   42
#define SCAN_BACK_SLASH   43
#define SCAN_Z            44
#define SCAN_X            45
#define SCAN_C            46
#define SCAN_V            47
#define SCAN_B            48
#define SCAN_N            49
#define SCAN_M            50
#define SCAN_COMMA        51
#define SCAN_PERIOD       52
#define SCANOWARD_SLASH 53
#define SCAN_RIGHT_SHIFT  54
#define SCAN_PRT_SCRN     55
#define SCAN_ALT          56
#define SCAN_SPACE        57
#define SCAN_CAPS_LOCK    58
#define SCAN1           59
#define SCAN2           60
#define SCAN3           61
#define SCAN4           62
#define SCAN5           63
#define SCAN6           64
#define SCAN7           65
#define SCAN8           66
#define SCAN9           67
#define SCAN10          68
#define SCAN11          133
#define SCAN12          134
#define SCAN_NUM_LOCK     69
#define SCAN_SCROLL_LOCK  70
#define SCAN_HOME         71
#define SCAN_UP           72
#define SCAN_PGUP         73
#define SCAN_NUM_MINUS    74
#define SCAN_LEFT         75
#define SCAN_CENTER       76
#define SCAN_RIGHT        77
#define SCAN_NUM_PLUS     78
#define SCAN_END          79
#define SCAN_DOWN         80
#define SCAN_PGDWN        81
#define SCAN_INS          82
#define SCAN_DEL          83
*/
#define KEYBOARD 0x09
//
// Keyboard press/release codes for the INT 9h handler
//
#define RIGHT_ARROW_PRESSED   77
#define RIGHT_ARROW_REL      205
#define LEFT_ARROW_PRESSED    75
#define LEFT_ARROW_REL       203
#define ESC_PRESSED          129
#define UP_ARROW_PRESSED      72
#define UP_ARROW_REL         200
#define DOWN_ARROW_PRESSED    80
#define DOWN_ARROW_REL       208
#define Q_PRESSED             16
#define P_PRESSED             25
#define SPACE_PRESSED         57
#define CTRL_PRESSED          29
#define CTRL_REL              157
#define ALT_PRESSED				56
#define ALT_REL					184

#ifdef __cplusplus
	#define __CPPARGS ...
#else
	 #define __CPPARGS
#endif
// F U N C T I O N S /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

