#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <string.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif
// ---------------------------------------------------------------------------
#include <cust_adc.h>
#define MIN_VOLTAGE (700)	//++++rgk bug-id:no modify by yangjuwei 20140401
#define MAX_VOLTAGE (1000)	//++++rgk bug-id:no modify by yangjuwei 20140401

//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH		(720)
#define FRAME_HEIGHT		(1280)
#define LCM_ID			0x1283 

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

unsigned static int lcm_esd_test = FALSE;      ///only for ESD test

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util ;

#define SET_RESET_PIN(v)    			(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 				(lcm_util.udelay(n))
#define MDELAY(n) 				(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(para_tbl,size,force_update)			lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)		lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)			lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)							lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)							lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE						0

//for lcm detect ,read adc voltage
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);

static LCM_setting_table_V3 lcm_initialization_setting[] = {
	/*************************************************
	  Copyright HUIZHOU TRUST INDUSTRY CO.LTD. All rights reserved.

	  Project:T050MHD664MT; 
										
	  Driver :OTM1283A+CMI5.0HD;

	  Interface :  MIPI-4LANE;   

	  Voltage :  VCI=2.8V IOVCC=1.8V 	

	  Author: RD/CHEN

	  History:
		 V01  First Release      2014-3-21
	************************************************/  
	{0x15,0x00,1,{0x00}},
	{0x39,0xff,3,{0x12,0x83,0x01}},


	{0x15,0x00,1,{0x80}},
	{0x39,0xff,2,{0x12,0x83}},

	{0x15,0x00,1,{0x92}},
	{0x39,0xff,2,{0x20,0x02}},
	//Delayms(1}},

	{0x15,0x00,1,{0xB9}},
	{0x15,0xB0,1,{0x51}},

	{0x15,0x00,1,{0xc6}},
	{0x15,0xb0,1,{0x03}},

	{0x15,0x00,1,{0x80}},
	{0x39,0xc0,9,{0x00,0x64,0x00,0x10,0x10,0x00,0x64,0x10,0x10}},

	{0x15,0x00,1,{0x90}},
	{0x39,0xc0,6,{0x00,0x5c,0x00,0x01,0x00,0x04}},//5c

	{0x15,0x00,1,{0xb3}},
	{0x39,0xc0,2,{0x00,0x50}},

	{0x15,0x00,1,{0x81}},
	{0x15,0xc1,1,{0x66}},

	{0x15,0x00,1,{0xA0}},
	{0x15,0xc1,1,{0x02}},

	{0x15,0x00,1,{0x82}},
	{0x15,0xC4,1,{0x02}},

	{0x15,0x00,1,{0x8B}},
	{0x15,0xC4,1,{0x40}},

	{0x15,0x00,1,{0x90}},
	{0x15,0xc4,1,{0x49}},

	{0x15,0x00,1,{0xa0}},
	{0x39,0xc4,14,{0x05,0x10,0x04,0x02,0x05,0x15,0x11,0x05,0x10,0x07,0x02,0x05,0x15,0x11}},

	{0x15,0x00,1,{0xb0}},
	{0x39,0xc4,2,{0x00,0x00}},

	{0x15,0x00,1,{0x90}},
	{0x39,0xc5,4,{0x50,0xa6,0xd0,0x66}},

	{0x15,0x00,1,{0xb0}},
	{0x39,0xc5,2,{0x04,0x38}},

	{0x15,0x00,1,{0xb4}},
	{0x15,0xc5,1,{0xc0}},

	{0x15,0x00,1,{0xb5}},
	{0x39,0xc5,6,{0x0b,0x95,0xff,0x0b,0x95,0xff}},

	{0x15,0x00,1,{0xbb}},
	{0x15,0xc5,1,{0x80}},

	{0x15,0x00,1,{0x90}},
	{0x39,0xf5,4,{0x02,0x11,0x02,0x11}},

	{0x15,0x00,1,{0x94}},
	{0x15,0xF5,1,{0x02}},

	{0x15,0x00,1,{0xb2}},
	{0x39,0xf5,8,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x15,0x00,1,{0xBA}},
	{0x15,0xF5,1,{0x03}},


	{0x15,0x00,1,{0x80}},
	{0x39,0xcb,11,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x15,0x00,1,{0x90}},
	{0x39,0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x15,0x00,1,{0xa0}},
	{0x39,0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x15,0x00,1,{0xb0}},
	{0x39,0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x15,0x00,1,{0xc0}},
	{0x39,0xcb,15,{0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x04,0x00,0x00,0x00,0x00}},

	{0x15,0x00,1,{0xd0}},
	{0x39,0xcb,15,{0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04}},

	{0x15,0x00,1,{0xe0}},
	{0x39,0xcb,14,{0x04,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00}},

	{0x15,0x00,1,{0xf0}},
	{0x39,0xcb,11,{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}},

	{0x15,0x00,1,{0x80}},
	{0x39,0xcc,15,{0x29,0x2a,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x06,0x00,0x08,0x00,0x00,0x00,0x00}},

	{0x15,0x00,1,{0x90}},
	{0x39,0xcc,15,{0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x29,0x2a,0x09,0x0b,0x0d,0x0f,0x11,0x13}},

	{0x15,0x00,1,{0xa0}},
	{0x39,0xcc,14,{0x05,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00}},

	{0x15,0x00,1,{0xb0}},
	{0x39,0xcc,15,{0x29,0x2a,0x13,0x11,0x0f,0x0d,0x0b,0x09,0x01,0x00,0x07,0x00,0x00,0x00,0x00}},

	{0x15,0x00,1,{0xc0}},
	{0x39,0xcc,15,{0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x29,0x2a,0x14,0x12,0x10,0x0e,0x0c,0x0a}},

	{0x15,0x00,1,{0xd0}},
	{0x39,0xcc,14,{0x02,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00}},

	{0x15,0x00,1,{0x80}},
	{0x39,0xce,12,{0x89,0x05,0x10,0x88,0x05,0x10,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x15,0x00,1,{0x90}},
	{0x39,0xce,14,{0x54,0xFD,0x10,0x54,0xFE,0x10,0x55,0x01,0x10,0x55,0x02,0x10,0x00,0x00}},

	{0x15,0x00,1,{0xa0}},
	{0x39,0xce,14,{0x58,0x07,0x04,0xFD,0x00,0x10,0x00,0x58,0x06,0x04,0xFE,0x00,0x10,0x00}},

	{0x15,0x00,1,{0xb0}},
	{0x39,0xce,14,{0x58,0x05,0x04,0xFF,0x00,0x10,0x00,0x58,0x04,0x05,0x00,0x00,0x10,0x00}},

	{0x15,0x00,1,{0xc0}},
	{0x39,0xce,14,{0x58,0x03,0x05,0x01,0x00,0x10,0x00,0x58,0x02,0x05,0x02,0x00,0x10,0x00}},

	{0x15,0x00,1,{0xd0}},
	{0x39,0xce,14,{0x58,0x01,0x05,0x03,0x00,0x10,0x00,0x58,0x00,0x05,0x04,0x00,0x10,0x00}},

	{0x15,0x00,1,{0x80}},
	{0x39,0xcf,14,{0x50,0x00,0x05,0x05,0x00,0x10,0x00,0x50,0x01,0x05,0x06,0x00,0x10,0x00}},

	{0x15,0x00,1,{0x90}},
	{0x39,0xcf,14,{0x50,0x02,0x05,0x07,0x00,0x10,0x00,0x50,0x03,0x05,0x08,0x00,0x10,0x00}},

	{0x15,0x00,1,{0xc0}},
	{0x39,0xcf,11,{0x39,0x39,0x20,0x20,0x00,0x00,0x01,0x01,0x20,0x00,0x00}},

	{0x15,0x00,1,{0x00}},
	{0x39,0xD8,2,{0xac,0xac}},//be

	{0x15,0x00,1,{0x00}},
	{0x15,0xD9,1,{0x75}},//73 7a 8a 6a

       {0x15,0x00,1,{0x80}},
	{0x15,0xc4,1,{0x01}},//FAE �ؼ�

	{0x15,0x00,1,{0x88}},
	{0x15,0xc4,1,{0x80}},
//FAE �ؼ�

	{0x15,0x00,1,{0xC2}},
	{0x15,0xF5,1,{0xC0}},
//FAE	�ؼ�
	{0x15,0x00,1,{0x94}},
	{0x39,0xC5,2,{0x33,0x33}},//44

	{0x15,0x00,1,{0x00}},
	{0x39,0xE1,16,{0x02,0x10,0x16,0x0E,0x07,0x0F,0x0B,0x0A,0x04,0x07,0x0D,0x08,0x0F,0x14,0x0E,0x08}},
	{0x15,0x00,1,{0x00}},
	{0x39,0xE2,16,{0x02,0x11,0x17,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x08,0x0D,0x08,0x0F,0x14,0x0E,0x08}},

	{0x15,0x00,1,{0x00}},
	{0x39,0xff,3,{0xff,0xff,0xff}},


	{0x15,0x00,1,{0x00}},        
	{0x05,0x11,	0,	{0x00}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3,120,{}},
	{0x15,0x00,1,{0x00}},
	{0x05,0x29,	0,	{0x00}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3,20,{}},

};

static void lcm_register()
{
	unsigned int data_array[35];

	data_array[0] = 0x00110500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(250);
	data_array[0] = 0x00290500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(50);
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{

	memset(params, 0, sizeof(LCM_PARAMS));
	
	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;


#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
	params->dsi.mode   = SYNC_PULSE_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; 
#endif
	
	// DSI
	/* Command mode setting */
	//1 Three lane or Four lane
	params->dsi.LANE_NUM				= LCM_THREE_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Video mode setting		
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		
	params->dsi.vertical_sync_active				= 0x05;// 3    2
	params->dsi.vertical_backporch					= 0x0d;// 20   1
	params->dsi.vertical_frontporch					= 0x08; // 1  12
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 0x12;// 50  2
	params->dsi.horizontal_backporch				= 0x5f;
	params->dsi.horizontal_frontporch				= 0x5f;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
	
	//params->dsi.LPX=8; 

	// Bit rate calculation
	params->dsi.PLL_CLOCK = 286;//250 265 245 249 237
	//1 Every lane speed
	//params->dsi.pll_div1=0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
	//params->dsi.pll_div2=0;		// div2=0,1,2,3;div1_real=1,2,4,4	
	//params->dsi.fbk_div =9;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	


}


static void lcm_init(void)
{
	 //enable VSP & VSN
    lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ONE);
    lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ONE);
    MDELAY(50);
	//reset high to low to high
    SET_RESET_PIN(1);
    MDELAY(20);
    SET_RESET_PIN(0);
    MDELAY(30);
    SET_RESET_PIN(1);
    MDELAY(30);

    //lcm_id_pin_handle();
 
	//lcm_register();
	dsi_set_cmdq_V3(lcm_initialization_setting,sizeof(lcm_initialization_setting)/sizeof(lcm_initialization_setting[0]),1);

	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static  LCM_setting_table_V3 lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x05, 0x28, 0, {}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 10, {}},

	// Sleep Mode On
	{0x05, 0x10, 0, {}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 120, {}},
};
static void lcm_suspend(void)
{	
    //Back to MP.P7 baseline , solve LCD display abnormal On the right
    // when phone sleep , config output low, disable backlight drv chip  
  //  lcm_util.set_gpio_out(GPIO_LCD_DRV_EN_PIN, GPIO_OUT_ZERO);  	

    dsi_set_cmdq_V3(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(lcm_deep_sleep_mode_in_setting[0]), 1);
    //reset low
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(5);
    //disable VSP & VSN
	lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ZERO);
	lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ZERO);
    MDELAY(5);	
  /*#ifdef BUILD_LK
	;
	 #else
printk("sym lcm_suspend\n");
	 #endif
  lcm_compare_id();*/
  //  printk("kernel:boe_nt35521_lcm_suspend\n");
}


