/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2017 Tom Jones <tj@enoti.me>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 * Copyright (c) 2016 Mark Kettenis
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/gpio.h>
#include <sys/clock.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/endian.h>
#include <sys/rman.h>
#include <sys/types.h>
#include <sys/malloc.h>

#include <machine/bus.h>
#include <machine/resource.h>

#include <contrib/dev/acpica/include/acpi.h>
#include <contrib/dev/acpica/include/accommon.h>

#include <dev/acpica/acpivar.h>
#include <dev/gpio/gpiobusvar.h>

#include "opt_platform.h"
#include "opt_acpi.h"
#include "gpio_if.h"

#include "gmlgpio_reg.h"

/*
 *     Macros for driver mutex locking
 */
#define GMLGPIO_LOCK(_sc)               mtx_lock_spin(&(_sc)->sc_mtx)
#define GMLGPIO_UNLOCK(_sc)             mtx_unlock_spin(&(_sc)->sc_mtx)
#define GMLGPIO_LOCK_INIT(_sc) \
	mtx_init(&_sc->sc_mtx, device_get_nameunit((_sc)->sc_dev), \
	"gmlgpio", MTX_SPIN)
#define GMLGPIO_LOCK_DESTROY(_sc)       mtx_destroy(&(_sc)->sc_mtx)
#define GMLGPIO_ASSERT_LOCKED(_sc)      mtx_assert(&(_sc)->sc_mtx, MA_OWNED)
#define GMLGPIO_ASSERT_UNLOCKED(_sc) 	mtx_assert(&(_sc)->sc_mtx, MA_NOTOWNED)

struct gmlgpio_softc {
	device_t 	sc_dev;
	device_t 	sc_busdev;
	struct mtx 	sc_mtx;

	ACPI_HANDLE	sc_handle;

	int		sc_mem_rid;
	struct resource *sc_mem_res;

	int		sc_irq_rid;
	struct resource *sc_irq_res;
	void		*intr_handle;

	const char	*sc_bank_prefix;
	const int  	*sc_pins;
	int 		sc_npins;
	int 		sc_ngroups;
	int		sc_padbar;
	const char **sc_pin_names;
};

static void gmlgpio_intr(void *);
static int gmlgpio_probe(device_t);
static int gmlgpio_attach(device_t);
static int gmlgpio_detach(device_t);

static inline int
gmlgpio_read_padbar(struct gmlgpio_softc *sc)
{
	return bus_read_4(sc->sc_mem_res, GML_PADBAR);
}

static inline int
gmlgpio_pad_cfg_dw0_offset(struct gmlgpio_softc *sc, int pin)
{
	return (sc->sc_padbar + 0x010 * pin);
}

static inline int
gmlgpio_read_pad_cfg_dw0(struct gmlgpio_softc *sc, int pin)
{
	return bus_read_4(sc->sc_mem_res, gmlgpio_pad_cfg_dw0_offset(sc, pin));
}

static inline void
gmlgpio_write_pad_cfg_dw0(struct gmlgpio_softc *sc, int pin, uint32_t val)
{
	bus_write_4(sc->sc_mem_res, gmlgpio_pad_cfg_dw0_offset(sc, pin), val);
}

#ifdef notdef		/* Unused for now */
static inline int
gmlgpio_read_pad_cfg_dw1(struct gmlgpio_softc *sc, int pin)
{
	return bus_read_4(sc->sc_mem_res, gmlgpio_pad_cfg_dw0_offset(sc, pin) + 4);
}
#endif

static device_t
gmlgpio_get_bus(device_t dev)
{
	struct gmlgpio_softc *sc;

	sc = device_get_softc(dev);

	return (sc->sc_busdev);
}

static int
gmlgpio_pin_max(device_t dev, int *maxpin)
{
	struct gmlgpio_softc *sc;

	sc = device_get_softc(dev);

	*maxpin = sc->sc_npins - 1;

	return (0);
}

static int
gmlgpio_valid_pin(struct gmlgpio_softc *sc, int pin)
{
	if (pin < 0 || pin >= sc->sc_npins)
		return EINVAL;
	return (0);
}

static int
gmlgpio_pin_getname(device_t dev, uint32_t pin, char *name)
{
	struct gmlgpio_softc *sc;

	sc = device_get_softc(dev);
	if (gmlgpio_valid_pin(sc, pin) != 0)
		return (EINVAL);

	/* return pin name from datasheet */
	snprintf(name, GPIOMAXNAME, "%s", sc->sc_pin_names[pin]);
	name[GPIOMAXNAME - 1] = '\0';
	return (0);
}

