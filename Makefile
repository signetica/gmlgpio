# $FreeBSD$

.PATH:	${SRCTOP}/sys/dev/gpio
KMOD=	gmlgpio
SRCS=	gmlgpio.c
SRCS+=	acpi_if.h device_if.h bus_if.h gpio_if.h opt_acpi.h opt_platform.h

.include <bsd.kmod.mk>