static void lcm_resume(void)
{
	lcm_init();
	
	//push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

}
#endif


static unsigned int lcm_compare_id(void)
{

	int array[4];
	char buffer[5];
	char id_high=0;
	char id_low=0;
	int id=0;
   lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ONE);
    lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ONE);
	MDELAY(50);
	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(200);


	array[0] = 0x00053700;
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0xa1, buffer, 5);

	id_high = buffer[2];
	id_low = buffer[3];
	id = (id_high<<8) | id_low;
	
	#ifdef BUILD_LK
		printf("J5010 otm1283a uboot %s \n", __func__);
		printf("J5010 %s id = 0x%08x \n", __func__, id);
	#else
		printk("J5010 otm1283a kernel %s \n", __func__);
	printk("J5010 %s id = 0x%08x \n", __func__, id);
	#endif
  // lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ZERO);
 //   lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ZERO);	
	MDELAY(50);
	return (LCM_ID == id) ? 1:0;
 
	
}

// zhoulidong  add for lcm detect (start)
static int rgk_lcm_compare_id(void)
{
	int data[4] = {0,0,0,0};
	int res = 0;
	int rawdata = 0;
	int lcm_vol = 0;


#ifdef AUXADC_LCM_VOLTAGE_CHANNEL
	res = IMM_GetOneChannelValue(AUXADC_LCM_VOLTAGE_CHANNEL,data,&rawdata);
	if(res < 0)
	{ 
	#ifdef BUILD_LK
		printf("[adc_uboot]: get data error\n");
	#endif
		return 0;
	}
#endif

	lcm_vol = data[0]*1000+data[1]*10;

#ifdef BUILD_LK
	printf("sym j5010  [adc_uboot]: lcm_vol= %d\n",lcm_vol);
#else
	printk("sym j5010 [adc_uboot]: lcm_vol= %d\n",lcm_vol);
#endif
	if( lcm_compare_id())
	      ;
	else
		return 0;
	
	if (lcm_vol>=MIN_VOLTAGE &&lcm_vol <= MAX_VOLTAGE) 
	{
		return 1;
	}
	else
		return 0;
}

