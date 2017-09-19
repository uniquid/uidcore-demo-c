
OBJDIR := obj
EXEDIR := bin

LIBS		+= -lcurl -lpthread

EXTRA_LIBS		+= -Wl,-rpath='$$ORIGIN'
EXTRA_LIBS		+= -L./uidcore-c/trezor-crypto -ltrezor-crypto
EXTRA_LIBS		+= -L./uidcore-c -luidcore-c
EXTRA_LIBS		+= -L./paho.mqtt.c/build/output/ -lpaho-mqtt3c


CFLAGS := 
CFLAGS   += -W \
            -Wall \
            -Wextra \
            -Wimplicit-function-declaration \
            -Wredundant-decls \
            -Wstrict-prototypes \
            -Wundef \
            -Wshadow \
            -Wpointer-arith \
            -Wformat \
            -Wreturn-type \
            -Wsign-compare \
            -Wmultichar \
            -Wformat-nonliteral \
            -Winit-self \
            -Wuninitialized \
            -Wformat-security \
            -Werror

EXTRA_CFLAGS	+= -I./uidcore-c -I./paho.mqtt.c/src/ -std=gnu99

.PHONY: all
all: $(EXEDIR)/demo

$(EXEDIR)/demo: $(OBJDIR)/demo_init.o $(OBJDIR)/demo_provider.o $(OBJDIR)/demo_user.o $(OBJDIR)/mqtt_transport.o
	@mkdir -p $(EXEDIR)
	$(CC) -o $@ $^ $(EXTRA_LIBS) $(LIBS)
	# copy the libraries
	@cp uidcore-c/libuidcore-c.so $(EXEDIR)
	@cp uidcore-c/trezor-crypto/libtrezor-crypto.so $(EXEDIR)
	@cp paho.mqtt.c/build/output/libpaho-mqtt3c.so.1 $(EXEDIR)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

#
# Utility rules
#
.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -rf $(EXEDIR)


