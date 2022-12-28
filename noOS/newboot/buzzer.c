/*buzzer.c*/
#define GPD0CON (*(volatile unsigned long *)0XE02000A0)
#define GPD0DAT (*(volatile unsigned long *)0XE02000A4)
void buzzer_init(void)
{
GPD0CON = (GPD0CON &~(0xf<<8)) | (0x1<<8);
GPD0DAT  = (GPD0CON &~(0x1<<2)) | (0x0<<2);
}
void buzzer_on(void)
{
GPD0DAT =(GPD0DAT &~(0x1<<2)) | (0x1<<2);
}
void buzzer_off(void)
{
GPD0DAT =(GPD0DAT & ~(0x1<<2)) | (0x0<<2);
}
