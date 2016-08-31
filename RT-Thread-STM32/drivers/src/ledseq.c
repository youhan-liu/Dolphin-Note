
/* LED��˸�������������е�actionʵ�ֵģ�ÿһ��action�����˶�Ӧ��LED��on/off(value)�͸�״̬��������ʱ�䣬
 * ����ʱ�䵽�����ܼ���ִ����һ��������
 * LED����˸���д洢��һ��list�У�ÿһ��sequence�е�״̬��ʱ���ǰ������ȼ�����(index)��ÿһ��sequence
 * ֻ�ܶ�һ��LED��Ч��
 * �ù���ʹ�����ĸ������ʱ������Ӧ�ĸ�LED���ĸ���ʱ������һ����ʱ�������ڳ�ʱ������ͨ���Դ�����ʱ����
 * �ľ����ַ�����ж���ȷ���ö�ʱ���Ƕ���һ��LED���в������ڴ˹�������Ҫʹ���ź�����ȷ���ڳ�ʱ�������ڴ���
 * ��ʱ��������ʱ�������˸ó�ʱ����������²��ᵼ�¶�ʱ�����ҡ�
 */
#include <stdbool.h>
#include "board.h"
#include "ledseq.h"
//#include "led.h"

static void* timer_led[LED_NUM];//����յ�ַָ�룬ָ���ĸ���ʱ�������ַ����ʡ�ռ�

/* Led sequence priority */
static ledseq_t * sequences[] = //LED��˸���ж���
{
	seq_testPassed,	//����ͨ��
	seq_lowbat,		//������
	seq_power_on,	//������
	seq_flash_80_waring,	//�����
	seq_flash_60_waring,	//bootloaderָʾ
	seq_armed,		//����
	seq_alwayson,//У׼��
	seq_error,	//У׼���
	seq_alive,		//��������
	seq_flash_3_times,		//���ӳɹ�
};

/* Led sequences */
ledseq_t seq_lowbat[] = 
{
	{ true, LEDSEQ_WAITMS(1000)},
	{    0, LEDSEQ_LOOP},
};

ledseq_t seq_armed[] = 
{
	{ true, LEDSEQ_WAITMS(500)},
	{false, LEDSEQ_WAITMS(500)},
	{ true, LEDSEQ_WAITMS(500)},
	{false, LEDSEQ_WAITMS(500)},
	{ true, LEDSEQ_WAITMS(500)},
	{false, LEDSEQ_WAITMS(500)},
	{    0, LEDSEQ_STOP},
};

ledseq_t seq_alwayson[] = 
{
	{ true, LEDSEQ_WAITMS(1000)},
	{    0, LEDSEQ_LOOP},
};

ledseq_t seq_error[] = 
{
	{ true, LEDSEQ_WAITMS(500)},
	{false, LEDSEQ_WAITMS(500)},
	{    0, LEDSEQ_LOOP},
};

ledseq_t seq_alive[] = 
{
	{ true, LEDSEQ_WAITMS(50)},
	{false, LEDSEQ_WAITMS(950)},
	{    0, LEDSEQ_LOOP},
};

ledseq_t seq_flash_3_times[] = 
{
	{false, LEDSEQ_WAITMS(500)},
	{ true, LEDSEQ_WAITMS(500)},
	{false, LEDSEQ_WAITMS(500)},
	{ true, LEDSEQ_WAITMS(500)},
	{false, LEDSEQ_WAITMS(500)},
	{ true, LEDSEQ_WAITMS(500)},
	{    1, LEDSEQ_STOP},//���һ��״̬��STOP������»ᱣ��value��ֵ
};


ledseq_t seq_power_on[] = 
{
	{ true, LEDSEQ_WAITMS(1000)},
	{    1, LEDSEQ_STOP},
};

ledseq_t seq_flash_80_waring[] = 
{
	{ true, LEDSEQ_WAITMS(500)},
	{false, LEDSEQ_WAITMS(500)},
	{ true, LEDSEQ_WAITMS(500)},
	{false, LEDSEQ_WAITMS(500)},
	{    0, LEDSEQ_STOP},
};

ledseq_t seq_flash_60_waring[] = 
{
	{ true, LEDSEQ_WAITMS(500)},
	{false, LEDSEQ_WAITMS(500)},
	{    0, LEDSEQ_STOP},
};

ledseq_t seq_testPassed[] = 
{
	{ true, LEDSEQ_WAITMS(50)},
	{false, LEDSEQ_WAITMS(50)},
	{ true, LEDSEQ_WAITMS(50)},
	{false, LEDSEQ_WAITMS(50)},
	{ true, LEDSEQ_WAITMS(50)},
	{false, LEDSEQ_WAITMS(50)},
	{ true, LEDSEQ_WAITMS(50)},
	{false, LEDSEQ_WAITMS(50)},
	{ true, LEDSEQ_WAITMS(50)},
	{false, LEDSEQ_WAITMS(50)},
	{ true, LEDSEQ_WAITMS(50)},
	{false, LEDSEQ_WAITMS(50)},
	{ true, LEDSEQ_WAITMS(50)},
	{false, LEDSEQ_WAITMS(50)},
	{false, LEDSEQ_STOP},
};

/* LED���д������ */
#define SEQ_NUM (sizeof(sequences)/sizeof(sequences[0]))

