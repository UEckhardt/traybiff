# ![Logo](./src/icons/32x32/traybiff.png) TrayBiff 0.4.0
TrayBiff is a "biff" or new mail notification utility. It is configurable, easy to use and easy to setup. It embeds in
the tray bar of your window manger (currently only tested for KDE).

It supports several mail protocols: IMAP3, POP3, IMAP4, POP3 and IMAP4 with SSL, CRAM-MD5 and APOP authentication.

# Warning
**In the current step of development the passwords are stored as plain text!**

# Minimum Requirements for Builing
- CMake 3.25
- Qt 6.5.0
- libqsvgicon
- GCC or clang supporting C++17

# Clone, Build and Install

```
git clone https://github.com/UEckhardt/traybiff.git
mkdir build_traybiff
cd build_traybiff
cmake ../traybiff/
make
sudo make install
```

