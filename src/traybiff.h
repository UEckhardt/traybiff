#ifndef _QT_BIFF_H
#define _QT_BIFF_H

#include <QApplication>
#include <QMessageLogger>
#include <QDebug>
#include <memory>
#include "protocols/CMailMonitor.h"
#include "systemtray/CTrayMenu.h"

#define __STR(s) #s
#define __XSTR(s) __STR(s)
#define VER_STR __XSTR(VER_MAJOR) "." __XSTR(VER_MINOR) "." __XSTR(VER_STEP)

typedef enum
{
  PROTO_POP3,
  PROTO_POP3S,
  PROTO_IMAP4,
  PROTO_IMAP3,
  PROTO_IMAPS,
  PROTO_LAST
} PROTOCOLS;

#endif
