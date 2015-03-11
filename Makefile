# 

## Config
# Edit these to fit your system:
VERSION=0.2
PREFIX=/usr
MANDIR=$(PREFIX)/share/man/man1
BINDIR=$(PREFIX)/bin
SHAREDIR=$(PREFIX)/share/qemuctl
PIXMAPSDIR=$(PREFIX)/share/pixmaps
DOCSDIR=$(PREFIX)/share/doc/qemu-launcher
LOCALEDIR=$(PREFIX)/share/locale
APPLICATIONSDIR=$(PREFIX)/share/applications
## End config

SHELL=/bin/sh

DESTDIR=
EXECUTABLE = qemuctl
MANPAGE = qemuctl.1
DOCS=
POFILES= 

install: 
	install -D $(EXECUTABLE) $(DESTDIR)$(BINDIR)/$(EXECUTABLE)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m644 qemuctl.xml $(DESTDIR)$(SHAREDIR)/qemuctl.xml
	install -m644 -D $(MANPAGE) $(DESTDIR)$(MANDIR)/$(MANPAGE)
	gzip -f --best $(DESTDIR)$(MANDIR)/$(MANPAGE)

uninstall:
	rm $(DESTDIR)$(MANDIR)/$(MANPAGE).gz
	rm $(DESTDIR)$(BINDIR)/$(EXECUTABLE)
	rm -rf $(DESTDIR)$(SHAREDIR)

tar:
	rm -f qemuctl$(VERSION).tar.gz
	mkdir qemuctl$(VERSION) || true
	cp -f $(EXECUTABLE) qemuctl.xml $(MANPAGE) Makefile qemuctl$(VERSION)
	tar cvf qemuctl$(VERSION).tar.gz qemuctl$(VERSION)
