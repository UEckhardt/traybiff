#ifndef TRAYBIFF_H_
#define TRAYBIFF_H_

#include <QApplication>
#include <QMessageLogger>
#include <QDebug>
#include <memory>
#include "protocols/CMailMonitor.h"
#include "systemtray/CTrayMenu.h"

#define TB_STR(s) #s
#define TB_XSTR(s) TB_STR(s)
#define VER_STR TB_XSTR(VER_MAJOR) "." TB_XSTR(VER_MINOR) "." TB_XSTR(VER_STEP)

typedef enum
{
  PROTO_POP3,
  PROTO_POP3S,
  PROTO_IMAP4,
  PROTO_IMAP3,
  PROTO_IMAPS,
  PROTO_LAST
} PROTOCOLS;

#endif /* TRAYBIFF_H_ */
