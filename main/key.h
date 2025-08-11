#ifndef __KEY_H__
#define __KEY_H__

#define keyDelayTime 30	    //消抖时间
#define keyCheckTime 10     //检测间隔
#define keyLongTime 500    //长按时间

#ifdef CONFIG_IDF_TARGET_ESP32C3
enum{
#ifdef CONFIG_ADC_KEY
    keyIoSet=9,
#else
    keyIoSet=18,
#endif
    keyIoLeft=3,
    keyIoRight=9,
};
#else
enum{
    keyIoSet=9,
    keyIoLeft=3,
    keyIoRight=8,
};
#endif

enum{
    keyNumSet,
    keyNumLeft,
    keyNumRight,
    keyNumMax,
};

enum{
	keyStateNone,
	keyStateDowning,
	keyStateDowned,
	keyStateDownedLong,
	keyStateUping,
};

enum{
    keySetShort,
    keySetLong,
    keyLeftShort,
    keyLeftLong,
    keyRightShort,
    keyRightLong,
};

enum{
    keyEventNone,
    keyEventShort,  //短按
    keyEventLong,   //长按
};

enum{
    keyTypeTTL,
    keyTypeADC,
};

typedef void (*key_fun_t)(int);

typedef struct {
	unsigned long tm;	//上一次检测时间
	uint8_t io;			//IO口
	uint8_t state;		//状态
	uint8_t type;       //类型
	uint16_t valMin;	//最小触发值
	uint16_t valMax;	//最大触发值
    key_fun_t fun;
} key_trigger_t;


int key_init(void);
int key_register_fun(int key, key_fun_t fun);

#endif