static int
gmlgpio_pin_getcaps(device_t dev, uint32_t pin, uint32_t *caps)
{
	struct gmlgpio_softc *sc;

	sc = device_get_softc(dev);
	if (gmlgpio_valid_pin(sc, pin) != 0)
		return (EINVAL);

	/* Fixed capabilities */
	*caps = GPIO_PIN_INPUT | GPIO_PIN_OUTPUT;

	return (0);
}

static int
gmlgpio_pin_getflags(device_t dev, uint32_t pin, uint32_t *flags)
{
	struct gmlgpio_softc *sc;
	uint32_t val;

	sc = device_get_softc(dev);
	if (gmlgpio_valid_pin(sc, pin) != 0)
		return (EINVAL);

	*flags = 0;

	/* Get the current pin state */
	GMLGPIO_LOCK(sc);
	val = gmlgpio_read_pad_cfg_dw0(sc, pin);

	if (!(val & GML_GPIO_PAD_CFG_DW0_GPIOTXDIS))
		*flags |= GPIO_PIN_OUTPUT;

	if (!(val & GML_GPIO_PAD_CFG_DW0_GPIORXDIS))
		*flags |= GPIO_PIN_INPUT;

	GMLGPIO_UNLOCK(sc);
	return (0);
}

static int
gmlgpio_pin_setflags(device_t dev, uint32_t pin, uint32_t flags)
{
	struct gmlgpio_softc *sc;
	uint32_t val;
	uint32_t allowed;

	sc = device_get_softc(dev);
	if (gmlgpio_valid_pin(sc, pin) != 0)
		return (EINVAL);

	allowed = GPIO_PIN_INPUT | GPIO_PIN_OUTPUT;

	/*
	 * Only direction flag allowed
	 */
	if (flags & ~allowed)
		return (EINVAL);

	/*
	 * The hardware supports bidirectional mode, but gpiobus.c prohibits it.
	 * We support it here but it cannot be activated without changing
	 * gpiobus.c.  Some pins have it activated by default.
	 */
#ifdef GPIO_NO_BIDIRECTIONAL
	/*
	 * Not both directions simultaneously
	 */
	if ((flags & allowed) == allowed)
		return (EINVAL);
#endif

	/* Set the GPIO mode and state */
	GMLGPIO_LOCK(sc);
	val = gmlgpio_read_pad_cfg_dw0(sc, pin);
	if (flags & GPIO_PIN_INPUT)
		val &= ~GML_GPIO_PAD_CFG_DW0_GPIORXDIS;
	else
		val |= GML_GPIO_PAD_CFG_DW0_GPIORXDIS;
	if (flags & GPIO_PIN_OUTPUT)
		val &= ~GML_GPIO_PAD_CFG_DW0_GPIOTXDIS;
	else
		val |= GML_GPIO_PAD_CFG_DW0_GPIOTXDIS;
	gmlgpio_write_pad_cfg_dw0(sc, pin, val);
	GMLGPIO_UNLOCK(sc);

	return (0);
}

static int
gmlgpio_pin_set(device_t dev, uint32_t pin, unsigned int value)
{
	struct gmlgpio_softc *sc;
	uint32_t val;

	sc = device_get_softc(dev);
	if (gmlgpio_valid_pin(sc, pin) != 0)
		return (EINVAL);

	GMLGPIO_LOCK(sc);

	val = gmlgpio_read_pad_cfg_dw0(sc, pin);
	if (val & GML_GPIO_PAD_CFG_DW0_GPIOTXDIS) {
		GMLGPIO_UNLOCK(sc);
		return (EPERM);
	}

	if (value == GPIO_PIN_LOW)
		val &= ~GML_GPIO_PAD_CFG_DW0_GPIOTXSTATE;
	else
		val |= GML_GPIO_PAD_CFG_DW0_GPIOTXSTATE;
	gmlgpio_write_pad_cfg_dw0(sc, pin, val);

	GMLGPIO_UNLOCK(sc);

	return (0);
}

static int
gmlgpio_pin_get(device_t dev, uint32_t pin, unsigned int *value)
{
	struct gmlgpio_softc *sc;
	uint32_t val;

	sc = device_get_softc(dev);
	if (gmlgpio_valid_pin(sc, pin) != 0)
		return (EINVAL);

	/*
	 * Read pin value:
	 * If RXDIS is not set, read RXSTATE.
	 * If RXDIS is set, read TXSTATE.
	 */
	GMLGPIO_LOCK(sc);
	val = gmlgpio_read_pad_cfg_dw0(sc, pin);
	if (!(val & GML_GPIO_PAD_CFG_DW0_GPIORXDIS))
		if (val & GML_GPIO_PAD_CFG_DW0_GPIORXSTATE)
			*value = GPIO_PIN_HIGH;
		else
			*value = GPIO_PIN_LOW;
	else
		if (val & GML_GPIO_PAD_CFG_DW0_GPIOTXSTATE)
			*value = GPIO_PIN_HIGH;
		else
			*value = GPIO_PIN_LOW;
	GMLGPIO_UNLOCK(sc);

	return (0);
}

