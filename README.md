# ![Logo](./src/icons/32x32/traybiff.png) TrayBiff 0.6.0
TrayBiff is a "biff" or new mail notification utility. It is configurable, easy to use and easy to setup. It embeds in
the tray bar of your window manager (currently only tested for KDE).

It supports several mail protocols: IMAP3, POP3, IMAP4, POP3 and IMAP4 with SSL, CRAM-MD5 and APOP authentication.

Passwords are stored in a Keychain e.g. the GNOME Keyring is used, or on KDE KWallet is used.

# Minimum Requirements for Building
- CMake 3.25
- Qt 6.5.0
- QtKeychain (Qt6) >= 0.14.0
- libqsvgicon
- GCC or clang supporting C++17

## Installing Build Dependencies

### openSUSE Leap 16.0
```bash
sudo zypper install cmake gcc-c++ make rpm-build \
  qt6-core-devel qt6-gui-devel qt6-network-devel qt6-widgets-devel \
  qtkeychain-qt6-devel libQt6Svg6
```

### Debian 13 (Trixie)
```bash
sudo apt install build-essential cmake \
  qt6-base-dev qtkeychain-qt6-dev qt6-svg-plugins
```

# Clone, Build and Install

```
git clone https://github.com/UEckhardt/traybiff.git
mkdir build_traybiff
cd build_traybiff
cmake -DCMAKE_BUILD_TYPE=Release ../traybiff/
make
make package
```

# Configuration

On first launch (when no mailbox is configured), TrayBiff automatically opens the **Setup** dialog. Once running in the system tray, you can open the configuration at any time via the tray icon's context menu (**Setup**).

## General Settings (`General` Tab)
- **Poll:** Interval in seconds between mail checks (`10`–`1000`, default: `360`).
- **Use session management:** Restores TrayBiff automatically when logging back into the desktop session.
- **Icons:** Click any state button (*Stopped*, *No Mailbox*, *No Mail*, *Old Mail*, *New Mail*) to select a custom icon (`*.png`, `*.jpg`, `*.bmp`). Use the button's **Reset** menu entry to restore the default theme icon.

## Mailboxes (`Mailbox` Tab)
You can configure and monitor multiple mail accounts:
- **Name:** Display identifier shown in the tray icon tooltip.
- **Protocol:** Select the mail protocol. Choosing a protocol automatically sets its default port:
  - `pop3` (default port `110`, uses `STLS` if supported by the server)
  - `pop3s` (default port `995`, implicit SSL/TLS)
  - `imap4` (default port `143`, uses `STARTTLS` if supported by the server)
  - `imap3` (default port `220`)
  - `imaps` (default port `993`, implicit SSL/TLS)
- **Server / Port:** Mail server hostname (or IP address) and port.
- **User / Password:** Account credentials. Passwords are never stored in plain text in the config file; they are saved in the desktop keychain (KDE KWallet or GNOME Keyring) via `QtKeychain`.
- **Mailbox:** IMAP folder name to monitor (e.g. `INBOX`), enabled for IMAP protocols.
- **Allow self-signed certificate:** Disables strict TLS certificate verification for servers using self-signed certificates (disabled by default).

Use the **Add** and **Delete** buttons below the server list to add additional mailboxes or remove existing ones, and confirm with **OK**.

## Configuration File & Command-Line Options
Settings (excluding passwords) are stored in `~/.config/uli-eckhardt/TrayBiff.conf`. The active configuration file path is also displayed in the **About** dialog.

| Option | Description |
| :--- | :--- |
| `-c, --alt-config <name>` | Use an alternate configuration file (`~/.config/uli-eckhardt/<name>.conf`). |
| `-d, --debug` | Run in the foreground and enable protocol debug logging to `~/.cache/traybiff.log`. |
| `-h, --help` | Display help on command-line options. |
| `-v, --version` | Display version information. |


