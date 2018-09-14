/*
 * Definitions for kxtf9 compass chip.
 */
#ifndef KXTF9_H
#define KXTF9_H

#include <linux/ioctl.h>

#define KXTF9IO                   0xA1

/* IOCTLs for kxtf9 library */
#define ECS_IOCTL_WRITE                 _IOW(KXTF9IO, 0x02, char[5])
#define ECS_IOCTL_READ                  _IOWR(KXTF9IO, 0x03, char[5])
#define ECS_IOCTL_GETDATA               _IOR(KXTF9IO, 0x08, char[RBUFF_SIZE])
#define ECS_IOCTL_SET_YPR               _IOW(KXTF9IO, 0x0C, short[12])
#define ECS_IOCTL_GET_OPEN_STATUS       _IOR(KXTF9IO, 0x0D, int)
#define ECS_IOCTL_GET_CLOSE_STATUS      _IOR(KXTF9IO, 0x0E, int)
#define ECS_IOCTL_GET_DELAY             _IOR(KXTF9IO, 0x30, short)

/* IOCTLs for APPs */
#define ECS_IOCTL_APP_SET_MFLAG		_IOW(KXTF9IO, 0x11, short)
#define ECS_IOCTL_APP_GET_MFLAG		_IOW(KXTF9IO, 0x12, short)
#define ECS_IOCTL_APP_SET_AFLAG		_IOW(KXTF9IO, 0x13, short)
#define ECS_IOCTL_APP_GET_AFLAG		_IOR(KXTF9IO, 0x14, short)
#define ECS_IOCTL_APP_SET_DELAY		_IOW(KXTF9IO, 0x18, short)
#define ECS_IOCTL_APP_GET_DELAY		ECS_IOCTL_GET_DELAY
/* Set raw magnetic vector flag */
#define ECS_IOCTL_APP_SET_MVFLAG	_IOW(KXTF9IO, 0x19, short)
/* Get raw magnetic vector flag */
#define ECS_IOCTL_APP_GET_MVFLAG	_IOR(KXTF9IO, 0x1A, short)
#define ECS_IOCTL_APP_SET_TFLAG         _IOR(KXTF9IO, 0x15, short)

#endif

