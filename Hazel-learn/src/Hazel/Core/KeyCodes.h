#pragma once

namespace Hazel
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		HZ_KEY_SPACE =32,
		HZ_KEY_APOSTROPHE =39,
		/* ' */
		HZ_KEY_COMMA =44,
		/* , */
		HZ_KEY_MINUS =45,
		/* - */
		HZ_KEY_PERIOD =46,
		/* . */
		HZ_KEY_SLASH =47,
		/* / */
		HZ_KEY_0 =48,
		HZ_KEY_1 =49,
		HZ_KEY_2 =50,
		HZ_KEY_3 =51,
		HZ_KEY_4 =52,
		HZ_KEY_5 =53,
		HZ_KEY_6 =54,
		HZ_KEY_7 =55,
		HZ_KEY_8 =56,
		HZ_KEY_9 =57,
		HZ_KEY_SEMICOLON =59,
		/* ; */
		HZ_KEY_EQUAL =61,
		/* = */
		HZ_KEY_A =65,
		HZ_KEY_B =66,
		HZ_KEY_C =67,
		HZ_KEY_D =68,
		HZ_KEY_E =69,
		HZ_KEY_F =70,
		HZ_KEY_G =71,
		HZ_KEY_H =72,
		HZ_KEY_I =73,
		HZ_KEY_J =74,
		HZ_KEY_K =75,
		HZ_KEY_L =76,
		HZ_KEY_M =77,
		HZ_KEY_N =78,
		HZ_KEY_O =79,
		HZ_KEY_P =80,
		HZ_KEY_Q =81,
		HZ_KEY_R =82,
		HZ_KEY_S =83,
		HZ_KEY_T =84,
		HZ_KEY_U =85,
		HZ_KEY_V =86,
		HZ_KEY_W =87,
		HZ_KEY_X =88,
		HZ_KEY_Y =89,
		HZ_KEY_Z =90,
		HZ_KEY_LEFT_BRACKET =91,
		/* [ */
		HZ_KEY_BACKSLASH =92,
		/* \ */
		HZ_KEY_RIGHT_BRACKET =93,
		/* ] */
		HZ_KEY_GRAVE_ACCENT =96,
		/* ` */
		HZ_KEY_WORLD_1 =161,
		/* non-US #1 */
		HZ_KEY_WORLD_2 =162,
		/* non-US #2 */

		/* Function keys */
		HZ_KEY_ESCAPE =256,
		HZ_KEY_ENTER =257,
		HZ_KEY_TAB =258,
		HZ_KEY_BACKSPACE =259,
		HZ_KEY_INSERT =260,
		HZ_KEY_DELETE =261,
		HZ_KEY_RIGHT =262,
		HZ_KEY_LEFT =263,
		HZ_KEY_DOWN =264,
		HZ_KEY_UP =265,
		HZ_KEY_PAGE_UP =266,
		HZ_KEY_PAGE_DOWN =267,
		HZ_KEY_HOME =268,
		HZ_KEY_END =269,
		HZ_KEY_CAPS_LOCK =280,
		HZ_KEY_SCROLL_LOCK =281,
		HZ_KEY_NUM_LOCK =282,
		HZ_KEY_PRINT_SCREEN =283,
		HZ_KEY_PAUSE =284,
		HZ_KEY_F1 =290,
		HZ_KEY_F2 =291,
		HZ_KEY_F3 =292,
		HZ_KEY_F4 =293,
		HZ_KEY_F5 =294,
		HZ_KEY_F6 =295,
		HZ_KEY_F7 =296,
		HZ_KEY_F8 =297,
		HZ_KEY_F9 =298,
		HZ_KEY_F10 =299,
		HZ_KEY_F11 =300,
		HZ_KEY_F12 =301,
		HZ_KEY_F13 =302,
		HZ_KEY_F14 =303,
		HZ_KEY_F15 =304,
		HZ_KEY_F16 =305,
		HZ_KEY_F17 =306,
		HZ_KEY_F18 =307,
		HZ_KEY_F19 =308,
		HZ_KEY_F20 =309,
		HZ_KEY_F21 =310,
		HZ_KEY_F22 =311,
		HZ_KEY_F23 =312,
		HZ_KEY_F24 =313,
		HZ_KEY_F25 =314,
		HZ_KEY_KP_0 =320,
		HZ_KEY_KP_1 =321,
		HZ_KEY_KP_2 =322,
		HZ_KEY_KP_3 =323,
		HZ_KEY_KP_4 =324,
		HZ_KEY_KP_5 =325,
		HZ_KEY_KP_6 =326,
		HZ_KEY_KP_7 =327,
		HZ_KEY_KP_8 =328,
		HZ_KEY_KP_9 =329,
		HZ_KEY_KP_DECIMAL =330,
		HZ_KEY_KP_DIVIDE =331,
		HZ_KEY_KP_MULTIPLY =332,
		HZ_KEY_KP_SUBTRACT =333,
		HZ_KEY_KP_ADD =334,
		HZ_KEY_KP_ENTER =335,
		HZ_KEY_KP_EQUAL =336,
		HZ_KEY_LEFT_SHIFT =340,
		HZ_KEY_LEFT_CONTROL =341,
		HZ_KEY_LEFT_ALT =342,
		HZ_KEY_LEFT_SUPER =343,
		HZ_KEY_RIGHT_SHIFT =344,
		HZ_KEY_RIGHT_CONTROL =345,
		HZ_KEY_RIGHT_ALT =346,
		HZ_KEY_RIGHT_SUPER =347,
		HZ_KEY_MENU =348,
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