// zhoulidong  add for lcm detect (end)


// zhoulidong add for eds(start)
static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_LK
	char  buffer[3];
	int   array[4];
	array[0] = 0x00013708;
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x0a, buffer, 1);
	printk("%s %d 0x0a=0x%x\n FALSE", __func__, __LINE__,buffer[0]);
	if(buffer[0]==0x9c)
	{
		#ifdef BUILD_LK
		printf("%s %d\n FALSE", __func__, __LINE__);
		#else
		printk("%s %d\n FALSE", __func__, __LINE__);
		#endif
		return FALSE;
	}
	else
	{	
		#ifdef BUILD_LK
		printf("%s %d\n FALSE", __func__, __LINE__);
		#else
		printk("%s %d\n FALSE", __func__, __LINE__);
		#endif		 
		return TRUE;
	}
#else
	//printk("lcm_esd_check()\n");
#endif 
}
#if 0
// zhoulidong add for eds(start)
static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_LK
	char  buffer[3];
	int   array[4];

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x0a, buffer, 1);
	if(buffer[0]==0x9c)
	{
		//#ifdef BUILD_LK
		//printf("%s %d\n FALSE", __func__, __LINE__);
		//#else
		//printk("%s %d\n FALSE", __func__, __LINE__);
		//#endif
		return FALSE;
	}
	else
	{	
		//#ifdef BUILD_LK
		//printf("%s %d\n FALSE", __func__, __LINE__);
		//#else
		//printk("%s %d\n FALSE", __func__, __LINE__);
		//#endif		 
		return TRUE;
	}
#endif

}
#endif

static unsigned int lcm_esd_recover(void)
{
	
	#ifdef BUILD_LK
		printf("lcm_esd_recover()\n");
	#else
		printk("lcm_esd_recover()\n");
	#endif	
	
	lcm_init();	

	return TRUE;
}
// zhoulidong add for eds(end)

LCM_DRIVER otm1283a_dsi_vdo_trust_hd720_cmi_ips_lcm_drv = 
{
	.name			= "otm1283a_dsi_vdo_trust_hd720_cmi_ips",
	.set_util_funcs		= lcm_set_util_funcs,
	.get_params		= lcm_get_params,
	.init			= lcm_init,
	.suspend		= lcm_suspend,
	.resume			= lcm_resume,
	.compare_id		= rgk_lcm_compare_id,
	.esd_check		= lcm_esd_check,
	.esd_recover		= lcm_esd_recover,
#if (LCM_DSI_CMD_MODE)
	//.set_backlight	= lcm_setbacklight,
	//.update		= lcm_update,
#endif
};
