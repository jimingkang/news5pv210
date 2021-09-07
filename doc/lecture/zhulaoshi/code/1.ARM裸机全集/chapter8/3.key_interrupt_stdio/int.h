#ifndef __INT_H__
#define __INT_H__


#define exception_vector_table_base		0xD0037400
#define exception_reset					(exception_vector_table_base + 0x00)
#define exception_undef					(exception_vector_table_base + 0x04)
#define exception_sotf_int				(exception_vector_table_base + 0x08)
#define exception_prefetch				(exception_vector_table_base + 0x0C)
#define exception_data					(exception_vector_table_base + 0x10)
#define exception_irq					(exception_vector_table_base + 0x18)
#define exception_fiq					(exception_vector_table_base + 0x1C)

#define r_exception_reset		(*(volatile unsigned int *)exception_reset)
#define r_exception_undef		(*(volatile unsigned int *)exception_undef)
#define r_exception_sotf_int	(*(volatile unsigned int *)exception_sotf_int)
#define r_exception_prefetch	(*(volatile unsigned int *)exception_prefetch)
#define r_exception_data		(*(volatile unsigned int *)exception_data)
#define r_exception_irq			(*(volatile unsigned int *)exception_irq)
#define r_exception_fiq			(*(volatile unsigned int *)exception_fiq)







#endif
