static int
gmlgpio_pin_toggle(device_t dev, uint32_t pin)
{
	struct gmlgpio_softc *sc;
	uint32_t val;

	sc = device_get_softc(dev);
	if (gmlgpio_valid_pin(sc, pin) != 0)
		return (EINVAL);

	GMLGPIO_LOCK(sc);

	val = gmlgpio_read_pad_cfg_dw0(sc, pin);
	if (val & GML_GPIO_PAD_CFG_DW0_GPIOTXDIS) {
		GMLGPIO_UNLOCK(sc);
		return (EPERM);
	}

	/* Toggle the pin */
	val = val ^ GML_GPIO_PAD_CFG_DW0_GPIOTXSTATE;
	gmlgpio_write_pad_cfg_dw0(sc, pin, val);

	GMLGPIO_UNLOCK(sc);

	return (0);
}

static char *gmlgpio_hids[] = {
	"INT3453",
	NULL
};

static int
gmlgpio_probe(device_t dev)
{
	int rv;
    
	if (acpi_disabled("gmlgpio"))
		return (ENXIO);
	rv = ACPI_ID_PROBE(device_get_parent(dev), dev, gmlgpio_hids, NULL);
	if (rv <= 0)
		device_set_desc(dev, "Intel Gemini Lake GPIO");
	return (rv);
}

static int
gmlgpio_attach(device_t dev)
{
	struct gmlgpio_softc *sc;
	ACPI_STATUS status;
	int uid;
	int i;
	int error;
	bus_size_t offset;

	sc = device_get_softc(dev);
	sc->sc_dev = dev;
	sc->sc_handle = acpi_get_handle(dev);

	status = acpi_GetInteger(sc->sc_handle, "_UID", &uid);
	if (ACPI_FAILURE(status)) {
		device_printf(dev, "failed to read _UID\n");
		return (ENXIO);
	}

	GMLGPIO_LOCK_INIT(sc);

	switch (uid) {
	case NW_UID:
		sc->sc_bank_prefix = NW_BANK_PREFIX;
		sc->sc_pins = gml_northwest_pins;
		sc->sc_pin_names = gml_northwest_pin_names;
		break;
	case N_UID:
		sc->sc_bank_prefix = N_BANK_PREFIX;
		sc->sc_pins = gml_north_pins;
		sc->sc_pin_names = gml_north_pin_names;
		break;
	case AUDIO_UID:
		sc->sc_bank_prefix = AUDIO_BANK_PREFIX;
		sc->sc_pins = gml_audio_pins;
		sc->sc_pin_names = gml_audio_pin_names;
		break;
	case SCC_UID:
		sc->sc_bank_prefix = SCC_BANK_PREFIX;
		sc->sc_pins = gml_scc_pins;
		sc->sc_pin_names = gml_scc_pin_names;
		break;
	default:
		device_printf(dev, "invalid _UID value: %d\n", uid);
		return (ENXIO);
	}

	for (i = 0; sc->sc_pins[i] >= 0; i++) {
		sc->sc_npins += sc->sc_pins[i];
		sc->sc_ngroups++;
	}

	sc->sc_mem_rid = 0;
	sc->sc_mem_res = bus_alloc_resource_any(sc->sc_dev, SYS_RES_MEMORY,
	    &sc->sc_mem_rid, RF_ACTIVE);
	if (sc->sc_mem_res == NULL) {
		GMLGPIO_LOCK_DESTROY(sc);
		device_printf(dev, "can't allocate memory resource\n");
		return (ENOMEM);
	}

	sc->sc_irq_res = bus_alloc_resource_any(dev, SYS_RES_IRQ,
	    &sc->sc_irq_rid, RF_ACTIVE | RF_SHAREABLE);

	if (!sc->sc_irq_res) {
		GMLGPIO_LOCK_DESTROY(sc);
		bus_release_resource(dev, SYS_RES_MEMORY,
		    sc->sc_mem_rid, sc->sc_mem_res);
		device_printf(dev, "can't allocate irq resource\n");
		return (ENOMEM);
	}

	error = bus_setup_intr(sc->sc_dev, sc->sc_irq_res, INTR_TYPE_MISC | INTR_MPSAFE,
	    NULL, gmlgpio_intr, sc, &sc->intr_handle);


	if (error) {
		device_printf(sc->sc_dev, "unable to setup irq: error %d\n", error);
		GMLGPIO_LOCK_DESTROY(sc);
		bus_release_resource(dev, SYS_RES_MEMORY,
		    sc->sc_mem_rid, sc->sc_mem_res);
		bus_release_resource(dev, SYS_RES_IRQ,
		    sc->sc_irq_rid, sc->sc_irq_res);
		return (ENXIO);
	}

	/* Mask and ack all interrupts. Smaller communities reserve unused registers. */
	for (offset = 0; offset <= GML_GPI_IS_3 - GML_GPI_IS_0; offset += 4) {
		bus_write_4(sc->sc_mem_res, GML_GPI_IE_0 + offset, 0);
		bus_write_4(sc->sc_mem_res, GML_GPI_IS_0 + offset, 0xffffffff);
	}

	sc->sc_busdev = gpiobus_attach_bus(dev);
	if (sc->sc_busdev == NULL) {
		GMLGPIO_LOCK_DESTROY(sc);
		bus_release_resource(dev, SYS_RES_MEMORY,
		    sc->sc_mem_rid, sc->sc_mem_res);
		bus_release_resource(dev, SYS_RES_IRQ,
		    sc->sc_irq_rid, sc->sc_irq_res);
		return (ENXIO);
	}

	/* Get PAD base address */
	sc->sc_padbar = gmlgpio_read_padbar(sc);

	return (0);
}