static void runLedseq(void * parameter);
static void updateActive(led_t led);
static int getPrio(ledseq_t *seq);

//State of every sequence for every led: LEDSEQ_STOP if stopped or the current step
static int state[LED_NUM][SEQ_NUM];
//Active sequence for each led
static int activeSeq[LED_NUM];
struct rt_semaphore ledseqSem;//����LED��˸�����ź��������ź�����FreeRTOS�е��ź�����������

static bool isInit = false;

void ledseq_init(void)
{
	int i = 0,j;

	if(isInit)
		return;
	/* initilize led */
	rt_hw_led_init();

	//��ʼ��led����
	for(i = 0; i < LED_NUM; i++)
	{
		activeSeq[i] = LEDSEQ_STOP;
		for(j = 0; j < SEQ_NUM; j ++)
			state[i][j] = LEDSEQ_STOP;
	}
	/* creat four soft timers for four LEDs .each timer use the same timeout function */
	for(i = 0; i < LED_NUM; i ++)
		timer_led[i] = rt_timer_create("ledseqTimer", 			/*��ʱ����ʶ  "ledseqTimer"*/
										runLedseq,				/*��ʱ�����ص�����*/
										(void *)&timer_led[i],	/*��ʱ������ڲ���*///���ｫÿһ����ʱ������ĵ�ַȡ���͸���ʱ����
										1000, 					/*��ʱ����*/
										RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);/*��ʱ��ʽΪ�����Զ�ʱ  �����ʱ*/
							
	rt_sem_init(&ledseqSem , "ledseqSem", 1, RT_IPC_FLAG_FIFO);//��ʼ���ź�������ʼֵΪ1�����ź����ᱻ��һ�����е��߳����

	isInit = true;
}

/* start up the flash function of one led */
void ledseq_run(led_t led, ledseq_t *sequence)
{
	int prio = getPrio(sequence);

	if(prio < 0) return;

	rt_sem_take(&ledseqSem, RT_WAITING_FOREVER);
	state[led][prio] = 0;  //Reset the seq. to its first step
	updateActive(led);
	rt_sem_release(&ledseqSem);

	//Run the first step if the new seq is the active sequence
	if(activeSeq[led] == prio)
		runLedseq(&timer_led[led]);
}
/* stop the flash function of current led */
void ledseq_stop(led_t led, ledseq_t *sequence)
{
	int prio = getPrio(sequence);

	if(prio < 0) return;
	/* take the semaphore */
	rt_sem_take(&ledseqSem, RT_WAITING_FOREVER);
	/* */
	state[led][prio] = LEDSEQ_STOP;  //Stop the seq.
	updateActive(led);
	/* release the semaphore */
	rt_sem_release(&ledseqSem);
	//Run the next active sequence (if any...)
	runLedseq(&timer_led[led]);
}

/* Center of the led sequence machine. This function is executed by the FreeRTOS
 * timer and runs the sequences.this function is the time-out-handle
 */
static void runLedseq(void *parameter)
{
	ledseq_t *step;
	led_t led;
	/* find out which timer trigger the time-out function by 
	   comparing the parameter with the address of the handler of timer*/
	if(parameter == &timer_led[0])
		led = LED_RED;
	
	else if(parameter == &timer_led[1])
		led = LED_GREEN;
	
	else if(parameter == &timer_led[2])
		led = LED_NET;
	
	else if(parameter == &timer_led[3])
		led = LED_SNP;
	
	else if(parameter == &timer_led[4])
		led = LED_REC;
	
	else if(parameter == &timer_led[5])
		led = BEEP;

	rt_timer_t *xTimer = (rt_timer_t *)parameter;//note:����˴��ݶ�ʱ�����������
	
	bool leave=false;
	
	while(!leave) 
	{
		int prio = activeSeq[led];

		if (prio == LEDSEQ_STOP)
			return;

		step = &sequences[prio][state[led][prio]];
		
		state[led][prio]++;
		/* take the semaphore */
		rt_sem_take(&ledseqSem, RT_WAITING_FOREVER);
		switch(step->action)
		{
			case LEDSEQ_LOOP:
				state[led][prio] = 0;
			break;
			case LEDSEQ_STOP:
				state[led][prio] = LEDSEQ_STOP;
				updateActive(led);
				ledSet(led, step->value);
			break;
			default:  //The step is a LED action and a time
				ledSet(led, step->value);
				if (step->action == 0)	break;
				/* change the period of current timer for active LED */
				rt_timer_control( *xTimer , RT_TIMER_CTRL_SET_TIME, (void*)&step->action);/**/
				/* start timer */
				rt_timer_start(*xTimer);
				leave=true;
			break;
		}
		/* release the semaphore */
		rt_sem_release(&ledseqSem);
	}
}

/* Utility functions */
static int getPrio(ledseq_t *seq)
{
	int prio;
	/* Find the priority of the sequence */
	for(prio = 0; prio < SEQ_NUM; prio ++)
		if(sequences[prio] == seq) 
			return prio;

	return -1; //Invalid sequence
}

static void updateActive(led_t led)
{
	int prio;

	activeSeq[led]=LEDSEQ_STOP;
	ledSet(led, false);

	for(prio = 0; prio < SEQ_NUM; prio ++)
	{
		if (state[led][prio] != LEDSEQ_STOP)
		{
			activeSeq[led] = prio;
			break;
		}
	}
}


