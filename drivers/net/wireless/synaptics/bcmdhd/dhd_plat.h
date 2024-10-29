/*
 * DHD Linux platform header file
 *
 * Copyright (C) 2024 Synaptics Incorporated. All rights reserved.
 *
 * This software is licensed to you under the terms of the
 * GNU General Public License version 2 (the "GPL") with Broadcom special exception.
 *
 * INFORMATION CONTAINED IN THIS DOCUMENT IS PROVIDED "AS-IS," AND SYNAPTICS
 * EXPRESSLY DISCLAIMS ALL EXPRESS AND IMPLIED WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
 * AND ANY WARRANTIES OF NON-INFRINGEMENT OF ANY INTELLECTUAL PROPERTY RIGHTS.
 * IN NO EVENT SHALL SYNAPTICS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, PUNITIVE, OR CONSEQUENTIAL DAMAGES ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OF THE INFORMATION CONTAINED IN THIS DOCUMENT, HOWEVER CAUSED
 * AND BASED ON ANY THEORY OF LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, AND EVEN IF SYNAPTICS WAS ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE. IF A TRIBUNAL OF COMPETENT JURISDICTION
 * DOES NOT PERMIT THE DISCLAIMER OF DIRECT DAMAGES OR ANY OTHER DAMAGES,
 * SYNAPTICS' TOTAL CUMULATIVE LIABILITY TO ANY PARTY SHALL NOT
 * EXCEED ONE HUNDRED U.S. DOLLARS
 *
 * Copyright (C) 2024, Broadcom.
 *
 *      Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2 (the "GPL"),
 * available at http://www.broadcom.com/licenses/GPLv2.php, with the
 * following added to such license:
 *
 *      As a special exception, the copyright holders of this software give you
 * permission to link this software with independent modules, and to copy and
 * distribute the resulting executable under terms of your choice, provided that
 * you also meet, for each linked independent module, the terms and conditions of
 * the license of that module.  An independent module is a module which is not
 * derived from this software.  The special exception does not apply to any
 * modifications of the software.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id$
 */

#ifndef __DHD_PLAT_H__
#define __DHD_PLAT_H__

#if defined(__linux__)

#include <linuxver.h>

#if !defined(CONFIG_WIFI_CONTROL_FUNC)
#define WLAN_PLAT_NODFS_FLAG	0x01
#define WLAN_PLAT_AP_FLAG	0x02
struct wifi_platform_data {
	int (*set_power)(int val);
	int (*set_reset)(int val);
	int (*set_carddetect)(int val);
#ifdef DHD_COREDUMP
	int (*set_coredump)(const char *buf, int buf_len, const char *info);
#endif /* DHD_COREDUMP */
	void *(*mem_prealloc)(int section, unsigned long size);
	int (*get_mac_addr)(unsigned char *buf);
#ifdef DHD_USE_HOST_WAKE
	int (*get_wake_irq)(void);
	int (*get_oob_gpio_level)(void);
#endif /* DHD_USE_HOST_WAKE */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 58)) || defined(CUSTOM_COUNTRY_CODE)
	void *(*get_country_code)(char *ccode, u32 flags);
#else /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 58)) || defined (CUSTOM_COUNTRY_CODE) */
	void *(*get_country_code)(char *ccode);
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 58)) */
};
#endif /* CONFIG_WIFI_CONTROL_FUNC */

#include <linux/pci.h>

typedef void (*dhd_pcie_event_cb_t) (struct pci_dev *pdev);
extern int dhd_plat_pcie_register_event(void *plat_info,
               struct pci_dev *pdev, dhd_pcie_event_cb_t pfn);
extern void dhd_plat_pcie_deregister_event(void *plat_info);
extern void dhd_plat_report_bh_sched(void *plat_info, int resched);
extern int dhd_plat_pcie_suspend(void *plat_info);
extern int dhd_plat_pcie_resume(void *plat_info);
extern void dhd_plat_pcie_register_dump(void *plat_info);
extern void dhd_plat_pin_dbg_show(void *plat_info);

extern uint32 dhd_plat_get_info_size(void);
extern void dhd_plat_l1ss_ctrl(bool ctrl);

#include <linux/pci.h>

/* To be called when we intend to exit L1 while performing wreg, rreg operations */
extern void dhd_plat_l1_exit_io(void);

/* To be called when we intend to exit L1 in non-io case */
extern void dhd_plat_l1_exit(void);

extern uint32 dhd_plat_get_rc_vendor_id(void);
extern uint32 dhd_plat_get_rc_device_id(void);

#endif /* __linux__ */
#endif /* __DHD_PLAT_H__ */
