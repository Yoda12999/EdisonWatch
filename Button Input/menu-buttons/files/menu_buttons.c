#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/io.h>
#include <asm/intel_scu_flis.h>

#define DRVNAME "menu_buttons"
#define NBUTTONS 3

/* 196 */
#define GPIO_46 tng_gp_kbd_dkin_2
/* 197 */
#define GPIO_47 tng_gp_kbd_dkin_3
/* 198 */
#define GPIO_48 tng_gp_kbd_mkin_0

static int verbose = 1;

static struct gpio_keys_button menu_buttons_table[NBUTTONS] = {
	{
		.desc = "Top Button",
		.gpio = 46,
		.code = KEY_UP,
		.type = EV_KEY,
		.active_low = true,
		.debounce_interval = 5, /* ms */
		.can_disable = true,
		.wakeup = 0,
		.irq = 0,
		.value = 0,
	},
	{
		.desc = "Center Button",
		.gpio = 47,
		.code = KEY_ENTER,
		.type = EV_KEY,
		.active_low = true,
		.debounce_interval = 5, /* ms */
		.can_disable = true,
		.wakeup = 1,
		.irq = 0,
		.value = 0,
	},
	{
		.desc = "Bottom Button",
		.gpio = 48,
		.code = KEY_DOWN,
		.type = EV_KEY,
		.active_low = true,
		.debounce_interval = 5, /* ms */
		.can_disable = true,
		.wakeup = 0,
		.irq = 0,
		.value = 0,
	}
};

static struct gpio_keys_platform_data pdata = {
	.buttons = menu_buttons_table,
	.nbuttons = NBUTTONS,
	.poll_interval = 20,
};

static void pdev_release(struct device *dev);

static struct platform_device menu_buttons = {
	.name = "gpio-keys",
	.dev  = {
		.release = pdev_release,
		.platform_data = &pdata,
	},
};

static void gpio_pull(unsigned int pud) {
	/* TODO: check for success */
	config_pin_flis(GPIO_46, PULL, pud ? UP_50K : DOWN_50K);
	config_pin_flis(GPIO_47, PULL, pud ? UP_50K : DOWN_50K);
	config_pin_flis(GPIO_48, PULL, pud ? UP_50K : DOWN_50K);
}

static void pdev_release(struct device *dev) {
	if (verbose > 1)
		pr_info(DRVNAME": %s()\n", __func__);

	gpio_pull(0);
}

static int __init menu_buttons_init(void) {
	int ret;

	if (verbose) {
		pr_info("\n\n"DRVNAME": %s()\n", __func__);
		pr_info(DRVNAME":   driver: %s\n", menu_buttons.name);
	}

	gpio_pull(1);

	ret = platform_device_register(&menu_buttons);
	if (ret < 0) {
		pr_err(DRVNAME": platform_device_register() returned %d\n", ret);
		return ret;
	}

	return 0;
}

static void __exit menu_buttons_exit(void) {
	if (verbose)
		pr_info(DRVNAME": %s()\n", __func__);

	platform_device_unregister(&menu_buttons);
}

module_init(menu_buttons_init);
module_exit(menu_buttons_exit);

MODULE_DESCRIPTION("Adds device for EdisonWatch menu buttons");
MODULE_AUTHOR("Gabriel Anders Smith");
MODULE_LICENSE("GPL");
