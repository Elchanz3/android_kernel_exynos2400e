# SPDX-License-Identifier: GPL-2.0
dtb-y += exynos/s5e5515_nocp.dtb
dtbo-y += samsung/exynos5515-lucky7bs_na_open_00.dtbo

targets += dtbs
DTB_LIST  := $(dtb-y) $(dtbo-y)
always    := $(DTB_LIST)

dtbs: $(addprefix $(obj)/, $(DTB_LIST))

clean-files := *.dtb*