/* Clear incoming interrupts */
static void
gmlgpio_intr(void *arg)
{
	struct gmlgpio_softc *sc = arg;
	uint32_t reg;
	int line;
	bus_size_t offset;

	for (offset = GML_GPI_IS_0; offset <= GML_GPI_IS_3; offset += 4) {
		reg = bus_read_4(sc->sc_mem_res, offset);
		for (line = 0; line < 32; line++) {
			if ((reg & (1 << line)) == 0)
				continue;
			bus_write_4(sc->sc_mem_res, offset, 1 << line);
			device_printf(sc->sc_dev, "cleared interrupt on gpio bit %d\n",
			    (int)offset * 32 + line);
		}
	}
}

static int
gmlgpio_detach(device_t dev)
{
	struct gmlgpio_softc *sc;
	sc = device_get_softc(dev);

	if (sc->sc_busdev)
		gpiobus_detach_bus(dev);

	if (sc->intr_handle != NULL)
		bus_teardown_intr(sc->sc_dev, sc->sc_irq_res, sc->intr_handle);
	if (sc->sc_irq_res != NULL)
		bus_release_resource(dev, SYS_RES_IRQ, sc->sc_irq_rid, sc->sc_irq_res);
	if (sc->sc_mem_res != NULL)
		bus_release_resource(dev, SYS_RES_MEMORY, sc->sc_mem_rid,
		    sc->sc_mem_res);

	GMLGPIO_LOCK_DESTROY(sc);

    return (0);
}

static device_method_t gmlgpio_methods[] = {
	DEVMETHOD(device_probe,     	gmlgpio_probe),
	DEVMETHOD(device_attach,    	gmlgpio_attach),
	DEVMETHOD(device_detach,    	gmlgpio_detach),

	/* GPIO protocol */
	DEVMETHOD(gpio_get_bus, 	gmlgpio_get_bus),
	DEVMETHOD(gpio_pin_max, 	gmlgpio_pin_max),
	DEVMETHOD(gpio_pin_getname, 	gmlgpio_pin_getname),
	DEVMETHOD(gpio_pin_getflags,	gmlgpio_pin_getflags),
	DEVMETHOD(gpio_pin_getcaps, 	gmlgpio_pin_getcaps),
	DEVMETHOD(gpio_pin_setflags,	gmlgpio_pin_setflags),
	DEVMETHOD(gpio_pin_get, 	gmlgpio_pin_get),
	DEVMETHOD(gpio_pin_set, 	gmlgpio_pin_set),
	DEVMETHOD(gpio_pin_toggle, 	gmlgpio_pin_toggle),

	DEVMETHOD_END
};

static driver_t gmlgpio_driver = {
    .name = "gpio",
    .methods = gmlgpio_methods,
    .size = sizeof(struct gmlgpio_softc)
};

DRIVER_MODULE(gmlgpio, acpi, gmlgpio_driver, NULL , NULL);
MODULE_DEPEND(gmlgpio, acpi, 1, 1, 1);
MODULE_DEPEND(gmlgpio, gpiobus, 1, 1, 1);

MODULE_VERSION(gmlgpio, 1);