/*
// From glfw3.h
#define HZ_KEY_SPACE              32
#define HZ_KEY_APOSTROPHE         39  
#define HZ_KEY_COMMA              44 
#define HZ_KEY_MINUS              45
#define HZ_KEY_PERIOD             46
#define HZ_KEY_SLASH              47
#define HZ_KEY_0                  48
#define HZ_KEY_1                  49
#define HZ_KEY_2                  50
#define HZ_KEY_3                  51
#define HZ_KEY_4                  52
#define HZ_KEY_5                  53
#define HZ_KEY_6                  54
#define HZ_KEY_7                  55
#define HZ_KEY_8                  56
#define HZ_KEY_9                  57
#define HZ_KEY_SEMICOLON          59 
#define HZ_KEY_EQUAL              61
#define HZ_KEY_A                  65
#define HZ_KEY_B                  66
#define HZ_KEY_C                  67
#define HZ_KEY_D                  68
#define HZ_KEY_E                  69
#define HZ_KEY_F                  70
#define HZ_KEY_G                  71
#define HZ_KEY_H                  72
#define HZ_KEY_I                  73
#define HZ_KEY_J                  74
#define HZ_KEY_K                  75
#define HZ_KEY_L                  76
#define HZ_KEY_M                  77
#define HZ_KEY_N                  78
#define HZ_KEY_O                  79
#define HZ_KEY_P                  80
#define HZ_KEY_Q                  81
#define HZ_KEY_R                  82
#define HZ_KEY_S                  83
#define HZ_KEY_T                  84
#define HZ_KEY_U                  85
#define HZ_KEY_V                  86
#define HZ_KEY_W                  87
#define HZ_KEY_X                  88
#define HZ_KEY_Y                  89
#define HZ_KEY_Z                  90
#define HZ_KEY_LEFT_BRACKET       91 
#define HZ_KEY_BACKSLASH          92
#define HZ_KEY_RIGHT_BRACKET      93
#define HZ_KEY_GRAVE_ACCENT       96
#define HZ_KEY_WORLD_1            161
#define HZ_KEY_WORLD_2            162

#define HZ_KEY_ESCAPE             256
#define HZ_KEY_ENTER              257
#define HZ_KEY_TAB                258
#define HZ_KEY_BACKSPACE          259
#define HZ_KEY_INSERT             260
#define HZ_KEY_DELETE             261
#define HZ_KEY_RIGHT              262
#define HZ_KEY_LEFT               263
#define HZ_KEY_DOWN               264
#define HZ_KEY_UP                 265
#define HZ_KEY_PAGE_UP            266
#define HZ_KEY_PAGE_DOWN          267
#define HZ_KEY_HOME               268
#define HZ_KEY_END                269
#define HZ_KEY_CAPS_LOCK          280
#define HZ_KEY_SCROLL_LOCK        281
#define HZ_KEY_NUM_LOCK           282
#define HZ_KEY_PRINT_SCREEN       283
#define HZ_KEY_PAUSE              284
#define HZ_KEY_F1                 290
#define HZ_KEY_F2                 291
#define HZ_KEY_F3                 292
#define HZ_KEY_F4                 293
#define HZ_KEY_F5                 294
#define HZ_KEY_F6                 295
#define HZ_KEY_F7                 296
#define HZ_KEY_F8                 297
#define HZ_KEY_F9                 298
#define HZ_KEY_F10                299
#define HZ_KEY_F11                300
#define HZ_KEY_F12                301
#define HZ_KEY_F13                302
#define HZ_KEY_F14                303
#define HZ_KEY_F15                304
#define HZ_KEY_F16                305
#define HZ_KEY_F17                306
#define HZ_KEY_F18                307
#define HZ_KEY_F19                308
#define HZ_KEY_F20                309
#define HZ_KEY_F21                310
#define HZ_KEY_F22                311
#define HZ_KEY_F23                312
#define HZ_KEY_F24                313
#define HZ_KEY_F25                314
#define HZ_KEY_KP_0               320
#define HZ_KEY_KP_1               321
#define HZ_KEY_KP_2               322
#define HZ_KEY_KP_3               323
#define HZ_KEY_KP_4               324
#define HZ_KEY_KP_5               325
#define HZ_KEY_KP_6               326
#define HZ_KEY_KP_7               327
#define HZ_KEY_KP_8               328
#define HZ_KEY_KP_9               329
#define HZ_KEY_KP_DECIMAL         330
#define HZ_KEY_KP_DIVIDE          331
#define HZ_KEY_KP_MULTIPLY        332
#define HZ_KEY_KP_SUBTRACT        333
#define HZ_KEY_KP_ADD             334
#define HZ_KEY_KP_ENTER           335
#define HZ_KEY_KP_EQUAL           336
#define HZ_KEY_LEFT_SHIFT         340
#define HZ_KEY_LEFT_CONTROL       341
#define HZ_KEY_LEFT_ALT           342
#define HZ_KEY_LEFT_SUPER         343
#define HZ_KEY_RIGHT_SHIFT        344
#define HZ_KEY_RIGHT_CONTROL      345
#define HZ_KEY_RIGHT_ALT          346
#define HZ_KEY_RIGHT_SUPER        347
#define HZ_KEY_MENU               348
*/

namespace Hazel
{
	using MouseCode = uint16_t;

	namespace Mouse
	{
		enum: MouseCode
		{
			// From glfw3.h
			Button0 = 0,
			Button1 = 1,
			Button2 = 2,
			Button3 = 3,
			Button4 = 4,
			Button5 = 5,
			Button6 = 6,
			Button7 = 7,

			ButtonLast = Button7,
			ButtonLeft = Button0,
			ButtonRight = Button1,
			ButtonMiddle = Button2
		};
	}
}
