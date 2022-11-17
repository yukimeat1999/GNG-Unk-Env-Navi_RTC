/********************************************************
 * SerialPort.cpp
 *
 * Portable Serial Port Class Library for Windows and Unix.
 * @author ysuga@ysuga.net
 * @date 2010/11/02
 * @copyright Yuki Suga (ysuga.net), 2010 all rights reserved.
 ********************************************************/

/*************************************************
 * Header Including Division
 */
#ifdef WIN32

#else

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>
#define _POSIX_SOURCE 1

#endif

#include <iostream>
#include "SerialPort.h"

/* Header includeing division
 ************************************************/


using namespace net::ysuga;

/******************************
 */
SerialPort::SerialPort(const char* filename, const int32_t baudrate)
{
#ifdef WIN32
	DCB dcb;
	m_hComm = 0;
	m_hComm = CreateFileA(filename,	GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING,	FILE_ATTRIBUTE_NORMAL, NULL );
	if(m_hComm == INVALID_HANDLE_VALUE) {
		m_hComm = 0;
		throw ComOpenException();
	}


	this->FlushRxBuffer();

	if(!GetCommState (m_hComm, &dcb)) {
		CloseHandle(m_hComm); m_hComm = 0;
		throw ComStateException();
	}

	dcb.BaudRate           = baudrate;
    //dcb.fParity            = 0;
    //dcb.fOutxCtsFlow       = 0;
    //dcb.fOutxDsrFlow       = 0;
    dcb.fDtrControl        = RTS_CONTROL_DISABLE;
    //dcb.fDsrSensitivity    = 0;
   // dcb.fTXContinueOnXoff  = TRUE;
	dcb.XonLim              = 512;
	dcb.XoffLim             = 512;

    //dcb.fErrorChar         = 0;
    dcb.fNull              = 1;
    dcb.fRtsControl        = RTS_CONTROL_DISABLE;
    dcb.fAbortOnError      = TRUE;
    dcb.ByteSize           = 8;
    dcb.Parity             = NOPARITY;
    dcb.StopBits           = ONESTOPBIT;

	LPVOID lpMsgBuf;

	SetLastError(NO_ERROR);		//�G���[�����N���A����

//�����Ƀ`�F�b�N����������������



    if (!SetCommState (m_hComm, &dcb)) {
		CloseHandle(m_hComm); m_hComm = 0;
	  
		FormatMessage(				//�G���[�\��������쐬
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(),
			 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			 (LPTSTR) &lpMsgBuf, 0, NULL);

		MessageBox(NULL, (const char*)lpMsgBuf, NULL, MB_OK);	//���b�Z�[�W�\��

		LocalFree(lpMsgBuf);

      throw ComStateException();
    }



#else
  if((m_Fd = open(filename, O_RDWR /*| O_NOCTTY |O_NONBLOCK*/)) < 0) {
      throw ComOpenException();
  }
  struct termios tio;
  memset(&tio, 0, sizeof(tio));
  cfsetspeed(&tio, baudrate);
  tio.c_cflag |= CS8 | CLOCAL | CREAD;
  tcsetattr(m_Fd, TCSANOW, &tio);
#endif
}

/******************************
 */
SerialPort::~SerialPort()
{
#ifdef WIN32
	if(m_hComm) {
		CloseHandle(m_hComm);
	}
#else
	close(m_Fd);
#endif
}


/*******************************
 */
void SerialPort::FlushRxBuffer()
{
#ifdef WIN32
	if(!PurgeComm(m_hComm, PURGE_RXCLEAR)) {
		throw ComAccessException();
	}
#else
	if(tcflush( m_Fd, TCIFLUSH) < 0) {
		throw ComAccessException();
	}
#endif
}

/*******************************
 */
void SerialPort::FlushTxBuffer()
{
#ifdef WIN32
	if(!PurgeComm(m_hComm, PURGE_TXCLEAR)) {
		throw ComAccessException();
	}
#else
	if(tcflush(m_Fd, TCOFLUSH) < 0) {
		throw ComAccessException();
	}
#endif
}

/*******************************
 */
int SerialPort::GetSizeInRxBuffer()
{
#ifdef WIN32
    COMSTAT         stat;
    DWORD           lper;

	if(ClearCommError (m_hComm, &lper, &stat) == 0) {
		throw ComAccessException();
	}
    return stat.cbInQue;
#else
	struct timeval timeout;
	int nread;
	timeout.tv_sec = 0, timeout.tv_usec = 0;
	fd_set fds, dmy;
	FD_ZERO(&fds);
	FD_ZERO(&dmy);
	FD_SET(m_Fd, &fds);
	int res = select(FD_SETSIZE, &fds, &dmy, &dmy, &timeout);
	switch(res) {
	case 0: //timeout
		return 0;
	case -1: //Error
		throw ComAccessException();
	default:
		if(FD_ISSET(m_Fd, &fds)) {
		ioctl(m_Fd, FIONREAD, &nread);
			return nread;
		}
	}
	return 0;
#endif
}

/*******************************
 */
int SerialPort::Write(const void* src, const unsigned int size)
{
	if(size == 0) {
		return 0;
	}
#ifdef WIN32
	DWORD WrittenBytes = 0;
	if(!WriteFile(m_hComm, src, size, &WrittenBytes, NULL)) {
		throw ComAccessException();
	}

	return WrittenBytes;
#else
	int ret;
	if((ret = write(m_Fd, src, size)) < 0) {
		throw ComAccessException();
	}
	return ret;
#endif

}

/*******************************
 */
int SerialPort::Read(void *dst, const unsigned int size)
{
#ifdef WIN32
	DWORD ReadBytes;
	if(!ReadFile(m_hComm, dst, size, &ReadBytes, NULL)) {
	  throw ComAccessException();
	}

	return ReadBytes;
#else
	int ret;
	if((ret = read(m_Fd, dst, size))< 0) {
	  throw ComAccessException();
	}
	return ret;
#endif
}
